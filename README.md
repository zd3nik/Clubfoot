Clubfoot
========

By Shawn Chidester <zd3nik@gmail.com>

This is a basic chess engine that started as an example for the [Senjo UCI adapter](https://github.com/zd3nik/SenjoUCIAdapter).  It was not my purpose to make this a serious engine.  But I'm having a lot of fun tinkering with it trying to make it stronger.

I want Clubfoot to be relatively small and written in a way that is easy to comprehend.  Unfortunately, some areas of code got to be a bit more complicated than I would like, particularly in move generation.  Making those areas of code simpler would have sacrificed too much in terms of capability and/or performance.

### No bitboards

I've decided to see how strong this engine can be made without using bitboards.  Bitboards really are the way to go when writing a chess engine, but Clubfoot will remain sans bitboards.  Quality of static positional evaluation and speed of move generation suffer the most from this limitation.  I will do what I can to improve both of these areas over time.

### Use recursive clone

Clubfoot uses the [Senjo UCI adapter](https://github.com/zd3nik/SenjoUCIAdapter) as a sub-module so you must use the --recursive option when cloning.  See [Git - Submodules](http://git-scm.com/book/en/v2/Git-Tools-Submodules) for more information.

`git clone --recursive https://github.com/zd3nik/Clubfoot.git`

Features
--------

* 0x88 board representation (uses ~0x77 instead of 0x88 for legality testing).
* Aspiration windows.
* Delta pruning.
* Depth adaptive null move pruning.
* History Heuristic.
* Internal iterative deepening.
* Iterative deepening.
* Killer move heuristic.
* Late move reductions.
* Perft search.
* Piece/Square tables.
* Principal variation search.
* Quiescence search.
* Razoring.
* Static exchange evaluation.
* Transposition table.

Binary Downloads
----------------

I will periodically make binary builds of this engine available.  Please do not ask for binaries for platforms other than those provided as I do not have the resources to provide binaries for other platforms.

**NOTICE**: *These binaries are provided under the terms of the license described below.  Use them at your own risk.  I welcome feedback on the performance of Clubfoot but I provide no warranty, no promises that it will work on your computer, and no form of technical support.*

* [Clubfoot-1.0.31eadf2-windows-msvc2012.zip](https://drive.google.com/open?id=0B3Bl0MPLTCLjd3o0Rkx2dXRsM0k&authuser=0)
* md5sums:
 
  * `7f5ab0ee570c532b2abdc3e0e54bf944` Clubfoot-1.0.31eadf2-windows-msvc2012.zip
  * `be1d1334d3c24c14054c3027ab3cfc9b` Clubfoot-1.0.31eadf2-win32.exe
  * `823b30c84fc39be6343f71bc42075455` Clubfoot-1.0.31eadf2-win64.exe
  * `875326af033c5ba0d1fb42ad5946b08d` Clubfoot-Logo.png


* [Clubfoot-1.0.0d03228-linux-libc-2.20.zip](https://drive.google.com/open?id=0B3Bl0MPLTCLjNEZMU3VnanJQTU0&authuser=0)
* md5sums:
 
  * `f52641c7e77d34d8a2a7e746e95ed545` Clubfoot-1.0.0d03228-linux-libc-2.20.zip
  * `66a23300105fadd42d17c9c0aba1f304` Clubfoot-1.0.0d03228-linux32
  * `01a2f0885b8eeb0cbd506084e87fb67f` Clubfoot-1.0.0d03228-linux64
  * `875326af033c5ba0d1fb42ad5946b08d` Clubfoot-Logo.png

License
-------

Copyright (c) 2015 Shawn Chidester <zd3nik@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
