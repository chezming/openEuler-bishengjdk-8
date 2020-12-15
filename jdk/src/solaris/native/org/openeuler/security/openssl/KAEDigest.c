/*
 * Copyright (c) 2020, Huawei Technologies Co., Ltd. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <openssl/evp.h>
#include <openssl/md5.h>
#include "kae_util.h"
#include "org_openeuler_security_openssl_KAEDigest.h"

#define DIGEST_STACK_SIZE 1024
#define DIGEST_CHUNK_SIZE 64*1024

/*
 * Class:     org_openeuler_security_openssl_KAEDigest
 * Method:    nativeInit
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_openeuler_security_openssl_KAEDigest_nativeInit
        (JNIEnv *env, jclass cls, jstring algorithmName) {
    if (algorithmName == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return 0;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    if (ctx == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, EVP_MD_CTX_CREATE_ERROR);
        return 0;
    }

    // EVP_get_digestbyname
    const char *algo_utf = (*env)->GetStringUTFChars(env, algorithmName, 0);
    if (algo_utf == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, GETSTRINGUTFCHARS_ERROR);
        return 0;
    }
    EVP_MD *md = (EVP_MD *) EVP_get_digestbyname(algo_utf);
    (*env)->ReleaseStringUTFChars(env, algorithmName, algo_utf);
    if (md == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, EVP_GET_DIGESTBYNAME_ERROR);
        return 0;
    }

    // EVP_DigestInit_ex
    int result_code = EVP_DigestInit_ex(ctx, md, NULL);
    if (result_code == 0) {
        EVP_MD_CTX_destroy(ctx);
        KAE_ThrowDigestExceptionByErrcode(env, EVP_DIGESTINIT_EX_ERROR);
        return 0;
    }
    return (jlong) ctx;
}

/*
 * Class:     org_openeuler_security_openssl_KAEDigest
 * Method:    nativeUpdate
 * Signature: (Ljava/lang/String;J[BII)I
 */
JNIEXPORT void JNICALL Java_org_openeuler_security_openssl_KAEDigest_nativeUpdate
        (JNIEnv *env, jclass cls, jlong ctxAddress, jbyteArray input, jint offset, jint inLen) {
    // return when inLen is 0
    if (inLen == 0) {
        return;
    }
    if (input == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return;
    }
    int inputLen = (*env)->GetArrayLength(env, input);
    if ((offset < 0) || (inLen < 0) || (offset > inputLen - inLen)) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return;
    }
    EVP_MD_CTX *ctx = (EVP_MD_CTX *) ctxAddress;
    if (ctx == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return;
    }

    jint in_offset = offset;
    jint in_size = inLen;
    int result_code = 0;
    if (in_size <= DIGEST_STACK_SIZE) { // allocation on the stack
        jbyte buffer[DIGEST_STACK_SIZE];
        (*env)->GetByteArrayRegion(env, input, offset, inLen, buffer);
        result_code = EVP_DigestUpdate(ctx, buffer, inLen);
    } else { // data chunk
        jint remaining = in_size;
        jint buf_size = (remaining >= DIGEST_CHUNK_SIZE) ? DIGEST_CHUNK_SIZE : remaining;
        jbyte *buffer = malloc(buf_size);
        if (buffer == NULL) {
            KAE_ThrowDigestExceptionByErrcode(env, MALLOC_ERROR);
            return;
        }
        while (remaining > 0) {
            jint chunk_size = (remaining >= buf_size) ? buf_size : remaining;
            (*env)->GetByteArrayRegion(env, input, in_offset, chunk_size, buffer);
            result_code = EVP_DigestUpdate(ctx, buffer, chunk_size);
            if (!result_code) {
                break;
            }
            in_offset += chunk_size;
            remaining -= chunk_size;
        }
        free(buffer);
    }
    if (!result_code) {
        KAE_ThrowDigestExceptionByErrcode(env, EVP_DIGESTUPDATE_ERROR);
        return;
    }
}

/*
 * Class:     org_openeuler_security_openssl_KAEDigest
 * Method:    nativeDigest
 * Signature: (Ljava/lang/String;J[BII)I
 */
JNIEXPORT jint JNICALL Java_org_openeuler_security_openssl_KAEDigest_nativeDigest
        (JNIEnv *env, jclass cls, jlong ctxAddress, jbyteArray output, jint offset, jint len) {
    EVP_MD_CTX *ctx = (EVP_MD_CTX *) ctxAddress;
    if (ctx == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return 0;
    }
    if (output == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return 0;
    }
    int outputLen = (*env)->GetArrayLength(env, output);
    if ((offset < 0) || (len < 0) || (offset > outputLen - len)) {
        KAE_ThrowDigestExceptionByErrcode(env, BUFFER_TOO_SHORT);
        return 0;
    }

    unsigned char *md = malloc(len);
    if (md == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, MALLOC_ERROR);
        return 0;
    }

    // EVP_DigestFinal_ex
    unsigned int bytesWritten = 1;
    int result_code = EVP_DigestFinal_ex(ctx, md, &bytesWritten);
    if (result_code == 0) {
        free(md);
        KAE_ThrowDigestExceptionByErrcode(env, EVP_DIGESTFINAL_EX_ERROR);
        return 0;
    }
    (*env)->SetByteArrayRegion(env, output, offset, bytesWritten, (jbyte *) md);
    free(md);
    return bytesWritten;
}

/*
* Class:     org_openeuler_security_openssl_KAEDigest
* Method:    nativeClone
* Signature: (J)J
*/
JNIEXPORT jlong JNICALL Java_org_openeuler_security_openssl_KAEDigest_nativeClone
        (JNIEnv *env, jclass cls, jlong ctxAddress) {
    EVP_MD_CTX *ctx = (EVP_MD_CTX *) ctxAddress;
    if (ctx == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, INPUT_PARAM_EMPTY_OR_INVALID_ERROR);
        return 0;
    }

    EVP_MD_CTX *ctxCopy = EVP_MD_CTX_create();
    if (ctxCopy == NULL) {
        KAE_ThrowDigestExceptionByErrcode(env, EVP_MD_CTX_CREATE_ERROR);
        return 0;
    }

    int result_code = EVP_MD_CTX_copy_ex(ctxCopy, ctx);
    if (result_code == 0) {
        EVP_MD_CTX_destroy(ctxCopy);
        KAE_ThrowDigestExceptionByErrcode(env, EVP_MD_CTX_COPY_EX_ERROR);
        return 0;
    }
    return (jlong) ctxCopy;
}

/*
 * Class:     org_openeuler_security_openssl_KAEDigest
 * Method:    nativeFree
 * Signature: (Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_org_openeuler_security_openssl_KAEDigest_nativeFree
        (JNIEnv *env, jclass cls, jlong ctxAddress) {
    EVP_MD_CTX *ctx = (EVP_MD_CTX *) ctxAddress;
    if (ctx != NULL) {
        EVP_MD_CTX_destroy(ctx);
    }
}