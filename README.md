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

I will periodically make binary builds of this engine available.  Please do not ask for binaries for platforms other than those provided as I do not have the resources to build binaries for other platforms.

**NOTICE**: *On Microsoft Windows platforms if you get errors about missing dlls when running the Clubfoot executable you must download and install the [Microsoft Visual C++ Redistributable Package](http://www.microsoft.com/en-us/download/details.aspx?id=40784).*

* On the Microsoft Visual C++ Redistributable Package page (link provided above) click the `Download` button and choose `vcredist_x86.exe` for 32-bit, choose `vcredist_x64.exe` for 64-bit.  Install the package and try Clubfoot again.

**NOTICE**: *These binaries are provided under the terms of the license described below.  Use them at your own risk.  I welcome feedback on the performance of Clubfoot but I provide no warranty, no promises that it will work on your computer, and no form of technical support.*

* [Clubfoot-1.0.0fa7e53-windows-msvc2012.zip](https://drive.google.com/open?id=0B3Bl0MPLTCLjc0oyTE95MlBhSlU&authuser=0)
* md5sums:
 
  * `d9260e00ae2375f6095d61e9455be0e9` Clubfoot-1.0.0fa7e53-windows-msvc2012.zip
  * `59d60ceaf467a06ff6be52adaf3f110a` Clubfoot-1.0.0fa7e53-win32.exe
  * `99e84d8a44cf043f9145ee9676559075` Clubfoot-1.0.0fa7e53-win64.exe
  * `b612c7e5b65b78c5cebf4c1acc419bf5` Clubfoot-Logo-Small.png


* [Clubfoot-1.0.0fa7e53-linux-libc-2.20.zip](https://drive.google.com/open?id=0B3Bl0MPLTCLjRC1tY2JxbnhzVms&authuser=0)
* md5sums:
 
  * `416902a729da33b774e3fca1198c9038` Clubfoot-1.0.0fa7e53-linux-libc-2.20.zip
  * `5a13e7fc02d2297953635889fb49e5ed` Clubfoot-1.0.0fa7e53-linux32
  * `57b629c8b6b7a10230bfe2fa4aa1ef89` Clubfoot-1.0.0fa7e53-linux64
  * `b612c7e5b65b78c5cebf4c1acc419bf5` Clubfoot-Logo-Small.png

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
