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

#ifndef KAE_ERRNO_H
#define KAE_ERRNO_H

#define UNKNOWN_ERROR "Unknown error"

// errno define
enum KAEErrorCode {
    SUCCESS = 0,
    KAE_ENGINE_NOT_FOUND_ERROR = -1,
    INPUT_PARAM_EMPTY_OR_INVALID_ERROR = -2,
    MALLOC_ERROR = -3,
    BUFFER_TOO_SHORT = -4,
    GETSTRINGUTFCHARS_ERROR = -5,
    EVP_MD_CTX_CREATE_ERROR = -6,
    EVP_GET_DIGESTBYNAME_ERROR = -7,
    EVP_DIGESTINIT_EX_ERROR = -8,
    EVP_DIGESTUPDATE_ERROR = -9,
    EVP_DIGESTFINAL_EX_ERROR = -10,
    EVP_MD_CTX_COPY_EX_ERROR = -11
};

// errmsg define
static const char *errorMessage[] = {
        "",
        "KAE engine not found, use the default openssl engine",
        "Input parameter is empty or invalid",
        "invoke malloc error",
        "Buffer too short to store digest",
        "Invoke GetStringUTFChars failed",
        "Invoke EVP_MD_CTX_create failed",
        "Invoke EVP_get_digestbyname failed",
        "Invoke EVP_DigestInit_ex failed",
        "Invoke EVP_DigestUpdate failed",
        "Invoke EVP_DigestFinal_ex failed"
};

static int errorMessageLen = sizeof(errorMessage) / sizeof(char *);

// get errmsg
const char *GetErrorMessage(int errorCode);

#endif