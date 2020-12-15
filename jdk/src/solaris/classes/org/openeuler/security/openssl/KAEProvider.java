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

package org.openeuler.security.openssl;

import java.security.Provider;
import java.security.ProviderException;

/**
 * KAE Provider
 */
public class KAEProvider extends Provider {

    static {
        System.loadLibrary("j2kae");

        boolean useKae = false;
        try {
            initOpenssl();
            useKae = true;
        } catch (ProviderException e) {
            System.out.println(e.getMessage());
        }

        if (useKae) {
            System.out.println("KAE Engine was found");
        }
    }

    public KAEProvider() {
        super("KAEProvider", 1.8d, "KAE provider");
        put("MessageDigest.MD5", "org.openeuler.security.openssl.KAEDigest$MD5");
    }

    // init openssl
    static native int initOpenssl() throws RuntimeException;
}