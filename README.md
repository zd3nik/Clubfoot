Clubfoot
========

By Shawn Chidester <zd3nik@gmail.com>

This is a very basic chess engine that started as an example for the [Senjo UCI adapter](https://github.com/zd3nik/SenjoUCIAdapter).  It has never been my purpose to make this a serious engine.  I wanted it to be as small as possible, written in a way that is easy to comprehend, yet still capable of playing a reasonable game of chess.  I was also thinking the simplistic code base would be an ideal way to provide real-world examples of some common chess engine programming concepts such as 0x88 board representation, transposition table, killer move heuristic, null move pruning, etc.  I suppose it might serve that purpose to some degree, but even the simplest and most common chess engine concepts can be implemented in vastly differing ways.  So, in retrospect, I've stricken that purpose from the list of Clubfoot's objectives.

There are many aspects of this engine that could be vastly improved with a little effort, particularly in the area of static positional analysis and threat detection.  To challenge myself I've decided to see how far I could take it sticking to pure 0x88 board techniques - e.g. no bitboards.  This is my first 0x88 chess engine so there are likely several tricks that I'm unaware of.  This is a learning experience for me and I welcome any enhancements that do not morph Clubfoot into a bitboard engine.

I'm also thinking that as long as Clubfoot sticks to the minimal basics it may come in handy as a sort of entry level benchmark for testing other engines (that's what I will be using it for anyway).  It may also serve very well as a sparing partner for budding human chess players.

All preamble aside, this is a free and open source project.  Use it in any way you wish.  Just please take the objectives I've stated above into consideration if you want to submit changes back on the master branch.

### Use recursive clone

Since Clubfoot uses the [Senjo UCI adapter](https://github.com/zd3nik/SenjoUCIAdapter) as a sub-module be sure to use the --recursive option when cloning.  See [Git - Submodules](http://git-scm.com/book/en/v2/Git-Tools-Submodules) for more information.

`git clone --recursive https://github.com/zd3nik/Clubfoot.git`

Features
--------

* 0x88 board representation (uses ~0x77 instead of 0x88 for legality testing).
* Aspiration windows.
* Internal iterative deepening.
* Iterative deepening.
* Killer move heuristic.
* Late move reductions.
* Null move pruning.
* Piece/Square tables.
* Principal variation search.
* Quiescence search.
* Static exchange evaluation.
* Transposition table.

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
