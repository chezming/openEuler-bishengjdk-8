/*
 * Copyright (c) 2003, 2014, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2020, Huawei Technologies Co., Ltd. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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

import java.lang.ref.PhantomReference;
import java.lang.ref.ReferenceQueue;
import java.security.DigestException;
import java.security.MessageDigestSpi;
import java.security.ProviderException;
import java.util.Set;
import java.util.concurrent.ConcurrentSkipListSet;

/**
 * KAE Digest
 */
abstract class KAEDigest extends MessageDigestSpi implements Cloneable {

    private static int DIGESTCONTEXT_CLEAN_THRESHOLD = 100;

    static {
        String threshold = System.getProperty("jdk.digestContext.clean.threshold");
        if (threshold != null && !"".equals(threshold)) {
            DIGESTCONTEXT_CLEAN_THRESHOLD = Integer.parseInt(threshold);
        }
    }

    public static final class MD5 extends KAEDigest {
        public MD5() {
            super("md5", 16);
        }
    }

    private final int digestLength;

    private final String algorithm;

    // field for ensuring native memory is freed
    private DigestContextRef contextRef = null;

    KAEDigest(String algorithm, int digestLength) {
        this.algorithm = algorithm;
        this.digestLength = digestLength;
    }

    private static class DigestContextRef extends PhantomReference<KAEDigest>
            implements Comparable<DigestContextRef> {

        private static ReferenceQueue<KAEDigest> referenceQueue =
                new ReferenceQueue<>();

        private static Set<DigestContextRef> referenceList = new ConcurrentSkipListSet<>();

        private final long ctxAddress;

        DigestContextRef(KAEDigest kaeDigest, long ctxAddress) {
            super(kaeDigest, referenceQueue);
            this.ctxAddress = ctxAddress;
            referenceList.add(this);
            drainRefQueueBounded();
        }

        @Override
        public int compareTo(DigestContextRef other) {
            if (this.ctxAddress == other.ctxAddress) {
                return 0;
            } else {
                return (this.ctxAddress < other.ctxAddress) ? -1 : 1;
            }
        }

        private static void drainRefQueueBounded() {
            int i = 0;
            while (i <= DIGESTCONTEXT_CLEAN_THRESHOLD) {
                DigestContextRef next = (DigestContextRef) referenceQueue.poll();
                if (next == null) {
                    break;
                }
                next.dispose(true);
                i++;
            }
        }

        void dispose(boolean needFree) {
            referenceList.remove(this);
            try {
                if (needFree) {
                    nativeFree(ctxAddress);
                }
            } finally {
                this.clear();
            }
        }
    }

    // single byte update. See JCA doc.
    @Override
    protected synchronized void engineUpdate(byte input) {
        byte[] oneByte = new byte[]{input};
        engineUpdate(oneByte, 0, 1);
    }


    // array update. See JCA doc.
    @Override
    protected synchronized void engineUpdate(byte[] input, int offset, int len) {
        if (len == 0) {
            return;
        }
        if ((offset < 0) || (len < 0) || (offset > input.length - len)) {
            throw new ArrayIndexOutOfBoundsException();
        }
        if (contextRef == null) {
            contextRef = createDigestContext(this);
        }

        try {
            nativeUpdate(contextRef.ctxAddress, input, offset, len);
        } catch (DigestException e) {
            engineReset();
            throw new ProviderException("nativeUpdate failed for " + algorithm + " digests," + e.getMessage());
        }
    }


    // return the digest. See JCA doc.
    @Override
    protected synchronized byte[] engineDigest() {
        final byte[] output = new byte[digestLength];
        try {
            engineDigest(output, 0, digestLength);
        } catch (DigestException e) {
            throw new ProviderException("Internal error", e);
        }
        return output;
    }

    // return the digest in the specified array. See JCA doc.
    @Override
    protected int engineDigest(byte[] output, int offset, int len) throws DigestException {
        if (len < digestLength) {
            throw new DigestException("Length must be at least "
                    + digestLength + " for " + algorithm + " digests");
        }
        if ((offset < 0) || (len < 0) || (offset > output.length - len)) {
            throw new DigestException("Buffer too short to store digest");
        }
        if (contextRef == null) {
            contextRef = createDigestContext(this);
        }
        try {
            nativeDigest(contextRef.ctxAddress, output, offset, digestLength);
        } catch (DigestException e) {
            throw new ProviderException("Invoke nativeDigest failed for " + algorithm + " digests, " + e.getMessage());
        } finally {
            engineReset();
        }
        return digestLength;
    }

    // reset this object. See JCA doc.
    @Override
    protected synchronized void engineReset() {
        if (contextRef != null) {
            contextRef.dispose(true);
            contextRef = null;
        }
    }

    // return digest length. See JCA doc.
    @Override
    protected int engineGetDigestLength() {
        return digestLength;
    }

    @Override
    public synchronized Object clone() throws CloneNotSupportedException {
        KAEDigest kaeDigest = (KAEDigest) super.clone();
        if (kaeDigest.contextRef != null && kaeDigest.contextRef.ctxAddress != 0) {
            long addr;
            try {
                addr = nativeClone(kaeDigest.contextRef.ctxAddress);
            } catch (DigestException e) {
                throw new ProviderException("Invoke nativeClone failed for " + algorithm + " digests, " + e.getMessage());
            }
            kaeDigest.contextRef = new DigestContextRef(kaeDigest, addr);
        }
        return kaeDigest;
    }

    private DigestContextRef createDigestContext(KAEDigest kaeDigest) {
        long addr;
        try {
            addr = nativeInit(algorithm);
        } catch (DigestException e) {
            throw new ProviderException("Invoke nativeInit failed for " + algorithm + " digests, " + e.getMessage());
        }
        return new DigestContextRef(kaeDigest, addr);
    }

    // return pointer to the context
    protected static native long nativeInit(String algorithmName) throws DigestException;

    // update the input byte
    protected static native void nativeUpdate(long ctxAddress, byte[] input, int offset, int inLen) throws DigestException;

    // digest and store the digest message to output
    protected static native int nativeDigest(long ctxAddress, byte[] output, int offset, int len) throws DigestException;

    // digest clone
    protected static native long nativeClone(long ctxAddress) throws DigestException;

    // free the specified context
    protected static native void nativeFree(long ctxAddress);
}
