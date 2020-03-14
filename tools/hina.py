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

import argparse
from ctypes import *

from PIL import Image

def hina(im, password, decrypt):
    libhina = cdll.LoadLibrary('libhina.so')
    c_hina = libhina.hina
    c_hina.argtypes = [POINTER(c_size_t), POINTER(c_size_t), POINTER(c_uint8), c_size_t, c_size_t, c_char_p, c_int]
    c_hina.restype = POINTER(c_uint8)
    c_hina_free = libhina.hina_free
    c_hina_free.argtypes = [POINTER(c_uint8)]
    im = im.convert('L') if decrypt else im.convert('RGB')
    decrypt = int(decrypt)
    return im

def main():
    parser = argparse.ArgumentParser(description='A hina frontend in Python.')
    parser.add_argument('-d', '--decrypt', action='store_true', help='decrypt the image')
    parser.add_argument('-p', '--password', default='', help='the password to encrypt/decrypt the image')
    parser.add_argument('infile', help='the image to encrypt/decrypt')
    parser.add_argument('outfile', help='the filename to output the resulting image')
    args = parser.parse_args()
    hina(Image.open(args.infile), args.password, args.decrypt).save(args.outfile)

if __name__ == '__main__':
    main()
