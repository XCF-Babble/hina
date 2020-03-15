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

#include "hina_impl.h"

using namespace std;

void Hina::hina_encrypt(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const std::string &password)
{
    out = in;
    out_height = in_height;
    out_width = in_width * 3;
}

void Hina::hina_decrypt(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const std::string &password)
{
    out = in;
    out_height = in_height;
    out_width = in_width / 3;
}

void Hina::hina(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const string &password, bool decrypt)
{
    if (decrypt)
        hina_decrypt(out, out_height, out_width, in, in_height, in_width, password);
    else
        hina_encrypt(out, out_height, out_width, in, in_height, in_width, password);
}
