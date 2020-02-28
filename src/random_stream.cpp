/*
 * Copyright (c) 2020, The Hina Authors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "random_stream.h"

#include <stdexcept>

using namespace std;

uint32_t StreamingDPRNG::maybe_reseed_and_get_random() {
    uint32_t r;

    if (pos == 0)
        /* Reseed */
        randombytes_buf_deterministic(buf.data(), buf.max_size(), key.data());

    r = buf[pos];
    pos = (pos + 1) % NUM_RAND_BYTES;
    return r;
}

StreamingDPRNG::StreamingDPRNG(uint8_t key[KEY_LEN], uint32_t buckets) : pos(0), buckets(buckets) {
    if (sodium_init() < 0)
        throw runtime_error("Could not initialize libsodium");

    if (buckets < 2)
        throw invalid_argument("Must have at least two buckets");
    key = move(key);
}

uint32_t StreamingDPRNG::get_random() {
    uint32_t min, r;

    min = (1U + ~buckets) % buckets; /* = 2**32 mod upper_bound */
    do {
        r = maybe_reseed_and_get_random();
    } while (r < min);
    /* r is now clamped to a set whose size mod upper_bound == 0
     * the worst case (2**31+1) requires ~ 2 attempts */

    return r % buckets;
}