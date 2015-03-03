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

#ifndef CLUBFOOT_MOVE_H
#define CLUBFOOT_MOVE_H

#include "senjo/src/Square.h"
#include "Types.h"

namespace clubfoot
{

//----------------------------------------------------------------------------
//! \brief A packed chess move representation
//! Move info packed into 32 bits for fast store/load in transposition table
//----------------------------------------------------------------------------
class Move {
public:
  //--------------------------------------------------------------------------
  enum MoveType {
    Invalid,
    Normal,
    PawnPush,
    PawnLung,
    PawnCapture,
    EnPassant,
    KingMove,
    CastleShort,
    CastleLong
  };

  //--------------------------------------------------------------------------
  enum Shifts {
    FromShift   = 4,  ///< Number of bits the 'from' square is shifted left
    ToShift     = 12, ///< Number of bits the 'to' square is shifted left
    PcShift     = 20, ///< Number of bits the 'piece' value is shifted left
    CapShift    = 24, ///< Number of bits the 'cap' value is shifted left
    PromoShift  = 28  ///< Number of bits the 'promo' value is shifted left
  };

  //--------------------------------------------------------------------------
  //! \brief Compare two moves lexically
  //! \param a The left move
  //! \param b The righ move
  //! \return true if move \p a has a lower lexical value than move \p b
  //--------------------------------------------------------------------------
  static bool LexicalCompare(const Move& a, const Move& b) {
    return (strcmp(a.ToString().c_str(), b.ToString().c_str()) < 0);
  }

  //--------------------------------------------------------------------------
  //! \brief Compare the score of two moves
  //! \param a The left move
  //! \param b The right move
  //! \return true if the score of move \p a is less than the score of move \p b
  //--------------------------------------------------------------------------
  static bool ScoreCompare(const Move& a, const Move& b) {
    return (a.score < b.score);
  }

  //--------------------------------------------------------------------------
  //! \brief Default constructor
  //! \param bits The 32 bit packed form of the move
  //! \param score The move score
  //--------------------------------------------------------------------------
  Move(const uint32_t bits = 0, const int score = 0)
    : bits(bits),
      score(score)
  { }

  //--------------------------------------------------------------------------
  //! \brief Copy constructor
  //! \param other The move to copy
  //--------------------------------------------------------------------------
  Move(const Move& other)
    : bits(other.bits),
      score(other.score)
  {
    assert(abs(score) <= Infinity);
  }

  //--------------------------------------------------------------------------
  //! \brief Initialize this move
  //! \param bits The 32 bit packed form of the move
  //! \param score The move score
  //--------------------------------------------------------------------------
  void Init(const uint32_t bits, const int score) {
    assert(abs(score) <= Infinity);
    this->bits = bits;
    this->score = static_cast<int32_t>(score);
  }

  //--------------------------------------------------------------------------
  //! \brief Initialize this move
  //! \param type The move type
  //! \param from The source square
  //! \param to The destination square
  //! \param pc The type of piece being moved
  //! \param cap The type of piece captured
  //! \param promo The type of piece promoted to
  //! \param score The move score
  //--------------------------------------------------------------------------
  void Init(const MoveType type,
            const int from,
            const int to,
            const int pc,
            const int cap = 0,
            const int promo = 0,
            const int score = 0)
  {
    assert(abs(score) <= Infinity);
    bits = (static_cast<uint32_t>(type)                             |
            static_cast<uint32_t>((from  & EightBits) << FromShift) |
            static_cast<uint32_t>((to    & EightBits) << ToShift)   |
            static_cast<uint32_t>((pc    & FourBits)  << PcShift)   |
            static_cast<uint32_t>((cap   & FourBits)  << CapShift)  |
            static_cast<uint32_t>((promo & FourBits)  << PromoShift));
    this->score = static_cast<int32_t>(score);
  }

  //--------------------------------------------------------------------------
  //! \brief Assignment operator
  //! \param other The move to copy
  //--------------------------------------------------------------------------
  Move& operator=(const Move& other) {
    bits = other.bits;
    score = other.score;
    return *this;
  }

  //--------------------------------------------------------------------------
  //! \brief Get this move's score
  //! \return This move's score
  //--------------------------------------------------------------------------
  int GetScore() const {
    return score;
  }

  //--------------------------------------------------------------------------
  //! \brief Get a reference to this move's score value
  //! \return A reference to this move's score value
  //--------------------------------------------------------------------------
  int& Score() {
    return score;
  }

  //--------------------------------------------------------------------------
  //! \brief Get the 32 bit packed form of this move
  //! \return The 32 bit packed form of this move
  //--------------------------------------------------------------------------
  uint32_t GetBits() const {
    return bits;
  }

  //--------------------------------------------------------------------------
  //! \brief Get the type of this move
  //! \return The move type
  //--------------------------------------------------------------------------
  MoveType GetType() const {
    return static_cast<MoveType>(bits & FourBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the type of piece being moved
  //! \return The type of piece being moved
  //--------------------------------------------------------------------------
  int GetPc() const {
    return static_cast<int>((bits >> PcShift) & FourBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the the type of piece captured
  //! \return The type of piece captured
  //--------------------------------------------------------------------------
  int GetCap() const {
    return static_cast<int>((bits >> CapShift) & FourBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the type of piece promoted to
  //! \return The type of piece promoted to
  //--------------------------------------------------------------------------
  int GetPromo() const {
    return static_cast<int>((bits >> PromoShift) & FourBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the source square of this move
  //! \return The source square
  //--------------------------------------------------------------------------
  senjo::Square GetFrom() const {
    return GetFromName();
  }

  //--------------------------------------------------------------------------
  //! \brief Get the name of the source square of this move
  //! \return The source square name
  //--------------------------------------------------------------------------
  int GetFromName() const {
    return static_cast<int>((bits >> FromShift) & EightBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the destination square of this move
  //! \return The destination square
  //--------------------------------------------------------------------------
  senjo::Square GetTo() const {
    return GetToName();
  }

  //--------------------------------------------------------------------------
  //! \brief Get the name of the destination square of this move
  //! \return The destination square name
  //--------------------------------------------------------------------------
  int GetToName() const {
    return static_cast<int>((bits >> ToShift) & EightBits);
  }

  //--------------------------------------------------------------------------
  //! \brief Is this move initialized?
  //! \return true if this move has type, from, and to values
  //--------------------------------------------------------------------------
  bool IsValid() const {
    return (GetType() && (GetFromName() != GetToName()));
  }

  //--------------------------------------------------------------------------
  //! \brief Does this move capture a piece or promote a pawn?
  //! \return true if this move captures a piece or promotes a pawn
  //--------------------------------------------------------------------------
  bool IsCapOrPromo() const {
    return (bits & 0xFF000000UL);
  }

  //--------------------------------------------------------------------------
  //! \brief Get the history array index for this move
  //! \return The history array index for this move
  //--------------------------------------------------------------------------
  int GetHistoryIndex() const {
    return ((bits >> FromShift) & 0xFFFFF);
  }

  //--------------------------------------------------------------------------
  //! \brief Equality comparision operator
  //! \return true if this move is the same as \p other
  //--------------------------------------------------------------------------
  bool operator==(const Move& other) const {
    return (bits && (bits == other.bits));
  }

  //--------------------------------------------------------------------------
  //! \brief Inequality comparison operator
  //! \return true if this move is not the same as \p other
  //--------------------------------------------------------------------------
  bool operator!=(const Move& other) const {
    return (!bits || (bits != other.bits));
  }

  //--------------------------------------------------------------------------
  //! \brief Less-than comparison operator
  //! \param other The move to compare this one to
  //! \return true if this move has a score less than \p other
  //--------------------------------------------------------------------------
  bool operator<(const Move& other) const {
    return (score < other.score);
  }

  //--------------------------------------------------------------------------
  //! \brief Clear the contents of this move
  //--------------------------------------------------------------------------
  void Clear() {
    bits = 0;
    score = 0;
  }

  //--------------------------------------------------------------------------
  //! \brief Swap the contents of this move with another move
  //! \param other The move to swap contents with
  //--------------------------------------------------------------------------
  void SwapWith(Move& other) {
    const uint32_t tbits  = bits;
    const int32_t  tscore = score;
    bits  = other.bits;
    score = other.score;
    other.bits =  tbits;
    other.score = tscore;
  }

  //--------------------------------------------------------------------------
  //! \brief Get coordinate notation string representation of this move
  //! \return an empty string if this move is not valid
  //--------------------------------------------------------------------------
  std::string ToString() const {
    std::string str;
    if (IsValid()) {
      str = (GetFrom().ToString() + GetTo().ToString());
      switch (GetPromo()) {
      case (White|Knight): case (Black|Knight): return (str + "n");
      case (White|Bishop): case (Black|Bishop): return (str + "b");
      case (White|Rook):   case (Black|Rook):   return (str + "r");
      case (White|Queen):  case (Black|Queen):  return (str + "q");
      default:
        break;
      }
    }
    return str;
  }

private:
  uint32_t bits;
  int32_t score;
};

} // namespace clubfoot

#endif // CLUBFOOT_MOVE_H
