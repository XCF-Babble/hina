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

#include "crypto.h"

#include <stdexcept>

#include <sodium.h>

using namespace std;

void Crypto::init()
{
    if (sodium_init() == -1)
        throw runtime_error("could not initialize libsodium");
}

string Crypto::derive_key(const string &password, const string &salt)
{
    string ret(crypto_stream_chacha20_ietf_KEYBYTES, 0);
    if (crypto_pwhash(reinterpret_cast<unsigned char *>(&ret[0]), ret.size(),
        password.c_str(), password.size(), reinterpret_cast<const unsigned char *>(salt.c_str()),
        crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0)
        throw runtime_error("unable to derive key");
    return ret;
}

string Crypto::zero_nonce()
{
    return string(crypto_stream_chacha20_ietf_NONCEBYTES, 0);
}

void Crypto::plus_one(string &s)
{
    sodium_increment(reinterpret_cast<unsigned char *>(&s[0]), s.size());
}

string Crypto::prng(const string &key, const string &nonce, size_t len)
{
    string ret(len, 0);
    crypto_stream_chacha20_ietf(reinterpret_cast<unsigned char *>(&ret[0]), ret.size(),
        reinterpret_cast<const unsigned char *>(nonce.c_str()), reinterpret_cast<const unsigned char *>(key.c_str()));
    return ret;
}
