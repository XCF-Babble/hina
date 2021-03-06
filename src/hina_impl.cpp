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

#include "prng.h"

using namespace std;

const string Hina::salt_scramble("scramblescramble");
const string Hina::salt_rotate("rotaterotaterota");
const string Hina::salt_invert("invertinvertinve");
const string Hina::salt_negate("negatenegatenega");

Slice2D Hina::get_block(size_t index, size_t block_size, const Slice2D &slice)
{
    size_t width = slice.w();
    size_t num_blocks_row = width / block_size;
    size_t x = index / num_blocks_row * block_size;
    size_t y = index % num_blocks_row * block_size;
    return Slice2D(slice.data(), width, x, x + block_size, 1, y, y + block_size, 1);
}

void Hina::hina_encrypt(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const string &password, size_t block_size)
{
    out_height = in_height + ((block_size - in_height % block_size) % block_size);
    out_width = in_width + ((block_size - in_width % block_size) % block_size);
    bool vstack = out_height * 3 >= out_width * 2;
    out.resize(out_height * out_width * 3);
    Slice2D slice_r(const_cast<vec_byte &>(in), in_width * 3, 0, in_height, 1, 0, in_width * 3, 3);
    Slice2D slice_g(const_cast<vec_byte &>(in), in_width * 3, 0, in_height, 1, 1, in_width * 3, 3);
    Slice2D slice_b(const_cast<vec_byte &>(in), in_width * 3, 0, in_height, 1, 2, in_width * 3, 3);
    if (vstack) {
        Slice2D slice_out_1(out, out_width, 0, out_height * 3, 1, 0, out_width, 1);
        Slice2D slice_out_2(out, out_width, out_height, out_height * 3, 1, 0, out_width, 1);
        Slice2D slice_out_3(out, out_width, out_height * 2, out_height * 3, 1, 0, out_width, 1);
        slice_r.copy_to(slice_out_1);
        slice_g.copy_to(slice_out_2);
        slice_b.copy_to(slice_out_3);
        out_height *= 3;
    } else {
        Slice2D slice_out_1(out, out_width * 3, 0, out_height, 1, 0, out_width * 3, 1);
        Slice2D slice_out_2(out, out_width * 3, 0, out_height, 1, out_width, out_width * 3, 1);
        Slice2D slice_out_3(out, out_width * 3, 0, out_height, 1, out_width * 2, out_width * 3, 1);
        slice_r.copy_to(slice_out_1);
        slice_g.copy_to(slice_out_2);
        slice_b.copy_to(slice_out_3);
        out_width *= 3;
    }
    Slice2D slice(out, out_width);
    size_t num_blocks = (out_height / block_size) * (out_width / block_size);

    PRNG prng_scramble(password, salt_scramble, static_cast<uint32_t>(num_blocks));
    for (size_t i = 0; i < num_blocks; ++i)
        get_block(i, block_size, slice).swap(get_block(prng_scramble.get_random(), block_size, slice));
    
    PRNG prng_rotate(password, salt_rotate, 4);
    for (size_t i = 0; i < num_blocks; ++i)
        get_block(i, block_size, slice).rotate(static_cast<Slice2D::RotationDirection>(prng_rotate.get_random()));

    PRNG prng_invert(password, salt_invert, 3);
    for (size_t i = 0; i < num_blocks; ++i)
        get_block(i, block_size, slice).invert(static_cast<Slice2D::InversionDirection>(prng_invert.get_random()));
    
    PRNG prng_negate(password, salt_negate, 2);
    for (size_t i = 0; i < num_blocks; ++i)
        if (prng_negate.get_random())
            get_block(i, block_size, slice).negate();
}

void Hina::hina_decrypt(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const string &password, size_t block_size)
{
    vec_byte tmp(in);
    Slice2D slice(tmp, in_width);
    size_t num_blocks = (in_height / block_size) * (in_width / block_size);

    PRNG prng_negate(password, salt_negate, 2);
    for (size_t i = 0; i < num_blocks; ++i)
        if (prng_negate.get_random())
            get_block(i, block_size, slice).negate();

    PRNG prng_invert(password, salt_invert, 3);
    for (size_t i = 0; i < num_blocks; ++i)
        get_block(i, block_size, slice).invert(static_cast<Slice2D::InversionDirection>(prng_invert.get_random()));

    PRNG prng_rotate(password, salt_rotate, 4);
    for (size_t i = 0; i < num_blocks; ++i)
        switch (static_cast<Slice2D::RotationDirection>(prng_rotate.get_random())) {
        case Slice2D::ROTATE_0:
            break;
        case Slice2D::ROTATE_90:
            get_block(i, block_size, slice).rotate(Slice2D::ROTATE_270);
            break;
        case Slice2D::ROTATE_180:
            get_block(i, block_size, slice).rotate(Slice2D::ROTATE_180);
            break;
        case Slice2D::ROTATE_270:
            get_block(i, block_size, slice).rotate(Slice2D::ROTATE_90);
            break;
        }

    PRNG prng_scramble(password, salt_scramble, static_cast<uint32_t>(num_blocks));
    vector<uint32_t> scramble_rands(num_blocks, 0);
    for (size_t i = 0; i < num_blocks; ++i)
        scramble_rands[i] = prng_scramble.get_random();
    for (size_t i = num_blocks - 1; i != static_cast<size_t>(-1); --i)
        get_block(i, block_size, slice).swap(get_block(scramble_rands[i], block_size, slice));

    bool vstack = in_height > in_width;
    out.resize(tmp.size());
    if (vstack) {
        out_height = in_height / 3;
        out_width = in_width;
        Slice2D slice_r(out, out_width * 3, 0, out_height, 1, 0, out_width * 3, 3);
        Slice2D slice_g(out, out_width * 3, 0, out_height, 1, 1, out_width * 3, 3);
        Slice2D slice_b(out, out_width * 3, 0, out_height, 1, 2, out_width * 3, 3);
        Slice2D slice_tmp_1(tmp, in_width, 0, out_height, 1, 0, in_width, 1);
        Slice2D slice_tmp_2(tmp, in_width, out_height, out_height * 2, 1, 0, in_width, 1);
        Slice2D slice_tmp_3(tmp, in_width, out_height * 2, in_height, 1, 0, in_width, 1);
        slice_tmp_1.copy_to(slice_r);
        slice_tmp_2.copy_to(slice_g);
        slice_tmp_3.copy_to(slice_b);
    } else {
        out_height = in_height;
        out_width = in_width / 3;
        Slice2D slice_r(out, out_width * 3, 0, out_height, 1, 0, out_width * 3, 3);
        Slice2D slice_g(out, out_width * 3, 0, out_height, 1, 1, out_width * 3, 3);
        Slice2D slice_b(out, out_width * 3, 0, out_height, 1, 2, out_width * 3, 3);
        Slice2D slice_tmp_1(tmp, in_width, 0, out_height, 1, 0, out_width, 1);
        Slice2D slice_tmp_2(tmp, in_width, 0, out_height, 1, out_width, out_width * 2, 1);
        Slice2D slice_tmp_3(tmp, in_width, 0, out_height, 1, out_width * 2, in_width, 1);
        slice_tmp_1.copy_to(slice_r);
        slice_tmp_2.copy_to(slice_g);
        slice_tmp_3.copy_to(slice_b);
    }
}

void Hina::hina(vec_byte &out, size_t &out_height, size_t &out_width,
    const vec_byte &in, size_t in_height, size_t in_width,
    const string &password, size_t block_size, bool decrypt)
{
    if (decrypt)
        hina_decrypt(out, out_height, out_width, in, in_height, in_width, password, block_size);
    else
        hina_encrypt(out, out_height, out_width, in, in_height, in_width, password, block_size);
}
