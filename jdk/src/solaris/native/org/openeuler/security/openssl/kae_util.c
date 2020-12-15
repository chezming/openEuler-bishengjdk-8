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

#include "kae_util.h"

void KAE_ThrowByName(JNIEnv *env, const char *name, const char *msg) {
    jclass cls = (*env)->FindClass(env, name);
    if (cls != 0) {
        (*env)->ThrowNew(env, cls, msg);
    }
}

void KAE_ThrowDigestException(JNIEnv *env, const char *msg) {
    KAE_ThrowByName(env, "java/security/DigestException", msg);
}

void KAE_ThrowDigestExceptionByErrcode(JNIEnv *env, enum KAEErrorCode errcode) {
    const char *errmsg = GetErrorMessage(errcode);
    KAE_ThrowDigestException(env, errmsg);
}

void KAE_ThrowProviderException(JNIEnv *env, const char *msg) {
    KAE_ThrowByName(env, "java/security/ProviderException", msg);
}

void KAE_ThrowProviderExceptionByErrCode(JNIEnv *env, enum KAEErrorCode errcode) {
    const char *errmsg = GetErrorMessage(errcode);
    KAE_ThrowProviderException(env, errmsg);
}