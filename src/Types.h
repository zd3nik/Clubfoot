//----------------------------------------------------------------------------
// Copyright (c) 2015 Shawn Chidester <zd3nik@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//----------------------------------------------------------------------------

#ifndef CLUBFOOT_TYPES_H
#define CLUBFOOT_TYPES_H

namespace clubfoot
{

#define COLOR_OF(p) ((p) & ColorMask)

typedef bool Color;

enum ColorName {
  White = 0,
  Black = 1
};

enum PieceType {
  NoPiece = 0,
  Pawn    = 2,
  Knight  = 4,
  Bishop  = 6,
  Rook    = 8,
  Queen   = 10,
  King    = 12
};

enum PieceValue {
  PawnValue   = 100,
  KnightValue = 350,
  BishopValue = 350,
  RookValue   = 500,
  QueenValue  = 950
};

enum CastleType {
  WhiteShort = 0x02,
  WhiteLong  = 0x04,
  BlackShort = 0x08,
  BlackLong  = 0x10
};

enum BitMask {
  OneBit          = 0x1,
  TwoBits         = 0x3,
  ThreeBits       = 0x7,
  FourBits        = 0xF,
  FiveBits        = 0x1F,
  SixBits         = 0x3F,
  SevenBits       = 0x7F,
  EightBits       = 0xFF
};

enum Misc {
  ColorMask       = 0x1,
  WhiteCastleMask = (WhiteShort|WhiteLong),
  BlackCastleMask = (BlackShort|BlackLong),
  CastleMask      = (WhiteCastleMask|BlackCastleMask),
  Draw            = 0x20,
  MaxPlies        = 100,
  MaxMoves        = 128,
  StartMaterial   = ((8 * PawnValue) + (2 * KnightValue) +
                     (2 * BishopValue) + (2 * RookValue) +  QueenValue),
  WinningScore    = 30000,
  MateScore       = 31000,
  Infinity        = 32000
};

enum CheckState {
  Unknown,
  InCheck,
  NotInCheck
};

enum MoveGenType {
  AllMoves,
  CapsAndChecks,
  Captures
};

} // namespace clubfoot

#endif // CLUBFOOT_TYPES_H
