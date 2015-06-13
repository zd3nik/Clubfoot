Clubfoot
========

By Shawn Chidester <zd3nik@gmail.com>

This is a basic chess engine that started as an example for the [Senjo UCI adapter](https://github.com/zd3nik/SenjoUCIAdapter).  It was not my intent to make this a serious engine.  But I'm having a lot of fun tinkering with it trying to make it stronger.

I want Clubfoot to be relatively small and written in a way that is easy to comprehend.  Unfortunately, some areas of code got to be a bit more complicated than I would like, particularly in move generation.  Making those areas of code simpler would have sacrificed too much in terms of capability and/or performance.

### No bitboards

I've decided to see how strong I can make this engine without using bitboards.  Bitboards really are the way to go when writing a chess engine, but Clubfoot will remain sans bitboards.  Quality of static positional evaluation and speed of move generation suffer the most from this limitation.  I will do what I can to improve both of these areas over time.

See [Bitfoot](https://github.com/zd3nik/Bitfoot) for a bitboard version of Clubfoot.

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

Why is so much of the code in one header file?
----------------------------------------------

A very important feature of a chess program is speed.  Breaking things up into separate classes, functions, and especially different object files has negative performance implications.  I'm not an expert on compiler optimizations, but I know from experimentation that optimizers do much better when all your speed critical code is in one object file.

And most of the code is in Clubfoot.h instead of Clubfoot.cpp because of the heavy use of templates.  It's possible to arrange things in such a way that template code can be placed in the cpp file, but I see no reason to jump through such hoops.  I'm just as happy looking at code in a file with a .h extension as I am looking at code in a file with a .cpp extension.

Binary Downloads
----------------

I will periodically make binary builds of this engine available.  Please do not ask for binaries for platforms other than those provided as I do not have the resources to build binaries for other platforms.

**NOTICE**: *On Microsoft Windows platforms if you get errors about missing dlls when running the Clubfoot executable you must download and install the [Microsoft Visual C++ Redistributable Package](http://www.microsoft.com/en-us/download/details.aspx?id=40784).*

* On the Microsoft Visual C++ Redistributable Package page (link provided above) click the `Download` button and choose `vcredist_x86.exe` for 32-bit, choose `vcredist_x64.exe` for 64-bit.  Install the package and try Clubfoot again.

**NOTICE**: *Binaries are provided under the terms of the license described below.  Use them at your own risk.  I welcome feedback on the performance of Clubfoot but I provide no warranty, no promises that it will work on your computer, and no form of technical support.*

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
