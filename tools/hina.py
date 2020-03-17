#!/usr/bin/env python3

# Copyright (c) 2020, The Hina Authors
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from ctypes import *
import platform

from PIL import Image

system = platform.uname()[0]
if system == 'Windows':
    lib_name = 'hina.dll'
elif system == 'Linux':
    lib_name = 'libhina.so'
else:
    lib_name = 'libhina.dylib'

libhina = cdll.LoadLibrary(lib_name)
c_hina = libhina.hina
c_hina.argtypes = [POINTER(c_size_t), POINTER(c_size_t), POINTER(c_uint8), c_size_t, c_size_t, c_char_p, c_int]
c_hina.restype = POINTER(c_uint8)
c_hina_free = libhina.hina_free
c_hina_free.argtypes = [POINTER(c_uint8)]

def hina(im, password, decrypt):
    im = im.convert('L' if decrypt else 'RGB')
    decrypt = int(decrypt)
    c_out_height, c_out_width = c_size_t(), c_size_t()
    im_data = tuple(im.getdata())
    if not decrypt:
        im_data = tuple(x for sets in im_data for x in sets)
    c_in = (c_uint8 * len(im_data))(*im_data)
    c_out = c_hina(byref(c_out_height), byref(c_out_width), c_in, im.size[1], im.size[0], password.encode(), decrypt)
    out_height, out_width = c_out_height.value, c_out_width.value
    out_size = out_height * out_width * (3 if decrypt else 1)
    out = bytes(c_out[i] for i in range(out_size))
    c_hina_free(c_out)
    return Image.frombytes('RGB' if decrypt else 'L', (out_width, out_height), out)  

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='A hina frontend in Python.')
    parser.add_argument('-d', '--decrypt', action='store_true', help='decrypt the image')
    parser.add_argument('-p', '--password', default='', help='the password to encrypt/decrypt the image')
    parser.add_argument('infile', help='the image to encrypt/decrypt')
    parser.add_argument('outfile', help='the filename to output the resulting image')
    args = parser.parse_args()
    hina(Image.open(args.infile), args.password, args.decrypt).save(args.outfile)
