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

#include "slice2d.h"

#include <stdexcept>

using namespace std;

size_t Slice2D::div_ceil(size_t x, size_t y)
{
    return x / y + (x % y > 0);
}

size_t Slice2D::translate(size_t x, size_t y) const
{
    if (x >= size_x || y >= size_y)
        throw runtime_error("access out of bounds: trying to access (" + to_string(x) + ", " + to_string(y) +
            "), but the shape is (" + to_string(size_x) + ", " + to_string(size_y) + ')');
    x = start_x + step_x * x;
    y = start_y + step_y * y;
    return x * width + y;
}


Slice2D::Slice2D(vec_byte &vec, size_t width,
    size_t start_x, size_t end_x, size_t step_x,
    size_t start_y, size_t end_y, size_t step_y) :
    vec(vec), width(width),
    start_x(start_x), size_x(div_ceil(end_x - start_x, step_x)), step_x(step_x),
    start_y(start_y), size_y(div_ceil(end_y - start_y, step_y)), step_y(step_y)
{
    if (translate(size_x - 1, size_y - 1) >= vec.size())
        throw runtime_error("slice out of bounds");
}

size_t Slice2D::h() const
{
    return size_x;
}

size_t Slice2D::w() const
{
    return size_y;
}

Slice2D::byte &Slice2D::operator ()(size_t x, size_t y) const
{
    return vec[translate(x, y)];
}

void Slice2D::for_each(for_each_cb cb) const
{
    for (size_t i = 0; i < size_x; ++i)
        for (size_t j = 0; j < size_y; ++j)
            cb(i, j, (*this)(i, j));
}

void Slice2D::copy_to(const Slice2D &other) const
{
    for_each([&](size_t i, size_t j, byte &data) {
        other(i, j) = data;
    });
}
