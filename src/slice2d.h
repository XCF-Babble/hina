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

#pragma once

#include <cstdint>
#include <functional>
#include <vector>

class Slice2D {
public:
    typedef uint8_t byte;
    typedef std::vector<byte> vec_byte;
    typedef std::function<void(size_t, size_t, byte&)> for_each_cb;
private:
    vec_byte &vec;
    size_t width;
    size_t start_x, size_x, step_x;
    size_t start_y, size_y, step_y;
    static size_t div_ceil(size_t x, size_t y);
    size_t translate(size_t x, size_t y) const;
public:
    enum RotationDirection {
        ROTATE_0,
        ROTATE_90,
        ROTATE_180,
        ROTATE_270
    };
    enum InversionDirection {
        INVERT_NONE,
        INVERT_HORIZONTAL,
        INVERT_VERTICAL
    };
    Slice2D(vec_byte &vec, size_t width);
    Slice2D(vec_byte &vec, size_t width,
        size_t start_x, size_t end_x, size_t step_x,
        size_t start_y, size_t end_y, size_t step_y);
    size_t h() const;
    size_t w() const;
    byte &operator ()(size_t x, size_t y) const;
    void for_each(for_each_cb cb) const;
    void print() const;
    void copy_to(const Slice2D &other) const;
    void swap(const Slice2D &other) const;
    void rotate(RotationDirection direction) const;
    void invert(InversionDirection direction) const;
    void negate() const;
};
