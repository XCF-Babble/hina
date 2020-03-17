# hina

A JPEG-resistant image encryptor/decryptor library. This library implements [arXiv:1811.00236](https://arxiv.org/abs/1811.00236) \[cs.CR\], which proposes an image encryption scheme that can defend the encrypted image against JPEG compression. Some notable differences between this implementation and the paper:

- Instead of converting the image from RGB space to YCbCr space, this implementation splits the image in the RGB space directly.
- The 3 channels of the encrypted image might be stacked vertically or horizontally so that one axis is always twice as big as the other axis. This is designed specifically for the compression scheme used by WeChat Moments.
- Block rotation and block inversion are 2 separate steps.

## Demos

The original image:  
(The following image is excerpted from [映画『天気の子』スペシャル予報](https://youtu.be/DdJXOvtNsCY?t=280).)

[![](img/01.original.jpg)](img/01.original.jpg)

The encrypted image (with password `testpass`):

[![](img/02.encrypted.jpg)](img/02.encrypted.jpg)

The decrypted image from above:

[![](img/03.decrypted.jpg)](img/03.decrypted.jpg)

The decrypted image with a bad password:

[![](img/04.badpassword.jpg)](img/04.badpassword.jpg)

Here is another example that demostrates vertical stacking.

The original image:

[![](img/11.original.jpg)](img/11.original.jpg)

The encrypted image (with password `testpass`):

[![](img/12.encrypted.jpg)](img/12.encrypted.jpg)

The decrypted image from above:

[![](img/13.decrypted.jpg)](img/13.decrypted.jpg)

The decrypted image with a bad password:

[![](img/14.badpassword.jpg)](img/14.badpassword.jpg)

## Usage

1. Clone this repository and its submodule:
    ```
    git clone --recursive https://github.com/XCF-Babble/hina
    cd hina
    ```
2. Make sure you have [CMake 3](https://cmake.org/), Make, Autotools, and a C/C++ compiler installed. To build the library:
    ```
    mkdir build
    cd build
    cmake ..
    make
    ```
    This will build the library itself as well as a copy of [libsodium](https://github.com/jedisct1/libsodium), which will be statically linked by the library.
3. Make sure you have [Python 3](https://www.python.org/) and [Pillow](https://pypi.org/project/Pillow/) installed. To try the helper Python script that calls into the library:
    ```
    cd ../tools
    # On Linux
    LD_LIBRARY_PATH=../build ./hina.py -h
    # or, on macOS
    DYLD_LIBRARY_PATH=../build ./hina.py -h
    ```
