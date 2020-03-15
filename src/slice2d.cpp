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

#include <iostream>
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

Slice2D::Slice2D(vec_byte &vec, size_t width) :
    vec(vec), width(width),
    start_x(0), size_x(vec.size() / width), step_x(1),
    start_y(0), size_y(width), step_y(1)
{
    if (vec.size() % width > 0)
        throw runtime_error("slice not rectangular");
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

vec_byte &Slice2D::data() const
{
    return vec;
}

uint8_t &Slice2D::operator ()(size_t x, size_t y) const
{
    return vec[translate(x, y)];
}

void Slice2D::for_each(for_each_cb cb) const
{
    for (size_t i = 0; i < size_x; ++i)
        for (size_t j = 0; j < size_y; ++j)
            cb(i, j, (*this)(i, j));
}

void Slice2D::print() const
{
    for_each([this](size_t, size_t j, uint8_t &data) {
        cout << uint32_t(data);
        if (j == w() - 1)
            cout << endl;
        else
            cout << ' ';
    });
}

void Slice2D::copy_to(const Slice2D &other) const
{
    for_each([&](size_t i, size_t j, uint8_t &data) {
        other(i, j) = data;
    });
}

void Slice2D::swap(const Slice2D &other) const
{
    vec_byte t(size_x * size_y);
    Slice2D st(t, size_y);
    copy_to(st);
    other.copy_to(*this);
    st.copy_to(other);
}

void Slice2D::rotate(RotationDirection direction) const
{
    if (direction == ROTATE_0)
        return;
    vec_byte t(size_x * size_y);
    Slice2D st(t, size_y);
    copy_to(st);
    if (direction == ROTATE_90) {
        st.for_each([this](size_t i, size_t j, uint8_t &data) {
            (*this)(size_y - j - 1, i) = data;
        });
    } else if (direction == ROTATE_180) {
        st.for_each([this](size_t i, size_t j, uint8_t &data) {
            (*this)(size_x - i - 1, size_y - j - 1) = data;
        });
    } else if (direction == ROTATE_270) {
        st.for_each([this](size_t i, size_t j, uint8_t &data) {
            (*this)(j, size_x - i - 1) = data;
        });
    }
}

void Slice2D::invert(InversionDirection direction) const
{
    if (direction == INVERT_HORIZONTAL) {
        size_t half = size_y / 2;
        for_each([&](size_t i, size_t j, uint8_t &data) {
            if (j < half) {
                uint8_t t = data;
                data = (*this)(i, size_y - j - 1);
                (*this)(i, size_y - j - 1) = t;
            }
        });
    } else if (direction == INVERT_VERTICAL) {
        size_t half = size_x / 2;
        for_each([&](size_t i, size_t j, uint8_t &data) {
            if (i < half) {
                uint8_t t = data;
                data = (*this)(size_x - i - 1, j);
                (*this)(size_x - i - 1, j) = t;
            }
        });
    }
}

void Slice2D::negate() const
{
    for_each([](size_t, size_t, uint8_t &data) {
        data = ~data;
    });
}
