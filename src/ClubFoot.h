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

#ifndef CLUBFOOT_H
#define CLUBFOOT_H

#include "senjo/src/ChessEngine.h"
#include "senjo/src/Square.h"
#include "senjo/src/Output.h"
#include "Types.h"
#include "Move.h"
#include "HashTable.h"
#include "Stats.h"

namespace clubfoot
{

//----------------------------------------------------------------------------
//! \brief Simplistic chess engine
//----------------------------------------------------------------------------
class ClubFoot : public senjo::ChessEngine
{
public:
  //--------------------------------------------------------------------------
  // senjo::ChessEngine methods (implemented in Clubfoot.cpp)
  //--------------------------------------------------------------------------
  std::string GetEngineName() const;
  std::string GetEngineVersion() const;
  std::string GetAuthorName() const;
  std::string GetCountryName() const;
  std::string GetFEN() const;
  std::list<senjo::EngineOption> GetOptions() const;
  bool SetEngineOption(const std::string& name, const std::string& value);
  bool IsInitialized() const;
  bool WhiteToMove() const;
  const char* SetPosition(const char* fen);
  const char* MakeMove(const char* str);
  void PrintBoard() const;
  void Initialize();
  void ClearSearchData();
  void PonderHit();
  void Quit();
  void ResetStatsTotals();
  void ShowStatsTotals() const;
  void GetStats(int* depth,
                int* seldepth = NULL,
                uint64_t* nodes = NULL,
                uint64_t* qnodes = NULL,
                uint64_t* msecs = NULL,
                int* movenum = NULL,
                char* move = NULL,
                const size_t movelen = 0) const;

protected:
  //--------------------------------------------------------------------------
  // senjo::ChessEngine methods (implemented in ClubFoot.cpp)
  //--------------------------------------------------------------------------
  uint64_t MyPerft(const int depth);
  std::string MyGo(const int depth,
                   const int movestogo = 0,
                   const uint64_t movetime = 0,
                   const uint64_t wtime = 0, const uint64_t winc = 0,
                   const uint64_t btime = 0, const uint64_t binc = 0,
                   std::string* ponder = NULL);

private:
  //--------------------------------------------------------------------------
  // global constants
  //--------------------------------------------------------------------------
  static const int _KING_SQR[128];      // king-square value table
  static const int _PIECE_SQR[12][128]; // piece-square value table

  //--------------------------------------------------------------------------
  // global variables
  //--------------------------------------------------------------------------
  static bool                _ext;            // check extensions
  static bool                _iid;            // internal iterative deepening
  static bool                _initialized;    // is the engine initialized?
  static bool                _nmp;            // null move pruning
  static bool                _oneReply;       // one reply extensions
  static char                _hist[0x100000]; // move performance history
  static int                 _board[128];     // piece positions
  static int                 _contempt;       // contempt for draw value
  static int                 _delta;          // delta pruning margin
  static int                 _depth;          // current root search depth
  static int                 _drawScore[2];   // score for getting a draw
  static int                 _lmr;            // late move reduction
  static int                 _movenum;        // current root search move number
  static int                 _rzr;            // razoring delta
  static int                 _seldepth;       // current selective search depth
  static int                 _tempo;          // tempo bonus for side to move
  static int                 _test;           // new feature test value
  static std::string         _currmove;       // current root search move
  static int64_t             _hashSize;       // transposition table byte size
  static ClubFoot            _node[MaxPlies]; // the node stack
  static std::set<uint64_t>  _seen;           // position keys already seen
  static Stats               _stats;          // misc counters
  static Stats               _totalStats;     // sum of misc counters
  static TranspositionTable  _tt;             // info about visited positions
  static senjo::EngineOption _optHash;        // hash size option
  static senjo::EngineOption _optClearHash;   // clear hash option
  static senjo::EngineOption _optContempt;    // contempt for draw option
  static senjo::EngineOption _optDelta;       // delta pruning margin option
  static senjo::EngineOption _optEXT;         // check extensions option
  static senjo::EngineOption _optIID;         // intrnl iterative deepening opt
  static senjo::EngineOption _optLMR;         // late move reductions option
  static senjo::EngineOption _optNMP;         // null move pruning option
  static senjo::EngineOption _optOneReply;    // one reply extensions options
  static senjo::EngineOption _optRZR;         // razoring delta option
  static senjo::EngineOption _optTempo;       // tempo bonus option
  static senjo::EngineOption _optTest;        // new feature testing option

  //--------------------------------------------------------------------------
  // position related variables (updated by Exec)
  //--------------------------------------------------------------------------
  int           king[2];     // king position for each color
  int           material[2]; // total material for each color
  int           mcount;      // half-move count
  int           rcount;      // reversible half-move count
  int           state;       // bit flags: side to move, castle rights, draw
  senjo::Square ep;          // en passant square
  CheckState    checkState;  // is the side to move in check?
  uint64_t      pieceKey;    // XOR-ed combination of piece/square hash values
  uint64_t      positionKey; // pieceKey ^ state ^ ep hash values

  //--------------------------------------------------------------------------
  // other variables
  //--------------------------------------------------------------------------
  int       ply;             // which ply is this node at?
  int       standPat;        // positional eval from perspective of side to move
  int       extended;        // plies the search at this node was extended
  int       reduced;         // plies the search at this node was reduced
  int       nullMoveOk;      // ok to try null move at this node?
  int       moveCount;       // number of moves in this node's 'moves' array
  int       moveIndex;       // which move in 'moves' array this node is on
  int       pvCount;         // move count in this node's principal variation
  int       pieceCount[14];  // piece counts per type
  int       kingEval[2];     // king positional evaluation score per color
  char      passers[128];    // location of passers (2) and semi-passers (1)
  char      openFile[2][8];  // files with no pawns (per color)
  ClubFoot* child;           // the node 1 ply after this node
  ClubFoot* parent;          // the node 1 ply before this one
  Move      killer[2];       // recent moves to cause beta cutoff at this node
  Move      moves[MaxMoves]; // moves generated for the position at this node
  Move      pv[MaxPlies];    // principal variation at this node

  //--------------------------------------------------------------------------
  //! Get the value of a given piece type
  //--------------------------------------------------------------------------
  static inline int ValueOf(const int piece) {
    static const int VALUE[] = {
      0,           0,
      PawnValue,   PawnValue,
      KnightValue, KnightValue,
      BishopValue, BishopValue,
      RookValue,   RookValue,
      QueenValue,  QueenValue,
      Infinity,    Infinity
    };
    assert(piece >= 0);
    assert(piece <= (Black|King));
    return VALUE[piece];
  }

  //--------------------------------------------------------------------------
  //! Used to update castling rights effected by touching a given square
  //--------------------------------------------------------------------------
  static inline int Touch(const int square) {
    static const int TOUCH[] = {
      WhiteLong, 0, 0, 0, WhiteCastleMask, 0, 0, WhiteShort, 0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      BlackLong, 0, 0, 0, BlackCastleMask, 0, 0, BlackShort, 0,0,0,0,0,0,0,0
    };
    assert(square >= senjo::Square::A1);
    assert(square <= senjo::Square::H8);
    return TOUCH[square];
  }

  //--------------------------------------------------------------------------
  //! Is it white's move or black's move in the position at this node?
  //--------------------------------------------------------------------------
  inline Color ColorToMove() const {
    return static_cast<Color>(COLOR_OF(state));
  }

  //--------------------------------------------------------------------------
  //! Set the size of the transposition table - this clears the table data
  //--------------------------------------------------------------------------
  void SetHashSize(const int64_t mbytes) {
    if (!_tt.Resize(static_cast<size_t>(mbytes))) {
      senjo::Output() << "cannot allocate hash table of " << mbytes << " MB";
    }
  }

  //--------------------------------------------------------------------------
  //! Clear all data in the transposition table
  //--------------------------------------------------------------------------
  void ClearHash() {
    _tt.Clear();
  }

  //--------------------------------------------------------------------------
  //! Clear move performance history
  //--------------------------------------------------------------------------
  void ClearHistory() {
    memset(_hist, 0, sizeof(_hist));
  }

  //--------------------------------------------------------------------------
  //! Clear killer moves in all nodes
  //--------------------------------------------------------------------------
  void ClearKillers() {
    killer[0].Clear();
    killer[1].Clear();
    for (int i = 0; i < MaxPlies; ++i) {
      _node[i].killer[0].Clear();
      _node[i].killer[1].Clear();
    }
  }

  //--------------------------------------------------------------------------
  //! Increment performance history for the given move
  //--------------------------------------------------------------------------
  inline void IncHistory(const Move& move, const bool check, const int depth) {
    if (!check && (depth > 0)) {
      const int idx = move.GetHistoryIndex();
      const int val = (_hist[idx] + depth + 2);
      _hist[idx] = static_cast<char>(std::min<int>(val, 40));
    }
  }

  //--------------------------------------------------------------------------
  //! Decrement performance history for the given move
  //--------------------------------------------------------------------------
  inline void DecHistory(const Move& move, const bool check) {
    if (!check) {
      const int idx = move.GetHistoryIndex();
      const int val = (_hist[idx] - 1);
      _hist[idx] = static_cast<char>(std::max<int>(val, -2));
    }
  }

  //--------------------------------------------------------------------------
  //! Add a move to the killer list for this node - replaces oldest (if any).
  //--------------------------------------------------------------------------
  inline void AddKiller(const Move& move) {
    if (move != killer[0]) {
      killer[1] = killer[0];
      killer[0] = move;
    }
  }

  //--------------------------------------------------------------------------
  //! Is the given move one of the killer moves at this node?
  //--------------------------------------------------------------------------
  inline bool IsKiller(const Move& move) const {
    return ((move == killer[0]) || (move == killer[1]));
  }

  //--------------------------------------------------------------------------
  //! Set a new principal variation on this node
  //! \param move First move of the new PV, remaining moves from 'child->pv'
  //--------------------------------------------------------------------------
  inline void UpdatePV(const Move& move) {
    pv[0] = move;
    if (child) {
      if ((pvCount = (child->pvCount + 1)) > 1) {
        assert(pvCount <= MaxPlies);
        memcpy((pv + 1), child->pv, (child->pvCount * sizeof(Move)));
      }
    }
    else {
      pvCount = 1;
    }
  }

  //--------------------------------------------------------------------------
  //! Output this node's principal variation
  //! \param score The score of the principal variation
  //! \param bound 0 means score is exact, -1 = upperbound, +1 = lowerbound
  //--------------------------------------------------------------------------
  void OutputPV(const int score, const int bound = 0) const {
    if (pvCount > 0) {
      const uint64_t msecs = (senjo::Now() - _startTime);
      senjo::Output out(senjo::Output::NoPrefix);

      const uint64_t nodes = (_stats.snodes + _stats.qnodes);
      out << "info depth " << _depth
          << " seldepth " << _seldepth
          << " nodes " << nodes
          << " time " << msecs
          << " nps " << static_cast<uint64_t>(senjo::Rate(nodes, msecs));

      if (bound) {
        out << " currmovenumber " << _movenum
            << " currmove " << _currmove;
      }

      if (abs(score) < MateScore) {
        out << " score cp " << score;
      }
      else {
        const int count = (Infinity - abs(score));
        const int mate = ((count + 1) / 2);
        out << " score mate " << ((score < 0) ? -mate : mate);
      }

      if (bound) {
        out << ((bound < 0) ? " upperbound" : " lowerbound");
      }
      else {
        out << " pv";
        for (int i = 0; i < pvCount; ++i) {
          const Move& move = pv[i];
          out << ' ' << move.ToString();
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Sort the 'moves' array according the string representation of the moves
  //--------------------------------------------------------------------------
  void SortMovesLexically() {
    std::sort(moves, (moves + moveCount), Move::LexicalCompare);
  }

  //--------------------------------------------------------------------------
  //! Scoot the move at the given index to the front of the move list
  //! \param idx The 'moves' array index of the move to scoot
  //--------------------------------------------------------------------------
  inline void ScootMoveToFront(int idx) {
    assert((idx >= 0) && (idx < moveCount));
    while (idx-- > 0) {
      moves[idx].SwapWith(moves[idx + 1]);
    }
  }

  //--------------------------------------------------------------------------
  //! Get the next move from this node's 'moves' array.
  //--------------------------------------------------------------------------
  inline Move* GetNextMove() {
    assert(moveIndex >= 0);
    assert((moveCount >= 0) && (moveCount < MaxMoves));

    if (moveIndex >= moveCount) {
      return NULL;
    }

    int best_index = moveIndex;
    int best_score = moves[moveIndex].GetScore();
    for (int i = (moveIndex + 1); i < moveCount; ++i) {
      if (moves[i].GetScore() > best_score) {
        best_score = moves[i].GetScore();
        best_index = i;
      }
    }
    if (best_index > moveIndex) {
      moves[moveIndex].SwapWith(moves[best_index]);
    }

    return (moves + moveIndex++);
  }

  //--------------------------------------------------------------------------
  //! Is the given square attacked by the specified color?
  //--------------------------------------------------------------------------
  template<Color color>
  inline bool AttackedBy(const senjo::Square& sqr) const {
    static const senjo::Direction DIRECTIONS[8] = {
      senjo::SouthWest, senjo::South,
      senjo::SouthEast, senjo::West,
      senjo::East,      senjo::NorthWest,
      senjo::North,     senjo::NorthEast
    };

    if (!sqr.IsValid()) {
      return false;
    }

    if ((_board[(sqr + senjo::KnightMove1).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove2).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove3).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove4).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove5).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove6).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove7).Name()] == (color|Knight)) ||
        (_board[(sqr + senjo::KnightMove8).Name()] == (color|Knight)))
    {
      return true;
    }

    int piece = 0;
    senjo::Direction direction;
    senjo::Square    dest;

    for (int i = 0; i < 8; ++i) {
      direction = DIRECTIONS[i];
      for (dest = (sqr + direction); dest.IsValid(); dest += direction) {
        switch ((piece = _board[dest.Name()])) {
        case (color|Pawn):
          if (sqr.DistanceTo(dest) == 1) {
            if (color == White) {
              switch (direction) {
              case senjo::SouthWest: case senjo::SouthEast:
                return true;
              default:
                break;
              }
            }
            else {
              switch (direction) {
              case senjo::NorthWest: case senjo::NorthEast:
                return true;
              default:
                break;
              }
            }
          }
          break;
        case (color|Bishop):
          switch (direction) {
          case senjo::SouthWest: case senjo::SouthEast:
          case senjo::NorthWest: case senjo::NorthEast:
            return true;
          default:
            break;
          }
          break;
        case (color|Rook):
          switch (direction) {
          case senjo::South: case senjo::West:
          case senjo::East:  case senjo::North:
            return true;
          default:
            break;
          }
          break;
        case (color|Queen):
          return true;
        case (color|King):
          if (sqr.DistanceTo(dest) == 1) {
            return true;
          }
          break;
        }
        if (piece) {
          break;
        }
      }
    }
    return false;
  }

  //--------------------------------------------------------------------------
  //! Is the position at this node a draw?
  //! \return True if the position at this node is a draw
  //--------------------------------------------------------------------------
  inline bool IsDraw() const {
    return ((state & Draw) || (rcount >= 100) || _seen.count(positionKey));
  }

  //--------------------------------------------------------------------------
  //! Is the king of the side to move in check?
  //--------------------------------------------------------------------------
  template<Color color>
  inline bool InCheck() {
    assert(color == ColorToMove());
    if (checkState == CheckState::Unknown) {
      if (AttackedBy<!color>(king[color])) {
        checkState = IsInCheck;
      }
      else {
        checkState = NotInCheck;
      }
    }
    return (checkState == IsInCheck);
  }

  //--------------------------------------------------------------------------
  //! Find the location of the least valuable piece attacking \p to square
  //! \param to The target square
  //! \return senjo::Square::None if no piece attacking \p to square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int SmallestAttacker(const senjo::Square& to) const {
    static const senjo::Direction DIRECTIONS[8] = {
      senjo::SouthWest, senjo::SouthEast,
      senjo::NorthWest, senjo::NorthEast,
      senjo::South,     senjo::West,
      senjo::East,      senjo::North
    };

    if (!to.IsValid()) {
      return senjo::Square::None;
    }

    senjo::Direction direction;
    senjo::Square    sqr;
    int              tmp;

    // pawn attackers
    if (color == White) {
      if ((_board[tmp = (to + senjo::SouthWest).Name()] == (color|Pawn)) ||
          (_board[tmp = (to + senjo::SouthEast).Name()] == (color|Pawn)))
      {
        return tmp;
      }
    }
    else {
      if ((_board[tmp = (to + senjo::NorthWest).Name()] == (color|Pawn)) ||
          (_board[tmp = (to + senjo::NorthEast).Name()] == (color|Pawn)))
      {
        return tmp;
      }
    }

    // knight attackers
    if ((_board[tmp = (to + senjo::KnightMove1).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove2).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove3).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove4).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove5).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove6).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove7).Name()] == (color|Knight)) ||
        (_board[tmp = (to + senjo::KnightMove8).Name()] == (color|Knight)))
    {
      return tmp;
    }

    int queen = senjo::Square::None;
    int king  = senjo::Square::None;

    // bishop attackers
    for (int i = 0; i < 4; ++i) {
      direction = DIRECTIONS[i];
      for (sqr = (to + direction); sqr.IsValid(); sqr += direction) {
        switch ((tmp = _board[sqr.Name()])) {
        case (color|Bishop): return sqr.Name();
        case (color|Queen):
          queen = sqr.Name();
          break;
        case (color|King):
          if (sqr.DistanceTo(to) == 1) king = sqr.Name();
          break;
        }
        if (tmp) break;
      }
    }

    // rook attackers
    for (int i = 4; i < 8; ++i) {
      direction = DIRECTIONS[i];
      for (sqr = (to + direction); sqr.IsValid(); sqr += direction) {
        switch ((tmp = _board[sqr.Name()])) {
        case (color|Rook): return sqr.Name();
        case (color|Queen):
          queen = sqr.Name();
          break;
        case (color|King):
          if (sqr.DistanceTo(to) == 1) king = sqr.Name();
          break;
        }
        if (tmp) break;
      }
    }

    // queen attackers
    if (queen != senjo::Square::None) {
      return queen;
    }

    // king attacker
    if ((king != senjo::Square::None) && !AttackedBy<!color>(to)) {
      return king;
    }

    return senjo::Square::None;
  }

  //--------------------------------------------------------------------------
  //! Get score after sensible exchange(s) made on \p to square
  //! \param to The square where piece exchanges would occur
  //! \return The score after the piece (if any) on \p to square was captured
  //--------------------------------------------------------------------------
  template<Color color>
  int StaticExchange(const senjo::Square& to) const {
    assert(to.IsValid());
    assert(_board[to.Name()]);
    assert(_board[to.Name()] < King);
    assert(COLOR_OF(_board[to.Name()]) != color);

    int value = 0;
    senjo::Square from = SmallestAttacker<color>(to);
    if (from.IsValid()) {
      const int piece = _board[from.Name()];
      const int cap = _board[to.Name()];
      value = ValueOf(cap);
      if (piece < King) {
        if (ValueOf(piece) >= value) {
          _board[to.Name()] = piece;
          _board[from.Name()] = 0;
          value = std::max<int>(0, (value - StaticExchange<!color>(to)));
          _board[from.Name()] = piece;
          _board[to.Name()] = cap;
        }
      }
    }
    return value;
  }

  //--------------------------------------------------------------------------
  //! Append a new move to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color>
  inline void AddMove(const senjo::Square& from,
                      const senjo::Square& to,
                      const Move::MoveType mtype,
                      const int cap = 0,
                      const int promo = PieceType::NoPiece)
  {
    assert((moveCount + 1) < MaxMoves);
    assert(from.IsValid());
    assert(to.IsValid());
    assert(COLOR_OF(_board[from.Name()]) == color);
    assert((!cap || ((cap >= Pawn) && (cap <= King))));
    assert(!promo || ((promo >= (White|Knight)) && (promo <= (Black|Queen))));

    const int fromSqr = from.Name();
    const int toSqr = to.Name();
    const int pc = _board[fromSqr];
    int score = (SquareValue(pc, toSqr) - SquareValue(pc, fromSqr));

    if (promo) {
      assert((mtype == Move::PawnCapture) || (mtype == Move::PawnPush));
      score += (ValueOf(cap) + ValueOf(promo) - PawnValue);
    }
    else {
      switch (mtype) {
      case Move::Invalid:
        assert(false);
        break;
      case Move::EnPassant:
        assert(pc == (color|Pawn));
        assert(cap == ((!color)|Pawn));
        score += PawnValue;
        break;
      case Move::Normal:
      case Move::PawnPush:
      case Move::PawnLung:
      case Move::PawnCapture:
        _board[toSqr] = pc;
        _board[fromSqr] = 0;
        score += (ValueOf(cap) - StaticExchange<!color>(to));
        _board[fromSqr] = pc;
        _board[toSqr] = cap;
        break;
      case Move::KingMove:
        score += ValueOf(cap);
        break;
      case Move::CastleShort:
      case Move::CastleLong:
        break;
      }
    }

    Move& move = moves[moveCount++];
    move.Init(mtype, fromSqr, toSqr, pc, cap, promo, score);
    if (!move.IsCapOrPromo()) {
      if (IsKiller(move)) {
        move.Score() += 50;
      }
      else {
        move.Score() += _hist[move.GetHistoryIndex()];
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Is the given move illegal due to king pin?
  //--------------------------------------------------------------------------
  template<Color color>
  inline bool Pinned(const senjo::Square& from,
                     const senjo::Square& to,
                     const Move::MoveType type) const
  {
    int piece = 0;
    senjo::Square sqr;
    switch (from.DirectionTo(senjo::Square(king[color]))) {
    case senjo::SouthWest:
      switch (from.DirectionTo(to)) {
      case senjo::SouthWest: case senjo::NorthEast: return false;
      default:
        for (sqr = (from + senjo::SouthWest); sqr.IsValid();
             sqr += senjo::SouthWest)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::NorthEast); sqr.IsValid();
             sqr += senjo::NorthEast)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Bishop)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::South:
      switch (from.DirectionTo(to)) {
      case senjo::South: case senjo::North: return false;
      default:
        for (sqr = (from + senjo::South); sqr.IsValid();
             sqr += senjo::South)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::North); sqr.IsValid();
             sqr += senjo::North)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Rook)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::SouthEast:
      switch (from.DirectionTo(to)) {
      case senjo::SouthEast: case senjo::NorthWest: return false;
      default:
        for (sqr = (from + senjo::SouthEast); sqr.IsValid();
             sqr += senjo::SouthEast)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::NorthWest); sqr.IsValid();
             sqr += senjo::NorthWest)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Bishop)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::West:
      switch (from.DirectionTo(to)) {
      case senjo::West: case senjo::East: return false;
      default:
        sqr = (from + senjo::West);
        if (type == Move::EnPassant) {
          switch (from.DirectionTo(to)) {
          case senjo::SouthWest: case senjo::NorthWest: sqr += senjo::West;
          default:
            break;
          }
        }
        for (; sqr.IsValid(); sqr += senjo::West) {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        sqr = (from + senjo::East);
        if (type == Move::EnPassant) {
          switch (from.DirectionTo(to)) {
          case senjo::SouthEast: case senjo::NorthEast: sqr += senjo::East;
          default:
            break;
          }
        }
        for (; sqr.IsValid(); sqr += senjo::East) {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Rook)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::East:
      switch (from.DirectionTo(to)) {
      case senjo::West: case senjo::East: return false;
      default:
        sqr = (from + senjo::East);
        if (type == Move::EnPassant) {
          switch (from.DirectionTo(to)) {
          case senjo::SouthEast: case senjo::NorthEast: sqr += senjo::East;
          default:
            break;
          }
        }
        for (; sqr.IsValid(); sqr += senjo::East) {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        sqr = (from + senjo::West);
        if (type == Move::EnPassant) {
          switch (from.DirectionTo(to)) {
          case senjo::SouthWest: case senjo::NorthWest: sqr += senjo::West;
          default:
            break;
          }
        }
        for (; sqr.IsValid(); sqr += senjo::West) {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Rook)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::NorthWest:
      switch (from.DirectionTo(to)) {
      case senjo::SouthEast: case senjo::NorthWest: return false;
      default:
        for (sqr = (from + senjo::NorthWest); sqr.IsValid();
             sqr += senjo::NorthWest)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::SouthEast); sqr.IsValid();
             sqr += senjo::SouthEast)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Bishop)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::North:
      switch (from.DirectionTo(to)) {
      case senjo::South: case senjo::North: return false;
      default:
        for (sqr = (from + senjo::North); sqr.IsValid();
             sqr += senjo::North)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::South); sqr.IsValid();
             sqr += senjo::South)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Rook)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    case senjo::NorthEast:
      switch (from.DirectionTo(to)) {
      case senjo::SouthWest: case senjo::NorthEast: return false;
      default:
        for (sqr = (from + senjo::NorthEast); sqr.IsValid();
             sqr += senjo::NorthEast)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if (piece != (color|King)) {
          return false;
        }
        for (sqr = (from + senjo::SouthWest); sqr.IsValid();
             sqr += senjo::SouthWest)
        {
          if ((piece = _board[sqr.Name()])) break;
        }
        if ((piece == ((!color)|Bishop)) || (piece == ((!color)|Queen))) {
          return true;
        }
      }
      break;
    default:
      break;
    }
    return false;
  }

  //--------------------------------------------------------------------------
  //! Is the given move illegal due to king pin?
  //--------------------------------------------------------------------------
  template<Color color>
  bool Pinned(const Move& move) const {
    return Pinned<color>(move.GetFrom(), move.GetTo(), move.GetType());
  }

  //--------------------------------------------------------------------------
  //! Generate moves that stop check (if in check)
  //! \return false if not in check and therefore no moves generated
  //--------------------------------------------------------------------------
  template<Color color>
  inline bool GetCheckEvasions() {
    static const senjo::Direction DIRECTIONS[16] = {
      senjo::KnightMove1, senjo::KnightMove2,
      senjo::KnightMove3, senjo::KnightMove4,
      senjo::KnightMove5, senjo::KnightMove6,
      senjo::KnightMove7, senjo::KnightMove8,
      senjo::SouthWest,   senjo::South,
      senjo::SouthEast,   senjo::West,
      senjo::East,        senjo::NorthWest,
      senjo::North,       senjo::NorthEast
    };

    int attackers = 0;
    int piece;
    int squareCount = 0;
    int squares[40];
    int tmp;
    int xray[2] = {-1,-1};
    int xrayCount = 0;

    senjo::Direction direction;
    senjo::Square    from = king[color];
    senjo::Square    to;

    // find pieces giving check and the squares between them and the king
    for (int i = 0; i < 16; ++i) {
      if (!(to = (from + (direction = DIRECTIONS[i])))) {
        continue;
      }
      switch (direction) {
      case senjo::KnightMove1: case senjo::KnightMove2:
      case senjo::KnightMove3: case senjo::KnightMove4:
      case senjo::KnightMove5: case senjo::KnightMove6:
      case senjo::KnightMove7: case senjo::KnightMove8:
        if (_board[to.Name()] == ((!color)|Knight)) {
          attackers++;
          squares[squareCount++] = to.Name();
        }
        break;
      case senjo::SouthWest: case senjo::SouthEast:
      case senjo::NorthWest: case senjo::NorthEast:
        switch ((piece = _board[to.Name()])) {
        case ((!color)|Pawn):
          if (color ? (direction < 0) : (direction > 0)) {
            attackers++;
            squares[squareCount++] = to.Name();
          }
          break;
        case ((!color)|Bishop): case ((!color)|Queen):
          attackers++;
          assert(xrayCount < 2);
          xray[xrayCount++] = (from - direction).Name();
          squares[squareCount++] = to.Name();
          break;
        default:
          squares[(tmp = squareCount)++] = to.Name();
          while (!piece && (to += direction).IsValid()) {
            squares[tmp++] = to.Name();
            switch ((piece = _board[to.Name()])) {
            case ((!color)|Bishop): case ((!color)|Queen):
              attackers++;
              assert(xrayCount < 2);
              xray[xrayCount++] = (from - direction).Name();
              squareCount = tmp;
              break;
            }
          }
        }
        break;
      case senjo::South: case senjo::West:
      case senjo::East: case senjo::North:
        tmp = squareCount;
        do {
          squares[tmp++] = to.Name();
          switch ((piece = _board[to.Name()])) {
          case ((!color)|Rook): case ((!color)|Queen):
            attackers++;
            assert(xrayCount < 2);
            xray[xrayCount++] = (from - direction).Name();
            squareCount = tmp;
            break;
          }
        } while (!piece && (to += direction).IsValid());
        break;
      case senjo::Unknown:
        break;
      }
    }

    switch (attackers) {
    case 0:
      checkState = NotInCheck;
      return false;
    case 1:
      // get non-king moves that block or capture the piece giving check
      for (tmp = 0; tmp < squareCount; ++tmp) {
        to = squares[tmp];
        if ((_board[to.Name()] == ((!color)|Pawn)) &&
            (ep == (to + (color ? senjo::South : senjo::North))))
        {
          if ((from = (ep + (color ? senjo::NorthWest
                                   : senjo::SouthWest))).IsValid() &&
              (_board[from.Name()] == (color|Pawn)) &&
              !Pinned<color>(from, ep, Move::EnPassant))
          {
            AddMove<color>(from, ep, Move::EnPassant, ((!color)|Pawn));
          }
          if ((from = (ep + (color ? senjo::NorthEast
                                   : senjo::SouthEast))).IsValid() &&
              (_board[from.Name()] == (color|Pawn)) &&
              !Pinned<color>(from, ep, Move::EnPassant))
          {
            AddMove<color>(from, ep, Move::EnPassant, ((!color)|Pawn));
          }
        }
        for (int i = 0; i < 16; ++i) {
          if (((direction = DIRECTIONS[i]) == to.DirectionTo(king[color])) ||
              !(from = (to + direction)))
          {
            continue;
          }
          switch (direction) {
          case senjo::KnightMove1: case senjo::KnightMove2:
          case senjo::KnightMove3: case senjo::KnightMove4:
          case senjo::KnightMove5: case senjo::KnightMove6:
          case senjo::KnightMove7: case senjo::KnightMove8:
            if ((_board[from.Name()] == (color|Knight)) &&
                !Pinned<color>(from, to, Move::Normal))
            {
              AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
            }
            break;
          case senjo::SouthWest: case senjo::SouthEast:
          case senjo::NorthWest: case senjo::NorthEast:
            switch ((piece = _board[from.Name()])) {
            case (color|Pawn):
              if ((color ? (direction > 0) : (direction < 0)) &&
                  _board[to.Name()] && !Pinned<color>(from, to, Move::Normal))
              {
                if (to.Y() == (color ? 0 : 7)) {
                  AddMove<color>(from, to, Move::PawnCapture, _board[to.Name()],
                      (color|Queen));
                  AddMove<color>(from, to, Move::PawnCapture, _board[to.Name()],
                      (color|Rook));
                  AddMove<color>(from, to, Move::PawnCapture, _board[to.Name()],
                      (color|Bishop));
                  AddMove<color>(from, to, Move::PawnCapture, _board[to.Name()],
                      (color|Knight));
                }
                else {
                  AddMove<color>(from, to, Move::PawnCapture,
                                 _board[to.Name()]);
                }
              }
              break;
            case (color|Bishop): case (color|Queen):
              if (!Pinned<color>(from, to, Move::Normal)) {
                AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
              }
              break;
            default:
              while (!piece && (from += direction).IsValid()) {
                switch ((piece = _board[from.Name()])) {
                case (color|Bishop): case (color|Queen):
                  if (!Pinned<color>(from, to, Move::Normal)) {
                    AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
                  }
                  break;
                }
              }
            }
            break;
          case senjo::South: case senjo::North:
            switch ((piece = _board[from.Name()])) {
            case (color|Pawn):
              if ((color ? (direction > 0) : (direction < 0)) &&
                  !_board[to.Name()] && !Pinned<color>(from, to, Move::Normal))
              {
                if (to.Y() == (color ? 0 : 7)) {
                  AddMove<color>(from, to, Move::PawnPush, _board[to.Name()],
                      (color|Queen));
                  AddMove<color>(from, to, Move::PawnPush, _board[to.Name()],
                      (color|Rook));
                  AddMove<color>(from, to, Move::PawnPush, _board[to.Name()],
                      (color|Bishop));
                  AddMove<color>(from, to, Move::PawnPush, _board[to.Name()],
                      (color|Knight));
                }
                else {
                  AddMove<color>(from, to, Move::PawnPush, _board[to.Name()]);
                }
              }
              break;
            case (color|Rook): case (color|Queen):
              if (!Pinned<color>(from, to, Move::Normal)) {
                AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
              }
              break;
            default:
              while (!piece && (from += direction).IsValid()) {
                switch ((piece = _board[from.Name()])) {
                case (color|Pawn):
                  if ((to.Y() == (color ? 4 : 3)) &&
                      (from.Y() == (color ? 6 : 1)) &&
                      !_board[to.Name()] &&
                      !Pinned<color>(from, to, Move::Normal))
                  {
                    AddMove<color>(from, to, Move::PawnLung, _board[to.Name()]);
                  }
                  break;
                case (color|Rook): case (color|Queen):
                  if (!Pinned<color>(from, to, Move::Normal)) {
                    AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
                  }
                  break;
                }
              }
            }
            break;
          case senjo::West: case senjo::East:
            do {
              switch ((piece = _board[from.Name()])) {
              case (color|Rook): case (color|Queen):
                if (!Pinned<color>(from, to, Move::Normal)) {
                  AddMove<color>(from, to, Move::Normal, _board[to.Name()]);
                }
                break;
              }
            } while (!piece && (from += direction).IsValid());
            break;
          case senjo::Unknown:
            break;
          }
        }
      }
      // fall through
    default:
      // get king moves
      from = king[color];
      for (int i = 8; i < 16; ++i) {
        if ((to = (from + (direction = DIRECTIONS[i]))).IsValid() &&
            (to.Name() != xray[0]) && (to.Name() != xray[1]) &&
            !AttackedBy<!color>(to))
        {
          switch ((piece = _board[to.Name()])) {
          case 0:
          case ((!color)|Pawn):
          case ((!color)|Knight):
          case ((!color)|Bishop):
          case ((!color)|Rook):
          case ((!color)|Queen):
            AddMove<color>(from, to, Move::KingMove, piece);
            break;
          }
        }
      }
      break;
    }

    checkState = IsInCheck;
    return true;
  }

  //--------------------------------------------------------------------------
  //! Add pawn promotions to this node's 'moves' array
  //! If 'underpromote' template parameter is false only queen promotions added
  //--------------------------------------------------------------------------
  template<Color color, bool underpromote>
  inline void GetPromos(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[2] = {
      (color ? senjo::SouthWest : senjo::NorthWest),
      (color ? senjo::SouthEast : senjo::NorthEast)
    };

    int cap;
    senjo::Square to;
    for (int i = 0; i < 2; ++i) {
      if ((to = (from + DIRECTIONS[i])).IsValid() &&
          !Pinned<color>(from, to, Move::PawnCapture))
      {
        switch ((cap = _board[to.Name()])) {
        case ((!color)|Knight):
        case ((!color)|Bishop):
        case ((!color)|Rook):
        case ((!color)|Queen):
          AddMove<color>(from, to, Move::PawnCapture, cap, (color|Queen));
          if (underpromote) {
            AddMove<color>(from, to, Move::PawnCapture, cap, (color|Rook));
            AddMove<color>(from, to, Move::PawnCapture, cap, (color|Bishop));
            AddMove<color>(from, to, Move::PawnCapture, cap, (color|Knight));
          }
          break;
        }
      }
    }
    if ((to = (from + (color ? senjo::South : senjo::North))).IsValid() &&
        !_board[to.Name()] && !Pinned<color>(from, to, Move::PawnPush))
    {
      AddMove<color>(from, to, Move::PawnPush, 0, (color|Queen));
      if (underpromote) {
        AddMove<color>(from, to, Move::PawnPush, 0, (color|Rook));
        AddMove<color>(from, to, Move::PawnPush, 0, (color|Bishop));
        AddMove<color>(from, to, Move::PawnPush, 0, (color|Knight));
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Add pawn captures, including en passant, to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color>
  inline void GetPawnCaps(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[2] = {
      (color ? senjo::SouthWest : senjo::NorthWest),
      (color ? senjo::SouthEast : senjo::NorthEast)
    };

    int cap;
    senjo::Square to;
    for (int i = 0; i < 2; ++i) {
      if ((to = (from + DIRECTIONS[i])).IsValid()) {
        if (to == ep) {
          if (!Pinned<color>(from, to, Move::EnPassant)) {
            AddMove<color>(from, to, Move::EnPassant, ((!color)|Pawn));
          }
        }
        else if (!Pinned<color>(from, to, Move::PawnCapture)) {
          switch ((cap = _board[to.Name()])) {
          case ((!color)|Pawn):
          case ((!color)|Knight):
          case ((!color)|Bishop):
          case ((!color)|Rook):
          case ((!color)|Queen):
            AddMove<color>(from, to, Move::PawnCapture, cap);
            break;
          }
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Add pawn moves that give check to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color>
  inline void GetPawnChecks(const senjo::Square& from) {
    senjo::Square to;
    if (!(to = (from + (color ? senjo::South : senjo::North))) ||
        _board[to.Name()] || Pinned<color>(from, to, Move::PawnPush))
    {
      return;
    }
    if (((to + (color ? senjo::SouthWest : senjo::NorthWest)) == king[!color])||
        ((to + (color ? senjo::SouthEast : senjo::NorthEast)) == king[!color])||
        Pinned<!color>(from, to, Move::PawnPush))
    {
      AddMove<color>(from, to, Move::PawnPush);
    }
    if ((from.Y() != (color ? 6 : 1)) ||
        !(to += (color ? senjo::South : senjo::North)) ||
        _board[to.Name()])
    {
      return;
    }
    if (((to + (color ? senjo::SouthWest : senjo::NorthWest)) == king[!color])||
        ((to + (color ? senjo::SouthEast : senjo::NorthEast)) == king[!color])||
        Pinned<!color>(from, to, Move::PawnLung))
    {
      AddMove<color>(from, to, Move::PawnLung);
    }
  }

  //--------------------------------------------------------------------------
  //! Add non-volatile pawn moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color>
  inline void GetPawnMoves(const senjo::Square& from) {
    senjo::Square to;
    if (!(to = (from + (color ? senjo::South : senjo::North))) ||
        _board[to.Name()] || Pinned<color>(from, to, Move::PawnPush))
    {
      return;
    }
    AddMove<color>(from, to, Move::PawnPush);
    if ((from.Y() == (color ? 6 : 1)) &&
        (to += (color ? senjo::South : senjo::North)).IsValid() &&
        !_board[to.Name()])
    {
      AddMove<color>(from, to, Move::PawnLung);
    }
  }

  //--------------------------------------------------------------------------
  //! Add knight moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color, MoveGenType type>
  inline void GetKnightMoves(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[8] = {
      senjo::KnightMove1, senjo::KnightMove2,
      senjo::KnightMove3, senjo::KnightMove4,
      senjo::KnightMove5, senjo::KnightMove6,
      senjo::KnightMove7, senjo::KnightMove8,
    };

    int cap;
    senjo::Square to;
    for (int i = 0; i < 8; ++i) {
      if ((to = (from + DIRECTIONS[i])).IsValid() &&
          !Pinned<color>(from, to, Move::Normal))
      {
        switch ((cap = _board[to.Name()])) {
        case 0:
          if ((type == AllMoves) || ((type == CapsAndChecks) &&
                                     Pinned<!color>(from, to, Move::Normal)))
          {
            AddMove<color>(from, to, Move::Normal);
          }
          break;
        case ((!color)|Pawn):
        case ((!color)|Knight):
        case ((!color)|Bishop):
        case ((!color)|Rook):
        case ((!color)|Queen):
          AddMove<color>(from, to, Move::Normal, cap);
          break;
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Add bishop moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color, MoveGenType type>
  inline void GetBishopMoves(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[4] = {
      senjo::SouthWest, senjo::SouthEast,
      senjo::NorthWest, senjo::NorthEast
    };

    int cap;
    int discovered;
    senjo::Square to;
    for (int i = 0; i < 4; ++i) {
      discovered = 0;
      if ((to = (from + DIRECTIONS[i])).IsValid() &&
          !Pinned<color>(from, to, Move::Normal)) do
      {
        if ((cap = _board[to.Name()])) {
          switch (cap) {
          case ((!color)|Pawn):
          case ((!color)|Knight):
          case ((!color)|Bishop):
          case ((!color)|Rook):
          case ((!color)|Queen):
            AddMove<color>(from, to, Move::Normal, cap);
            break;
          }
          break;
        }
        else if (type == AllMoves) {
          AddMove<color>(from, to, Move::Normal);
        }
        else if (type == CapsAndChecks) {
          if (!discovered) {
            discovered = Pinned<!color>(from, to, Move::Normal) ? 1 : 2;
          }
          if (discovered == 1) {
            AddMove<color>(from, to, Move::Normal);
          }
        }
      } while ((to += DIRECTIONS[i]).IsValid());
    }
  }

  //--------------------------------------------------------------------------
  //! Add rook moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color, MoveGenType type>
  inline void GetRookMoves(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[4] = {
      senjo::South, senjo::West,
      senjo::East,  senjo::North
    };

    int cap;
    int discovered;
    senjo::Square to;
    for (int i = 0; i < 4; ++i) {
      discovered = 0;
      if ((to = (from + DIRECTIONS[i])).IsValid() &&
          !Pinned<color>(from, to, Move::Normal)) do
      {
        if ((cap = _board[to.Name()])) {
          switch (cap) {
          case ((!color)|Pawn):
          case ((!color)|Knight):
          case ((!color)|Bishop):
          case ((!color)|Rook):
          case ((!color)|Queen):
            AddMove<color>(from, to, Move::Normal, cap);
            break;
          }
          break;
        }
        else if (type == AllMoves) {
          AddMove<color>(from, to, Move::Normal);
        }
        else if (type == CapsAndChecks) {
          if (!discovered) {
            discovered = Pinned<!color>(from, to, Move::Normal) ? 1 : 2;
          }
          if (discovered == 1) {
            AddMove<color>(from, to, Move::Normal);
          }
        }
      } while ((to += DIRECTIONS[i]).IsValid());
    }
  }

  //--------------------------------------------------------------------------
  //! Add queen moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color, MoveGenType type>
  inline void GetQueenMoves(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[8] = {
      senjo::SouthWest, senjo::South,
      senjo::SouthEast, senjo::West,
      senjo::East,      senjo::NorthWest,
      senjo::North,     senjo::NorthEast
    };

    int cap;
    int discovered;
    senjo::Square to;
    for (int i = 0; i < 8; ++i) {
      discovered = 0;
      if ((to = (from + DIRECTIONS[i])).IsValid() &&
          !Pinned<color>(from, to, Move::Normal)) do
      {
        if ((cap = _board[to.Name()])) {
          switch (cap) {
          case ((!color)|Pawn):
          case ((!color)|Knight):
          case ((!color)|Bishop):
          case ((!color)|Rook):
          case ((!color)|Queen):
            AddMove<color>(from, to, Move::Normal, cap);
            break;
          }
          break;
        }
        else if (type == AllMoves) {
          AddMove<color>(from, to, Move::Normal);
        }
        else if (type == CapsAndChecks) {
          if (!discovered) {
            discovered = Pinned<!color>(from, to, Move::Normal) ? 1 : 2;
          }
          if (discovered == 1) {
            AddMove<color>(from, to, Move::Normal);
          }
        }
      } while ((to += DIRECTIONS[i]).IsValid());
    }
  }

  //--------------------------------------------------------------------------
  //! Add king moves to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color, MoveGenType type>
  inline void GetKingMoves(const senjo::Square& from) {
    static const senjo::Direction DIRECTIONS[8] = {
      senjo::SouthWest, senjo::South,
      senjo::SouthEast, senjo::West,
      senjo::East,      senjo::NorthWest,
      senjo::North,     senjo::NorthEast
    };

    int cap;
    senjo::Square to;
    if (from == (color ? senjo::Square::E8 : senjo::Square::E1)) {
      if ((state & (color ? BlackShort : WhiteShort)) &&
          !_board[color ? senjo::Square::F8 : senjo::Square::F1] &&
          !_board[color ? senjo::Square::G8 : senjo::Square::G1] &&
          !AttackedBy<!color>(color ? senjo::Square::E8 : senjo::Square::E1) &&
          !AttackedBy<!color>(color ? senjo::Square::F8 : senjo::Square::F1) &&
          !AttackedBy<!color>(color ? senjo::Square::G8 : senjo::Square::G1))
      {
        if (type == AllMoves) {
          to = (color ? senjo::Square::G8 : senjo::Square::G1);
          AddMove<color>(from, to, Move::CastleShort);
        }
        else if (type == CapsAndChecks) {
          // TODO add move if rook gives check after castling
        }
      }
      if ((state & (color ? BlackLong : WhiteLong)) &&
          !_board[color ? senjo::Square::B8 : senjo::Square::B1] &&
          !_board[color ? senjo::Square::C8 : senjo::Square::C1] &&
          !_board[color ? senjo::Square::D8 : senjo::Square::D1] &&
          !AttackedBy<!color>(color ? senjo::Square::C8 : senjo::Square::C1) &&
          !AttackedBy<!color>(color ? senjo::Square::D8 : senjo::Square::D1) &&
          !AttackedBy<!color>(color ? senjo::Square::E8 : senjo::Square::E1))
      {
        if (type == AllMoves) {
          to = (color ? senjo::Square::C8 : senjo::Square::C1);
          AddMove<color>(from, to, Move::CastleLong);
        }
        else if (type == CapsAndChecks) {
          // TODO add move if rook gives check after castling
        }
      }
    }
    for (int i = 0; i < 8; ++i) {
      if ((to = (from + DIRECTIONS[i])).IsValid() && !AttackedBy<!color>(to)) {
        switch ((cap = _board[to.Name()])) {
        case 0:
          if ((type == AllMoves) || ((type == CapsAndChecks) &&
                                     Pinned<!color>(from, to, Move::KingMove)))
          {
            AddMove<color>(from, to, Move::KingMove);
          }
          break;
        case ((!color)|Pawn):
        case ((!color)|Knight):
        case ((!color)|Bishop):
        case ((!color)|Rook):
        case ((!color)|Queen):
          AddMove<color>(from, to, Move::KingMove, cap);
          break;
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Add legal moves to this node's 'moves' array
  //! If 'qsearch' template parameter is true only volatile moves are generated
  //--------------------------------------------------------------------------
  template<Color color, bool qsearch>
  inline void GenerateMoves(const int depth) {
    assert(color == ColorToMove());
    moveIndex = moveCount = 0;

    senjo::Square from;
    if ((checkState != NotInCheck) && GetCheckEvasions<color>()) {
      return;
    }
    else if (!qsearch) {
      // generate all moves
      for (from = senjo::Square::A1; from.IsValid(); ++from) {
        switch (_board[from.Name()]) {
        case (color|Pawn):
          if (from.Y() == (color ? 1 : 6)) {
            GetPromos<color, true>(from); // <color, underpromote=true>
          }
          else {
            GetPawnCaps<color>(from);
            GetPawnMoves<color>(from);
          }
          break;
        case (color|Knight): GetKnightMoves <color, AllMoves>(from); break;
        case (color|Bishop): GetBishopMoves <color, AllMoves>(from); break;
        case (color|Rook):   GetRookMoves   <color, AllMoves>(from); break;
        case (color|Queen):  GetQueenMoves  <color, AllMoves>(from); break;
        case (color|King):   GetKingMoves   <color, AllMoves>(from); break;
        }
      }
    }
    else if (depth < 0) {
      // quiescence search, no quiet checks
      for (from = senjo::Square::A1; from.IsValid(); ++from) {
        switch (_board[from.Name()]) {
        case (color|Pawn):
          if (from.Y() == (color ? 1 : 6)) {
            GetPromos<color, false>(from); // <color, underpromote=false>
          }
          else {
            GetPawnCaps<color>(from);
          }
          break;
        case (color|Knight): GetKnightMoves <color, Captures>(from); break;
        case (color|Bishop): GetBishopMoves <color, Captures>(from); break;
        case (color|Rook):   GetRookMoves   <color, Captures>(from); break;
        case (color|Queen):  GetQueenMoves  <color, Captures>(from); break;
        case (color|King):   GetKingMoves   <color, Captures>(from); break;
        }
      }
    }
    else {
      // quiescence search, plus quiet checks
      for (from = senjo::Square::A1; from.IsValid(); ++from) {
        switch (_board[from.Name()]) {
        case (color|Pawn):
          if (from.Y() == (color ? 1 : 6)) {
            GetPromos<color, false>(from); // <color, underpromote=false>
          }
          else {
            GetPawnCaps<color>(from);
            GetPawnChecks<color>(from);
          }
          break;
        case (color|Knight): GetKnightMoves <color, CapsAndChecks>(from); break;
        case (color|Bishop): GetBishopMoves <color, CapsAndChecks>(from); break;
        case (color|Rook):   GetRookMoves   <color, CapsAndChecks>(from); break;
        case (color|Queen):  GetQueenMoves  <color, CapsAndChecks>(from); break;
        case (color|King):   GetKingMoves   <color, CapsAndChecks>(from); break;
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //! Get endgame multiplier (ratio to use on endgame-only score values)
  //--------------------------------------------------------------------------
  inline float EndGame(const Color color) const {
    return (static_cast<float>(StartMaterial-material[!color])/StartMaterial);
  }

  //--------------------------------------------------------------------------
  //! Get midgame multiplier (ratio to use on midgame-only score values)
  //--------------------------------------------------------------------------
  inline float MidGame(const Color color) const {
    return (static_cast<float>(material[!color]) / StartMaterial);
  }

  //--------------------------------------------------------------------------
  //! Get static positional value of a given piece type on a given square
  //--------------------------------------------------------------------------
  inline int SquareValue(const int pc, const int sqr) const {
    assert((pc >= (White|Pawn)) && (pc <= (Black|King)));
    assert(senjo::Square(sqr).IsValid());
    if (pc < King) {
      return _PIECE_SQR[pc][sqr];
    }
    const float mid = (MidGame(COLOR_OF(pc)) * _KING_SQR[sqr]);
    const float end = (EndGame(COLOR_OF(pc)) * _KING_SQR[sqr + 8]);
    return static_cast<int>(mid + end);
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the pawn on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int PawnEval(const senjo::Square& sqr) {
    senjo::Square tmp;
    bool passed = true; // set to false below if not passed
    int score = SquareValue((color|Pawn), sqr.Name());
    const int x = sqr.X();
    const int y = sqr.Y();

    openFile[color][x] = 0;

    // find nearest friendly pawn on left flank
    senjo::Square left;
    for (tmp = (sqr + senjo::West); tmp.IsValid();
         tmp += (color ? senjo::North : senjo::South))
    {
      if (_board[tmp.Name()] == (color|Pawn)) {
        left = tmp;
        break;
      }
    }

    // find nearest friendly pawn on right flank
    senjo::Square right;
    for (tmp = (sqr + senjo::East); tmp.IsValid();
         tmp += (color ? senjo::North : senjo::South))
    {
      if (_board[tmp.Name()] == (color|Pawn)) {
        right = tmp;
        break;
      }
    }

    // penalty for center pawn blocked on home square
    switch (sqr.Name()) {
    case (color ? senjo::Square::D7 : senjo::Square::D2):
    case (color ? senjo::Square::E7 : senjo::Square::E2):
      if (_board[sqr.Name() + (color ? senjo::South : senjo::North)]) {
        score -= 16;
      }
      break;
    default:
      break;
    }

    // penalty if doubled
    for (tmp = (sqr + (color ? senjo::South : senjo::North));
         passed && tmp.IsValid(); // passed var simply used to break the loop
         tmp += (color ? senjo::South : senjo::North))
    {
      if (_board[tmp.Name()] == (color|Pawn)) {
        score -= 32;
        passed = false;
        break;
      }
      if (_board[tmp.Name()] == ((!color)|Pawn)) {
        passed = false;
        break;
      }
    }

    // bonus if passed or potentially passed
    if (passed) {
      senjo::Square leftOp;
      senjo::Square rightOp;
      if (y != (color ? 1 : 6)) {
        // find opposing pawn on left flank
        for (tmp = (sqr + (color ? senjo::SouthWest : senjo::NorthWest));
             tmp.IsValid(); tmp += (color ? senjo::North : senjo::South))
        {
          if (_board[tmp.Name()] == ((!color)|Pawn)) {
            leftOp = tmp;
            break;
          }
        }

        // find opposing pawn on right flank
        for (tmp = (sqr + (color ? senjo::SouthWest : senjo::NorthWest));
             tmp.IsValid(); tmp += (color ? senjo::North : senjo::South))
        {
          if (_board[tmp.Name()] == ((!color)|Pawn)) {
            rightOp = tmp;
            break;
          }
        }
      }

      // number of supporting flank pawns vs number of opposing flank pawns
      const int flanks = ((sqr.DistanceTo(left) < 2) +
                          (sqr.DistanceTo(right) < 2));
      const int opFlanks = (leftOp.IsValid() + rightOp.IsValid());

      // if friendly flank pawns >= opposing flank pawns we have a passer
      const int diff = (flanks - opFlanks);
      if (diff >= 0) {
        // remember pawn/square table also gives points for advancement
        static const int PASSER[6] = { 16, 24, 36, 52, 68, 80 };
        int bonus = PASSER[color ? (6 - y) : (y - 1)];

        // reduce bonus if only potentially passed
        if (opFlanks) {
          bonus /= 2;
          passed = false; // allow backward pawn penalty
          passers[sqr.Name()] = 1;
        }

        else {
          // increase bonus if completely passed and has support
          if (diff > 0) {
            bonus += (bonus / 3);
          }
          passers[sqr.Name()] = 2;
        }

        // reduce bonus if blocked
        if ((tmp = (sqr + (color ? senjo::South : senjo::North))).IsValid() &&
            _board[tmp.Name()])
        {
          bonus -= (bonus /= 4);
          passed = false; // allow backward pawn penalty
        }

        score += bonus;
      }
      else {
        passed = false; // not a passer nor a potential passer
      }
    }

    // penalty if over-extended/backward/isolated
    if (!passed && (x < 3) && (x > 4)) {
      int dist = std::min<int>(sqr.DistanceTo(left), sqr.DistanceTo(right));
      if (dist > 2) {
        score -= (2 * dist);

        // extra penalty if blocked by enemy knight or bishop
        switch (_board[(sqr + (color ? senjo::South : senjo::North)).Name()]) {
        case ((!color)|Knight):
        case ((!color)|Bishop):
          score -= 8;
          break;
        default:
          break;
        }
      }
    }

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the knight on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int KnightEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Knight), sqr.Name());

    // keep the knight close to the action
    // assuming the action is centered around the kings
    score += (2 * (8 - (sqr.DistanceTo(king[White]) +
                        sqr.DistanceTo(king[Black]))));
    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the bishop on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int BishopEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Bishop), sqr.Name());

    // stay close to fiendly king during endgame
    score += static_cast<int>(EndGame(color) *
                              (2 * (8 - sqr.DistanceTo(king[color]))));

    // bonus for being inline with enemy king
    switch (sqr.DirectionTo(king[!color])) {
    case senjo::SouthWest: case senjo::SouthEast:
    case senjo::NorthWest: case senjo::NorthEast:
      score += 8;
      break;
    default:
      break;
    }

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the rook on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int RookEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Rook), sqr.Name());

    // stay close to fiendly king during endgame
    score += static_cast<int>(EndGame(color) *
                              (2 * (8 - sqr.DistanceTo(king[color]))));

    // bonus for being on an open or half-open file
    const int x = sqr.X();
    if (openFile[color][x]) {
      score += (openFile[!color][x] ? 12 : 8);

      // bonus for being inline with enemy king
      switch (sqr.DirectionTo(king[!color])) {
      case senjo::South: case senjo::West: case senjo::East: case senjo::North:
        score += 8;
        break;
      default:
        break;
      }
    }

    // penalty for being on wrong side of king after losing ability to castle
    else if (!(state & (color ? BlackCastleMask : WhiteCastleMask))) {
      const int kx = (king[color] % 16);
      if (kx >= 4) {
        if (x >= kx) {
          score -= 20;
        }
      }
      else {
        if (x <= kx) {
          score -= 20;
        }
      }
    }

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the queen on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int QueenEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Queen), sqr.Name());

    // the queen is almost an entirely tactical piece. There are some positional
    // rules of thumb that could be applied for the queen, but we'll just leave
    // it up to the piece square table

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the king on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  inline int KingEval(const senjo::Square& sqr) {
    assert(sqr == king[color]);
    senjo::Square tmp;
    senjo::Square t2;
    const int y = sqr.Y();
    int score = SquareValue((color|King), sqr.Name());
    int val = 0;

    // penalty for being in front of own pieces
    for (tmp = (sqr + (color ? senjo::North : senjo::South)); tmp.IsValid();
         tmp += (color ? senjo::North : senjo::South))
    {
      const int pc = _board[tmp.Name()];
      if (pc) {
        switch (pc) {
        case (color|Pawn):
          score -= 16;
          break;
        case (color|Knight):
        case (color|Bishop):
        case (color|Rook):
        case (color|Queen):
          score -= 8;
          break;
        }
        break; // loop
      }
    }

    // penalty for open files on side of the board where the king is
    // bonus for pawn shield
    // penalty for pawn storm
    switch (sqr.X()) {
    case 0: case 1: case 2:
      val += (8 * (openFile[!color][0] + openFile[!color][1] +
                   openFile[!color][2] +
                   openFile[ color][0] + openFile[ color][1] +
                   openFile[ color][2]));
      for (tmp = senjo::Square(0, (y + (color ? senjo::South : senjo::North)));
           tmp.IsValid() && (tmp.X() < 3); ++tmp)
      {
        switch (_board[tmp.Name()]) {
          case (color|Pawn):    val += 10; break;
          case ((!color)|Pawn): val -= 10; break;
        }
        if ((t2 = (tmp + (color ? senjo::South : senjo::North))).IsValid()) {
          switch (_board[t2.Name()]) {
            case (color|Pawn):    val += 5; break;
            case ((!color)|Pawn): val -= 8; break;
          }
          if ((t2 += (color ? senjo::South : senjo::North)).IsValid()) {
            switch (_board[t2.Name()]) {
              case ((!color)|Pawn): val -= 6; break;
            }
          }
        }
      }
      break;
    case 3: case 4:
      val += (8 * (openFile[!color][2] + openFile[!color][3] +
                   openFile[!color][4] + openFile[!color][5] +
                   openFile[ color][2] + openFile[ color][3] +
                   openFile[ color][4] + openFile[ color][5]));
      for (tmp = senjo::Square(2, (y + (color ? senjo::South : senjo::North)));
           tmp.IsValid() && (tmp.X() < 6); ++tmp)
      {
        switch (_board[tmp.Name()]) {
          case (color|Pawn):    val += 10; break;
          case ((!color)|Pawn): val -= 10; break;
        }
        if ((t2 = (tmp + (color ? senjo::South : senjo::North))).IsValid()) {
          switch (_board[t2.Name()]) {
            case (color|Pawn):    val += 5; break;
            case ((!color)|Pawn): val -= 8; break;
          }
          if ((t2 += (color ? senjo::South : senjo::North)).IsValid()) {
            switch (_board[t2.Name()]) {
              case ((!color)|Pawn): val -= 6; break;
            }
          }
        }
      }
      break;
    case 5: case 6: case 7:
      val += (8 * (openFile[!color][5] + openFile[!color][6] +
                   openFile[!color][7] +
                   openFile[ color][5] + openFile[ color][6] +
                   openFile[ color][7]));
      for (tmp = senjo::Square(5, (y + (color ? senjo::South : senjo::North)));
           tmp.IsValid() && (tmp.X() < 8); ++tmp)
      {
        switch (_board[tmp.Name()]) {
          case (color|Pawn):    val += 10; break;
          case ((!color)|Pawn): val -= 10; break;
        }
        if ((t2 = (tmp + (color ? senjo::South : senjo::North))).IsValid()) {
          switch (_board[t2.Name()]) {
            case (color|Pawn):    val += 5; break;
            case ((!color)|Pawn): val -= 8; break;
          }
          if ((t2 += (color ? senjo::South : senjo::North)).IsValid()) {
            switch (_board[t2.Name()]) {
              case ((!color)|Pawn): val -= 6; break;
            }
          }
        }
      }
      break;
    default:
      assert(false);
    }
    if (val) {
      score -= static_cast<int>(MidGame(color) * val);
    }

    kingEval[color] = score;
    return score;
  }

  //--------------------------------------------------------------------------
  //! \brief Get static evaluation of the position at this node
  //! Most commonly used search techniques are practically required for any
  //! chess engine to be competitive.  There is a lot more room for
  //! personalization in positional evaluation (and move generation).
  //! Clubfoot is only intended to provide some basic concepts.
  //! For this reason, and due to the lack of bitboards to make expensive
  //! calculations such as piece mobility and threat detection feasible,
  //! very minimal evaluation techniques are used in Clubfoot.
  //--------------------------------------------------------------------------
  inline void Evaluate() {
    int pieceStack[32];
    int stackCount = 0;
    int pc;
    int eval = (material[White] - material[Black] +
                (ColorToMove() ? -_tempo : _tempo));

    memset(pieceCount, 0, sizeof(pieceCount));
    memset(passers, 0, sizeof(passers));
    memset(openFile, 1, sizeof(openFile));

    // evaluate pawns first to populate openFile map
    for (senjo::Square sqr = senjo::Square::A1; sqr.IsValid(); ++sqr) {
      switch ((pc = _board[sqr.Name()])) {
      case (White|Pawn): eval += PawnEval<White>(sqr); break;
      case (Black|Pawn): eval -= PawnEval<Black>(sqr); break;
      case (White|Knight):
      case (Black|Knight):
      case (White|Bishop):
      case (Black|Bishop):
      case (White|Rook):
      case (Black|Rook):
      case (White|Queen):
      case (Black|Queen):
        pieceCount[COLOR_OF(pc)]++;
        pieceStack[stackCount++] = sqr.Name();
        break;
      case (White|King):
      case (Black|King):
        assert(king[COLOR_OF(pc)] == sqr.Name());
        pieceStack[stackCount++] = sqr.Name();
        continue;
      default:
        assert(pc == 0);
        continue;
      }
      pieceCount[pc]++;
    }

    bool whiteCanWin = (pieceCount[White|Pawn] ||
                       (pieceCount[White|Knight] > 2) ||
                       (pieceCount[White|Bishop] > 1) ||
                       (pieceCount[White|Knight] && pieceCount[White|Bishop]) ||
                        pieceCount[White|Rook] ||
                        pieceCount[White|Queen]);

    bool blackCanWin = (pieceCount[Black|Pawn] ||
                       (pieceCount[Black|Knight] > 2) ||
                       (pieceCount[Black|Bishop] > 1) ||
                       (pieceCount[Black|Knight] && pieceCount[Black|Bishop]) ||
                        pieceCount[Black|Rook] ||
                        pieceCount[Black|Queen]);

    // draw due to insufficient mating material?
    if (!whiteCanWin && !blackCanWin) {
      state |= Draw;
      standPat = _drawScore[ColorToMove()];
      return;
    }

    // no pawns is bad
    if (!pieceCount[White|Pawn]) {
      eval -= 50;
    }
    if (!pieceCount[Black|Pawn]) {
      eval += 50;
    }

    // decrease value of loner knights and bishops
    if ((pieceCount[White] == 1) &&
        (pieceCount[White|Knight] || pieceCount[White|Bishop]))
    {
      eval -= 50;
    }
    if ((pieceCount[Black] == 1) &&
        (pieceCount[Black|Knight] || pieceCount[Black|Bishop]))
    {
      eval += 50;
    }

    // redundant knights are worth slightly less
    if (pieceCount[White|Knight] > 1) {
      eval -= (16 * (pieceCount[White|Knight] - 1));
    }
    if (pieceCount[Black|Knight] > 1) {
      eval += (16 * (pieceCount[Black|Knight] - 1));
    }

    // are there any pawns on the board?
    if ((pc = (pieceCount[White|Pawn] + pieceCount[Black|Pawn]))) {
      // increase value of 1 knight relative to # of pawns on the board
      pc = ((4 * pc) / 3); // inflate pawn count a little bit
      if (pieceCount[White|Knight]) {
        eval += pc;
      }
      if (pieceCount[Black|Knight]) {
        eval -= pc;
      }

      // increase value of 1 rook as pawns come off the board
      pc = ((4 * pc) / 3); // inflate pawn count a bit more
      if (pieceCount[White|Rook]) {
        eval += (28 - pc);
      }
      if (pieceCount[Black|Rook]) {
        eval -= (28 - pc);
      }

      // bishop pair more valuable as pawns come off the board
      // NOTE: this doesn't verify the bishops are on opposite color squares!
      if (pieceCount[White|Bishop] >= 2) {
        eval += (48 - pc);
      }
      if (pieceCount[Black|Bishop] >= 2) {
        eval -= (48 - pc);
      }
    }

    // evaluate pieces
    for (int i = 0; i < stackCount; ++i) {
      const senjo::Square sqr(pieceStack[i]);
      switch (_board[sqr.Name()]) {
      case (White|Knight): eval += KnightEval<White>(sqr); break;
      case (Black|Knight): eval -= KnightEval<Black>(sqr); break;
      case (White|Bishop): eval += BishopEval<White>(sqr); break;
      case (Black|Bishop): eval -= BishopEval<Black>(sqr); break;
      case (White|Rook):   eval += RookEval<White>(sqr);   break;
      case (Black|Rook):   eval -= RookEval<Black>(sqr);   break;
      case (White|Queen):  eval += QueenEval<White>(sqr);  break;
      case (Black|Queen):  eval -= QueenEval<Black>(sqr);  break;
      case (White|King):   eval += KingEval<White>(sqr);   break;
      case (Black|King):   eval -= KingEval<Black>(sqr);   break;
      default:
        assert(false);
      }
    }

    // reduce winning score if "winning" side can't win
    if (eval > 0) {
      if (!whiteCanWin) {
        eval = std::min<int>(50, (eval / 4));
      }
    }
    else if (eval < 0) {
      if (!blackCanWin) {
        eval = std::max<int>(-50, (eval / 4));
      }
    }

    // reduce winning score if rcount is getting large
    // NOTE: this destabilizes transposition table values
    //       because rcount is not encoded into positionKey
    if ((rcount > 25) && (abs(eval) > 8)) {
      eval = static_cast<int>(eval * (25.0 / rcount));
    }

    // round score to multiple of 8
    eval = ((eval / 8) * 8);

    // convert to perspective of side to move
    standPat = (ColorToMove() ? -eval : eval);
  }

  //--------------------------------------------------------------------------
  //! Execute a null move against the position at this node,
  //! the resulting position is applied to the given 'dest' node.
  //--------------------------------------------------------------------------
  template<Color color>
  inline void ExecNullMove(ClubFoot& dest) const {
    assert(ColorToMove() == color);
    assert(!AttackedBy<!color>(king[color]));
    assert(&dest != this);

    _stats.nullMoves++;

    dest.king[White] = king[White];
    dest.king[Black] = king[Black];
    dest.material[White] = material[White];
    dest.material[Black] = material[Black];
    dest.mcount = mcount;
    dest.rcount = rcount;
    dest.state = (state ^ ColorMask);
    dest.ep = senjo::Square::None;
    dest.checkState = CheckState::Unknown;
    dest.pieceKey = pieceKey;
    dest.positionKey = (pieceKey ^
                        _HASH[0][dest.state & FiveBits] ^
                        _HASH[0][senjo::Square::None]);
    dest.Evaluate();
  }

  //--------------------------------------------------------------------------
  //! Verify the given move is valid at this node
  //--------------------------------------------------------------------------
  template<Color color>
  inline int ValidateMove(const Move& move) const {
#ifdef NDEBUG
    int test = 0;
#define VASSERT(x) ++test; if (!(x)) return test;
#else
#define VASSERT(x) if (!(x)) { \
    PrintBoard(); \
    senjo::Output() << move.ToString(); \
    assert(false); \
  }
#endif

    VASSERT(ColorToMove() == color);
    VASSERT(move.IsValid());
    VASSERT(move.GetFrom().IsValid());
    VASSERT(move.GetTo().IsValid());

    const int from  = move.GetFromName();
    const int to    = move.GetToName();
    const int pc    = _board[from];
    const int cap   = _board[to];
    const int promo = move.GetPromo();

    VASSERT(pc);
    VASSERT(COLOR_OF(pc) == color);
    VASSERT((pc != (color|King)) || (from == king[color]));
    VASSERT(pc == move.GetPc());
    VASSERT(!cap || ((cap >= Pawn) && (cap < King)));
    VASSERT(!cap || (COLOR_OF(cap) != color));
    VASSERT(!promo || ((promo >= Knight) && (promo < King)));
    VASSERT(!promo || (COLOR_OF(promo) == color));

    switch (move.GetType()) {
    case Move::Normal:
      VASSERT(cap == move.GetCap());
      VASSERT(!promo);
      switch (pc) {
      case (color|Knight):
        switch (move.GetFrom().DirectionTo(move.GetTo())) {
        case senjo::KnightMove1: case senjo::KnightMove2:
        case senjo::KnightMove3: case senjo::KnightMove4:
        case senjo::KnightMove5: case senjo::KnightMove6:
        case senjo::KnightMove7: case senjo::KnightMove8:
          break;
        default:
          VASSERT(false);
        }
        break;
      case (color|Bishop):
        switch (move.GetFrom().DirectionTo(move.GetTo())) {
        case senjo::SouthWest: case senjo::SouthEast:
        case senjo::NorthWest: case senjo::NorthEast:
          break;
        default:
          VASSERT(false);
        }
        break;
      case (color|Rook):
        switch (move.GetFrom().DirectionTo(move.GetTo())) {
        case senjo::South: case senjo::West:
        case senjo::East:  case senjo::North:
          break;
        default:
          VASSERT(false);
        }
        break;
      case (color|Queen):
        switch (move.GetFrom().DirectionTo(move.GetTo())) {
        case senjo::SouthWest: case senjo::South:
        case senjo::SouthEast: case senjo::West:
        case senjo::East:      case senjo::NorthWest:
        case senjo::North:     case senjo::NorthEast:
          break;
        default:
          VASSERT(false);
        }
        break;
      default:
        VASSERT(false);
      }
      break;
    case Move::PawnPush:
      VASSERT(pc == (color|Pawn));
      VASSERT(!cap);
      VASSERT(to == (from + (color ? senjo::South : senjo::North)));
      VASSERT(promo ? (move.GetTo().Y() == (color ? 0 : 7))
                   : (move.GetTo().Y() != (color ? 0 : 7)));
      break;
    case Move::PawnLung:
      VASSERT(pc == (color|Pawn));
      VASSERT(!cap);
      VASSERT(!promo);
      VASSERT(to == (from + (2 * (color ? senjo::South : senjo::North))));
      VASSERT(move.GetFrom().Y() == (color ? 6 : 1));
      break;
    case Move::PawnCapture:
      VASSERT(pc == (color|Pawn));
      VASSERT(cap);
      VASSERT(cap == move.GetCap());
      VASSERT((to == (from + (color ? senjo::SouthWest : senjo::NorthWest))) ||
             (to == (from + (color ? senjo::SouthEast : senjo::NorthEast))));
      VASSERT(promo ? (move.GetTo().Y() == (color ? 0 : 7))
                   : (move.GetTo().Y() != (color ? 0 : 7)));
      break;
    case Move::EnPassant:
      VASSERT(pc == (color|Pawn));
      VASSERT(!cap);
      VASSERT(!promo);
      VASSERT(to == ep.Name());
      VASSERT((to == (from + (color ? senjo::SouthWest : senjo::NorthWest))) ||
             (to == (from + (color ? senjo::SouthEast : senjo::NorthEast))));
      VASSERT(move.GetFrom().Y() == (color ? 3 : 4));
      break;
    case Move::KingMove:
      VASSERT(pc == (color|King));
      VASSERT(cap == move.GetCap());
      VASSERT(!promo);
      VASSERT(move.GetFrom().DistanceTo(move.GetTo()) == 1);
      VASSERT(!AttackedBy<!color>(move.GetTo()));
      break;
    case Move::CastleShort:
      VASSERT(pc == (color|King));
      VASSERT(!cap);
      VASSERT(!promo);
      VASSERT(from == (color ? senjo::Square::E8 : senjo::Square::E1));
      VASSERT(  to == (color ? senjo::Square::G8 : senjo::Square::G1));
      VASSERT(!_board [color ? senjo::Square::F8 : senjo::Square::F1]);
      VASSERT(!_board [color ? senjo::Square::G8 : senjo::Square::G1]);
      VASSERT( _board [color ? senjo::Square::H8 : senjo::Square::H1] == (color|Rook));
      VASSERT(!AttackedBy<!color>(move.GetFrom()));
      VASSERT(!AttackedBy<!color>(move.GetFrom() + senjo::East));
      VASSERT(!AttackedBy<!color>(move.GetTo()));
      VASSERT(state & (color ? BlackShort : WhiteShort));
      break;
    case Move::CastleLong:
      VASSERT(pc == (color|King));
      VASSERT(!cap);
      VASSERT(!promo);
      VASSERT(from == (color ? senjo::Square::E8 : senjo::Square::E1));
      VASSERT(  to == (color ? senjo::Square::C8 : senjo::Square::C1));
      VASSERT( _board [color ? senjo::Square::A8 : senjo::Square::A1] == (color|Rook));
      VASSERT(!_board [color ? senjo::Square::B8 : senjo::Square::B1]);
      VASSERT(!_board [color ? senjo::Square::C8 : senjo::Square::C1]);
      VASSERT(!_board [color ? senjo::Square::D8 : senjo::Square::D1]);
      VASSERT(!AttackedBy<!color>(move.GetFrom()));
      VASSERT(!AttackedBy<!color>(move.GetFrom() + senjo::West));
      VASSERT(!AttackedBy<!color>(move.GetTo()));
      VASSERT(state & (color ? BlackLong : WhiteLong));
      break;
    default:
      VASSERT(false);
    }
    return 0;
  }

  //--------------------------------------------------------------------------
  //! Execute the given move against the position at this node,
  //! the resulting position is applied to the given 'dest' node.
  //--------------------------------------------------------------------------
  template<Color color>
  inline void Exec(const Move& move, ClubFoot& dest) const {
    assert(ColorToMove() == color);
    assert(ValidateMove<color>(move) == 0);

    _stats.execs++;
    _seen.insert(positionKey);

    switch (move.GetType()) {
    case Move::Invalid:
      senjo::Output() << "Cannot execute invalid move";
      return;
    case Move::Normal:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      dest.king[White] = king[White];
      dest.king[Black] = king[Black];
      dest.material[color] = material[color];
      dest.material[!color] = (material[!color] - ValueOf(move.GetCap()));
      dest.mcount = (mcount + 1);
      dest.rcount = (move.GetCap() ? 0 : (rcount + 1));
      dest.state = ((state ^ ColorMask) &
                     ~Touch(move.GetFromName()) &
                     ~Touch(move.GetToName()));
      dest.ep = senjo::Square::None;
      dest.pieceKey = (pieceKey ^
          _HASH[move.GetPc()][move.GetFromName()] ^
          _HASH[move.GetPc()][move.GetToName()] ^
          (move.GetCap() ? _HASH[move.GetCap()][move.GetToName()] : 0));
      break;
    case Move::PawnPush:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPromo() ? move.GetPromo()
                                                 : move.GetPc();
      dest.king[White] = king[White];
      dest.king[Black] = king[Black];
      if (move.GetPromo()) {
        dest.material[color] = (material[color] - PawnValue +
                                ValueOf(move.GetPromo()));
      }
      else {
        dest.material[color] = material[color];
      }
      dest.material[!color] = material[!color];
      dest.mcount = (mcount + 1);
      dest.rcount = 0;
      dest.state = (state ^ ColorMask);
      dest.ep = senjo::Square::None;
      if (move.GetPromo()) {
        dest.pieceKey = (pieceKey ^
            _HASH[color|Pawn][move.GetFromName()] ^
            _HASH[move.GetPromo()][move.GetToName()]);
      }
      else {
        dest.pieceKey = (pieceKey ^
            _HASH[color|Pawn][move.GetFromName()] ^
            _HASH[color|Pawn][move.GetToName()]);
      }
      break;
    case Move::PawnLung:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      dest.king[White] = king[White];
      dest.king[Black] = king[Black];
      dest.material[White] = material[White];
      dest.material[Black] = material[Black];
      dest.mcount = (mcount + 1);
      dest.rcount = 0;
      dest.state = (state ^ ColorMask);
      dest.ep = (move.GetFrom() + (color ? senjo::South : senjo::North));
      dest.pieceKey = (pieceKey ^
          _HASH[color|Pawn][move.GetFromName()] ^
          _HASH[color|Pawn][move.GetToName()]);
      break;
    case Move::PawnCapture:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPromo() ? move.GetPromo()
                                                 : move.GetPc();
      dest.king[White] = king[White];
      dest.king[Black] = king[Black];
      if (move.GetPromo()) {
        dest.material[color] = (material[color] - PawnValue +
                                ValueOf(move.GetPromo()));
      }
      else {
        dest.material[color] = material[color];
      }
      dest.material[!color] = (material[!color] - ValueOf(move.GetCap()));
      dest.mcount = (mcount + 1);
      dest.rcount = 0;
      dest.state = ((state ^ ColorMask) & ~Touch(move.GetToName()));
      dest.ep = senjo::Square::None;
      if (move.GetPromo()) {
        dest.pieceKey = (pieceKey ^
            _HASH[color|Pawn][move.GetFromName()] ^
            _HASH[move.GetPromo()][move.GetToName()] ^
            _HASH[move.GetCap()][move.GetToName()]);
      }
      else {
        dest.pieceKey = (pieceKey ^
            _HASH[color|Pawn][move.GetFromName()] ^
            _HASH[color|Pawn][move.GetToName()] ^
            _HASH[move.GetCap()][move.GetToName()]);
      }
      break;
    case Move::EnPassant:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      _board[move.GetToName() + (color ? senjo::North : senjo::South)] = 0;
      dest.king[White] = king[White];
      dest.king[Black] = king[Black];
      dest.material[color] = material[color];
      dest.material[!color] = (material[!color] - PawnValue);
      dest.mcount = (mcount + 1);
      dest.rcount = 0;
      dest.state = (state ^ ColorMask);
      dest.ep = senjo::Square::None;
      dest.pieceKey = (pieceKey ^
          _HASH[color|Pawn][move.GetFromName()] ^
          _HASH[color|Pawn][move.GetToName()] ^
          _HASH[(!color)|Pawn][move.GetToName() +
                (color ? senjo::North : senjo::South)]);
      break;
    case Move::KingMove:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      dest.king[color] = move.GetToName();
      dest.king[!color] = king[!color];
      dest.material[color] = material[color];
      dest.material[!color] = (material[!color] - ValueOf(move.GetCap()));
      dest.mcount = (mcount + 1);
      dest.rcount = (move.GetCap() ? 0 : (rcount + 1));
      dest.state = ((state ^ ColorMask) & ~Touch(move.GetToName()) &
                     (color ? ~BlackCastleMask : ~WhiteCastleMask));
      dest.ep = senjo::Square::None;
      dest.pieceKey = (pieceKey ^
          _HASH[color|King][move.GetFromName()] ^
          _HASH[color|King][move.GetToName()] ^
          (move.GetCap() ? _HASH[move.GetCap()][move.GetToName()] : 0));
      break;
    case Move::CastleShort:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      _board[color ? senjo::Square::H8 : senjo::Square::H1] = 0;
      _board[color ? senjo::Square::F8 : senjo::Square::F1] = (color|Rook);
      dest.king[color] = move.GetToName();
      dest.king[!color] = king[!color];
      dest.material[White] = material[White];
      dest.material[Black] = material[Black];
      dest.mcount = (mcount + 1);
      dest.rcount = (rcount + 1);
      dest.state = ((state ^ ColorMask) &
                     (color ? ~BlackCastleMask : ~WhiteCastleMask));
      dest.ep = senjo::Square::None;
      dest.pieceKey = (pieceKey ^
          _HASH[color|King][move.GetFromName()] ^
          _HASH[color|King][move.GetToName()] ^
          _HASH[color|Rook][color ? senjo::Square::F8 : senjo::Square::F1] ^
          _HASH[color|Rook][color ? senjo::Square::H8 : senjo::Square::H1]);
      break;
    case Move::CastleLong:
      _board[move.GetFromName()] = 0;
      _board[move.GetToName()] = move.GetPc();
      _board[color ? senjo::Square::A8 : senjo::Square::A1] = 0;
      _board[color ? senjo::Square::D8 : senjo::Square::D1] = (color|Rook);
      dest.king[color] = move.GetToName();
      dest.king[!color] = king[!color];
      dest.material[White] = material[White];
      dest.material[Black] = material[Black];
      dest.mcount = (mcount + 1);
      dest.rcount = (rcount + 1);
      dest.state = ((state ^ ColorMask) &
                     (color ? ~BlackCastleMask : ~WhiteCastleMask));
      dest.ep = senjo::Square::None;
      dest.pieceKey = (pieceKey ^
          _HASH[color|King][move.GetFromName()] ^
          _HASH[color|King][move.GetToName()] ^
          _HASH[color|Rook][color ? senjo::Square::A8 : senjo::Square::A1] ^
          _HASH[color|Rook][color ? senjo::Square::D8 : senjo::Square::D1]);
      break;
    }
    dest.checkState = CheckState::Unknown;
    dest.positionKey = (dest.pieceKey ^
                        _HASH[0][dest.state & FiveBits] ^
                        _HASH[0][dest.ep.Name()]);
    dest.Evaluate();
  }

  //--------------------------------------------------------------------------
  //! Undo the last move executed at this node
  //! \param move Must be the last move executed on this node
  //--------------------------------------------------------------------------
  template<Color color>
  inline void Undo(const Move& move) const {
    switch (move.GetType()) {
    case Move::Invalid:
      senjo::Output() << "Cannot undo invalid move";
      return;
    case Move::Normal:
      _board[move.GetToName()] = move.GetCap();
      break;
    case Move::PawnPush:
    case Move::PawnLung:
      _board[move.GetToName()] = 0;
      break;
    case Move::PawnCapture:
      _board[move.GetToName()] = move.GetCap();
      break;
    case Move::EnPassant:
      _board[move.GetToName()] = 0;
      _board[move.GetToName() + (color ? senjo::North : senjo::South)] = move.GetCap();
      break;
    case Move::KingMove:
      _board[move.GetToName()] = move.GetCap();
      break;
    case Move::CastleShort:
      _board[move.GetToName()] = 0;
      _board[color ? senjo::Square::H8 : senjo::Square::H1] = (color|Rook);
      _board[color ? senjo::Square::F8 : senjo::Square::F1] = 0;
      break;
    case Move::CastleLong:
      _board[move.GetToName()] = 0;
      _board[color ? senjo::Square::A8 : senjo::Square::A1] = (color|Rook);
      _board[color ? senjo::Square::D8 : senjo::Square::D1] = 0;
      break;
    }
    _board[move.GetFromName()] = move.GetPc();
    _seen.erase(positionKey);
  }

  //--------------------------------------------------------------------------
  //! Perform perft search at an innder node
  //! \returns The number of leaf nodes encountered
  //--------------------------------------------------------------------------
  template<Color color>
  uint64_t PerftSearch(const int depth) {
    GenerateMoves<color, false>(depth);
    if (!child || (depth <= 1)) {
      return moveCount;
    }

    uint64_t count = 0;

    for (; !_stop && (moveIndex < moveCount); ++moveIndex) {
      const Move& move = moves[moveIndex];
      Exec<color>(move, *child);
      count += child->PerftSearch<!color>(depth - 1);
      Undo<color>(move);
    }

    return count;
  }

  //--------------------------------------------------------------------------
  //! Perform perft search at the root node
  //! \returns The number of leaf nodes encountered
  //--------------------------------------------------------------------------
  template<Color color>
  uint64_t PerftSearchRoot(const int depth) {
    assert(_initialized);
    assert(ply == 0);
    assert(!parent);
    assert(child == _node);

    if (_debug) {
      PrintBoard();
      senjo::Output() << GetFEN();
    }

    GenerateMoves<color, false>(depth);
    SortMovesLexically();

    uint64_t count = 0;

    if (child && (depth > 1)) {
      for (; !_stop && (moveIndex < moveCount); ++moveIndex) {
        const Move& move = moves[moveIndex];
        Exec<color>(move, *child);
        const uint64_t c = child->PerftSearch<!color>(depth - 1);
        Undo<color>(move);
        senjo::Output() << move.ToString() << ' ' << c << ' ' << move.GetScore();
        count += c;
      }
    }
    else {
      for (; !_stop && (moveIndex < moveCount); ++moveIndex) {
        senjo::Output() << moves[moveIndex].ToString() << " 1 "
                        << moves[moveIndex].GetScore();
        count++;
      }
    }

    return count;
  }

  //--------------------------------------------------------------------------
  //! \brief Perform quiescence search at this node
  //! Quiescence search only examines volatile moves.  And if the side to move
  //! is not in check "standing pat" is considered as an option.
  //! All moves are considered volatile when in check.  Otherwise volatile
  //! moves are promotions, captures, and at depth 0 checks.
  //! Although this isn't considered to be the "primary" search routine, this
  //! is where the majority of search time is spent.
  //--------------------------------------------------------------------------
  template<Color color>
  int QSearch(int alpha, const int beta, const int depth) {
    assert(alpha < beta);
    assert(abs(alpha) <= Infinity);
    assert(abs(beta) <= Infinity);
    assert(depth <= 0);

    _stats.qnodes++;
    if (ply > _seldepth) {
      _seldepth = ply;
    }

    pvCount = 0;
    if (IsDraw()) {
      return _drawScore[color];
    }

    // mate distance pruning and standPat beta cutoff when not in check
    const bool check = InCheck<color>();
    int best = (check ? (ply - Infinity) : standPat);
    if ((best >= beta) || !child) {
      return best;
    }
    if (best > alpha) {
      alpha = best;
    }

    // do we have anything for this position in the transposition table?
    Move firstMove;
    HashEntry* entry = _tt.Probe(positionKey);
    if (entry) {
      switch (entry->GetPrimaryFlag()) {
      case HashEntry::Checkmate: return (ply - Infinity);
      case HashEntry::Stalemate: return _drawScore[color];
      case HashEntry::UpperBound:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        if (entry->score <= alpha) {
          pv[0] = firstMove;
          pvCount = 1;
          return entry->score;
        }
        break;
      case HashEntry::ExactScore:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        pv[0] = firstMove;
        pvCount = 1;
        if ((entry->score >= beta) && !firstMove.IsCapOrPromo()) {
          AddKiller(firstMove);
        }
        return entry->score;
      case HashEntry::LowerBound:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        if (entry->score >= beta) {
          pv[0] = firstMove;
          pvCount = 1;
          if (!firstMove.IsCapOrPromo()) {
            AddKiller(firstMove);
          }
          return entry->score;
        }
        break;
      default:
        assert(false);
      }

      // don't use firstMove unless it's a cap or promo, or we're in check
      if (!check && !firstMove.IsCapOrPromo()) {
        firstMove.Clear();
      }
    }

    // search firstMove if we have it
    const int orig_alpha = alpha;
    if (firstMove.IsValid()) {
      _stats.qexecs++;
      Exec<color>(firstMove, *child);
      firstMove.Score() = -child->QSearch<!color>(-beta, -alpha, (depth - 1));
      Undo<color>(firstMove);
      if (_stop) {
        return beta;
      }
      if (firstMove.GetScore() >= best) {
        best = firstMove.GetScore();
        UpdatePV(firstMove);
        if (firstMove.GetScore() >= beta) {
          if (!firstMove.IsCapOrPromo()) {
            AddKiller(firstMove);
          }
          if (check) {
            firstMove.Score() = beta;
            _tt.Store(positionKey, firstMove, 0, HashEntry::LowerBound, 0);
          }
          return best;
        }
        if (firstMove.GetScore() > alpha) {
          alpha = firstMove.GetScore();
        }
      }
    }

    // generate moves
    GenerateMoves<color, true>(depth);
    if (moveCount <= 0) {
      assert(!firstMove.IsValid());
      if (check) {
        _tt.StoreCheckmate(positionKey);
        return (ply - Infinity);
      }
      // don't call _tt.StoreStalemate()!!!
      return standPat;
    }

    // search 'em
    Move* move;
    while ((move = GetNextMove())) {
      if (firstMove == (*move)) {
        assert(firstMove.IsValid());
        continue;
      }

      _stats.qexecs++;
      Exec<color>(*move, *child);
      if (_delta && !check && (depth < 0) && !move->GetPromo() &&
          ((standPat + ValueOf(move->GetCap()) + _delta) <= alpha) &&
          !child->InCheck<!color>())
      {
        Undo<color>(*move);
        if (_stop) {
          return beta;
        }
        _stats.deltaCount++;
        continue;
      }

      move->Score() = -child->QSearch<!color>(-beta, -alpha, (depth - 1));
      Undo<color>(*move);
      if (_stop) {
        return beta;
      }

      if (move->GetScore() > best) {
        best = move->GetScore();
        UpdatePV(*move);
        if (move->GetScore() >= beta) {
          if (!move->IsCapOrPromo()) {
            AddKiller(*move);
          }
          if (check) {
            move->Score() = beta;
            _tt.Store(positionKey, *move, 0, HashEntry::LowerBound, 0);
          }
          return best;
        }
        if (move->GetScore() > alpha) {
          alpha = move->GetScore();
        }
      }
    }

    assert(best <= alpha);
    assert(alpha < beta);

    if (check && (pvCount > 0)) {
      if (alpha > orig_alpha) {
        assert(pv[0].GetScore() == alpha);
        _tt.Store(positionKey, pv[0], 0, HashEntry::ExactScore, 0);
      }
      else {
        assert(alpha == orig_alpha);
        assert(pv[0].GetScore() <= alpha);
        pv[0].Score() = alpha;
        _tt.Store(positionKey, pv[0], 0, HashEntry::UpperBound, 0);
      }
    }

    return best;
  }

  //--------------------------------------------------------------------------
  //! \brief Perform alpha/beta search on an internal node
  //! This is the main search routine
  //--------------------------------------------------------------------------
  template<Color color>
  int Search(int alpha, const int beta, int depth, const bool cutNode) {
    assert(alpha < beta);
    assert(abs(alpha) <= Infinity);
    assert(abs(beta) <= Infinity);
    assert(depth > 0);

    _stats.snodes++;
    extended  = 0;
    reduced   = 0;
    moveCount = 0;
    pvCount   = 0;

    if (IsDraw()) {
      return _drawScore[color];
    }

    // mate distance pruning
    int best = (ply - Infinity);
    if ((best >= beta) || !child) {
      return best;
    }
    if (best > alpha) {
      alpha = best;
    }

    // extend depth if in check and previous ply not extended
    const bool check = InCheck<color>();
    if (_ext && check && !parent->extended) {
      _stats.chkExts++;
      extended++;
      depth++;
    }

    // do we have anything for this position in the transposition table?
    const bool pvNode = ((alpha + 1) != beta);
    HashEntry* entry = _tt.Probe(positionKey);
    Move firstMove;
    if (entry) {
      switch (entry->GetPrimaryFlag()) {
      case HashEntry::Checkmate: return (ply - Infinity);
      case HashEntry::Stalemate: return _drawScore[color];
      case HashEntry::UpperBound:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        if ((entry->depth >= depth) && (entry->score <= alpha) &&
            (!pvNode || entry->HasPvFlag()))
        {
          pv[0] = firstMove;
          pvCount = 1;
          return entry->score;
        }
        if ((entry->depth >= (depth - 3)) && (entry->score < beta)) {
          nullMoveOk = 0;
        }
        break;
      case HashEntry::ExactScore:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        if (entry->depth >= depth) {
          pv[0] = firstMove;
          pvCount = 1;
          if ((entry->score >= beta) && !firstMove.IsCapOrPromo()) {
            IncHistory(firstMove, check, entry->depth);
            AddKiller(firstMove);
          }
          return entry->score;
        }
        if ((entry->depth >= (depth - 3)) && (entry->score < beta)) {
          nullMoveOk = 0;
        }
        break;
      case HashEntry::LowerBound:
        firstMove.Init(entry->moveBits, entry->score);
        assert(ValidateMove<color>(firstMove) == 0);
        if ((entry->depth >= depth) && (entry->score >= beta) &&
            (!pvNode || entry->HasPvFlag()))
        {
          pv[0] = firstMove;
          pvCount = 1;
          if (!firstMove.IsCapOrPromo()) {
            IncHistory(firstMove, check, entry->depth);
            AddKiller(firstMove);
          }
          return entry->score;
        }
        break;
      default:
        assert(false);
      }
      if (entry->HasExtendedFlag() && !extended && !parent->extended) {
        _stats.hashExts++;
        extended++;
        depth++;
      }
    }

    // razoring
    // if we're well below alpha and q-search doesn't show a saving tactic
    // return q-search result
    int eval;
    if (_rzr && !check && !pvNode && !firstMove.IsValid() && (depth <= 2) &&
        (abs(alpha) < WinningScore) &&
        ((standPat + _rzr + (64 * (depth - 1))) < alpha))
    {
      _stats.rzrCount++;
      eval = QSearch<color>(alpha, beta, 0);
      if (_stop) {
        return beta;
      }
      if (eval <= alpha) {
        _stats.rzrCutoffs++;
        return eval;
      }
    }

    // null move pruning
    // if we can get a score >= beta without even making a move, return beta
    if (_nmp && nullMoveOk && !check && !pvNode && (depth > 1) &&
        (standPat >= beta) && (abs(beta) < WinningScore) &&
        (pieceCount[color] > 1))
    {
      ExecNullMove<color>(*child);
      child->nullMoveOk = 0;
      const int rdepth = std::max<int>(0, (depth - 3 - (depth / 6) -
                                           ((standPat - beta) >= 400)));
      eval = (rdepth > 0)
          ? -child->Search<!color>(-beta, (1 - beta), rdepth, false)
          : -child->QSearch<!color>(-beta, (1 - beta), 0);
      if (_stop) {
        return beta;
      }
      if (eval >= beta) {
        // TODO do verification search at high depths
        pvCount = 0;
        _stats.nmCutoffs++;
        return beta; // do not return eval
      }
      // TODO return alpha if threat detected and it was enabled by parent move
      // TODO if threat and NOT caused by parent move do parent->nullMoveOK = 0
    }

    // internal iterative deepening if no firstMove in transposition table
    if (_iid && !check && !firstMove.IsValid() && (beta < Infinity) &&
        (depth > (pvNode ? 3 : 5)))
    {
      assert(!pvCount);
      _stats.iidCount++;
      const int saved = nullMoveOk;
      nullMoveOk = 0;
      eval = Search<color>((beta - 1), beta, (depth - (pvNode ? 2 : 4)), true);
      nullMoveOk = saved;
      if (_stop || !pvCount) {
        return eval;
      }
      if (eval >= beta) {
        _stats.iidBeta++;
      }
      assert(pv[0].IsValid());
      firstMove = pv[0];
    }

    // make sure firstMove is populated
    if (!firstMove.IsValid()) {
      GenerateMoves<color, false>(depth);
      if (moveCount <= 0) {
        if (check) {
          _tt.StoreCheckmate(positionKey);
          return (ply - Infinity);
        }
        _tt.StoreStalemate(positionKey);
        return _drawScore[0];
      }
      firstMove = *GetNextMove();
      if (_oneReply && (moveCount == 1) && !extended) {
        _stats.oneReplyExts++;
        extended++;
        depth++;
      }
    }

    // search first move with full alpha/beta window
    const int orig_alpha = alpha;
    Exec<color>(firstMove, *child);
    child->nullMoveOk = 1;
    eval = (depth > 1)
        ? -child->Search<!color>(-beta, -alpha, (depth - 1), !cutNode)
        : -child->QSearch<!color>(-beta, -alpha, 0);
    Undo<color>(firstMove);
    if (_stop) {
      return beta;
    }
    if (eval > alpha) {
      alpha = eval;
    }
    if (eval >= best) {
      best = eval;
      UpdatePV(firstMove);
      if (eval >= beta) {
        if (!firstMove.IsCapOrPromo()) {
          IncHistory(firstMove, check, depth);
          AddKiller(firstMove);
        }
        firstMove.Score() = beta;
        _tt.Store(positionKey, firstMove, depth, HashEntry::LowerBound,
                  ((extended ? HashEntry::Extended : 0) |
                   (pvNode ? HashEntry::FromPV : 0)));
        return best;
      }
    }
    else if (!firstMove.IsCapOrPromo()) {
      DecHistory(firstMove, check);
    }

    // generate moves if we haven't done so already
    if (moveCount <= 0) {
      GenerateMoves<color, false>(depth);
      assert(moveCount > 0);
      if (_oneReply && (moveCount == 1) && !extended) {
        _stats.oneReplyExts++;
        extended++;
        depth++;
      }
    }

    bool lmr_ok = (_lmr && !pvNode && !check && (depth > (_lmr + 1)));
    int pvDepth = depth;
    int newDepth;
    Move* move;

    // search remaining moves
    moveIndex = 0;
    while ((move = GetNextMove())) {
      if (firstMove == (*move)) {
        assert(firstMove.IsValid());
        continue;
      }

      Exec<color>(*move, *child);

      // late move reductions
      _stats.lateMoves++;
      _stats.lmCandidates += lmr_ok;
      if (lmr_ok &&
          !move->IsCapOrPromo() &&
          !IsKiller(*move) &&
          !((move->GetPc() == (color|Pawn)) &&
            (move->GetTo().Y() == (color ? 1 : 6))) &&
          !child->InCheck<!color>() &&
          (_hist[move->GetHistoryIndex()] < 0))
      {
        _stats.lmReductions++;
        reduced = _lmr;
        if ((depth > (reduced + 1)) && (_hist[move->GetHistoryIndex()] < -1)) {
          _stats.lmDoubleRed++;
          reduced++;
        }
      }
      else {
        reduced = 0;
      }

      // first search with a null window to quickly see if it improves alpha
      newDepth = (depth - 1 - reduced);
      child->nullMoveOk = 1;
      eval = (newDepth > 0)
          ? -child->Search<!color>(-(alpha + 1), -alpha, newDepth, true)
          : -child->QSearch<!color>(-(alpha + 1), -alpha, 0);
      child->nullMoveOk = 0;

      // re-search at full depth?
      if (!_stop && reduced && (eval > alpha)) {
        assert(depth > 1);
        _stats.lmResearches++;
        reduced = 0;
        eval = -child->Search<!color>(-(alpha + 1), -alpha, (depth - 1), false);
        if (!_stop && (eval > alpha)) {
          _stats.lmConfirmed++;
        }
      }

      // re-search with full window?
      if (!_stop && pvNode && (eval > alpha)) {
        assert(!reduced);
        eval = (depth > 1)
            ? -child->Search<!color>(-beta, -alpha, (depth - 1), false)
            : -child->QSearch<!color>(-beta, -alpha, 0);
      }

      Undo<color>(*move);
      if (_stop) {
        return beta;
      }
      if (eval > alpha) {
        alpha = eval;
        _stats.lmAlphaIncs++;
      }
      if (eval > best) {
        best = eval;
        UpdatePV(*move);
        pvDepth = (depth - reduced);
        if (eval >= beta) {
          if (!move->IsCapOrPromo()) {
            IncHistory(*move, check, pvDepth);
            AddKiller(*move);
          }
          move->Score() = beta;
          _tt.Store(positionKey, *move, pvDepth, HashEntry::LowerBound,
                    ((extended ? HashEntry::Extended : 0) |
                     (pvNode ? HashEntry::FromPV : 0)));
          return best;
        }
      }
      else if (!move->IsCapOrPromo()) {
        DecHistory(*move, check);
      }
    }

    assert(moveCount > 0);
    assert(best <= alpha);
    assert(alpha < beta);

    if (pvCount > 0) {
      pv[0].Score() = alpha;
      if (alpha > orig_alpha) {
        if (!pv[0].IsCapOrPromo()) {
          IncHistory(pv[0], check, pvDepth);
        }
        _tt.Store(positionKey, pv[0], pvDepth, HashEntry::ExactScore,
            ((extended ? HashEntry::Extended : 0) |
             (pvNode ? HashEntry::FromPV : 0)));
      }
      else {
        assert(alpha == orig_alpha);
        _tt.Store(positionKey, pv[0], pvDepth, HashEntry::UpperBound,
            ((extended ? HashEntry::Extended : 0) |
             (pvNode ? HashEntry::FromPV : 0)));
      }
    }

    return best;
  }

  //--------------------------------------------------------------------------
  //! \brief Find the best move at the current position
  //! Performs an Iterative Deepening (ID) search with aspiration windows.
  //! Principal Variation Search (PVS) logic is used, which does a null-window
  //! search on lines after a principal variation has been established.
  //! This null-window search is simply a fast way to determine whether a line
  //! looks like it has the potential to be better than the current PV.  In
  //! cases where a line looks like it may be better than the current PV
  //! regular searches are done on the line using aspiration windows.
  //--------------------------------------------------------------------------
  template<Color color>
  std::string SearchRoot(const int depth) {
    assert(_initialized);
    assert(ply == 0);
    assert(!parent);
    assert(child == _node);

    extended = 0;
    reduced = 0;

    if (_debug) {
      PrintBoard();
      senjo::Output() << GetFEN();
    }

    GenerateMoves<color, false>(1);
    if (moveCount <= 0) {
      senjo::Output() << "No legal moves";
      return std::string();
    }
    while (GetNextMove()) { ; } // sort 'em

    // move transposition table move (if any) to front of list
    if (moveCount > 1) {
      HashEntry* entry = _tt.Probe(positionKey);
      if (entry) {
        switch (entry->GetPrimaryFlag()) {
        case HashEntry::Checkmate:
        case HashEntry::Stalemate:
          assert(false);
          break;
        case HashEntry::UpperBound:
        case HashEntry::ExactScore:
        case HashEntry::LowerBound: {
          const Move ttMove(entry->moveBits, entry->score);
          assert(ValidateMove<color>(ttMove) == 0);
          for (int i = 0; i < moveCount; ++i) {
            if (moves[i] == ttMove) {
              ScootMoveToFront(i);
              break;
            }
          }
          break;
        }}
      }
    }

    // initial principal variation
    pvCount = 1;
    pv[0] = moves[0];

    // return immediately if we only have one move
    if (moveCount == 1) {
      OutputPV(pv[0].GetScore());
      return pv[0].ToString();
    }

    Move* move;
    bool  showPV = true;
    int   alpha;
    int   best = standPat;
    int   beta;
    int   delta;

    // iterative deepening
    for (int d = 0; !_stop && (d < depth); ++d) {
      _seldepth = _depth = (d + 1);
      child->nullMoveOk = (d > 0);

      showPV = true;
      delta  = 25;
      alpha  = std::max<int>((best - delta), -Infinity);
      beta   = std::min<int>((best + delta), +Infinity);

      for (moveIndex = 0; !_stop && (moveIndex < moveCount); ++moveIndex) {
        move      = (moves + moveIndex);
        _currmove = move->ToString();
        _movenum  = (moveIndex + 1);

        // aspiration window search
        Exec<color>(*move, *child);
        while (!_stop) {
          move->Score() = (_depth > 1)
              ? -child->Search<!color>(-beta, -alpha, (_depth - 1), false)
              : -child->QSearch<!color>(-beta, -alpha, 0);

          // expand aspiration window and re-search?
          if (!_stop && ((move->GetScore() >= beta) ||
                         ((move->GetScore() <= alpha) && (_movenum == 1))))
          {
            delta *= 20;
            if (move->GetScore() >= beta) {
              assert(move->GetScore() < Infinity);
              beta = std::min<int>((move->GetScore() + delta), Infinity);
              if ((senjo::Now() - _startTime) > 1000) {
                OutputPV(move->GetScore(), 1); // report lowerbound
              }
            }
            else {
              assert(move->GetScore() > -Infinity);
              alpha = std::max<int>((move->GetScore() - delta), -Infinity);
              if ((senjo::Now() - _startTime) > 1000) {
                OutputPV(move->GetScore(), -1); // report upperbound
              }
            }
            continue;
          }
          delta = 25;
          break;
        }
        Undo<color>(*move);

        // do we have a new principal variation?
        if (!_stop && ((_movenum == 1) || (move->GetScore() > best))) {
          UpdatePV(*move);
          OutputPV(move->GetScore());
          showPV = false;
          _tt.Store(positionKey, *move, _depth, HashEntry::ExactScore,
                    HashEntry::FromPV);

          // set null aspiration window now that we have a principal variation
          best = alpha = move->GetScore();
          beta = (alpha + 1);

          ScootMoveToFront(moveIndex);
        }
      }
    }

    if (showPV) {
      OutputPV(pv[0].GetScore());
    }

    return pv[0].ToString();
  }

  //--------------------------------------------------------------------------
  //! Initialize search variables
  //--------------------------------------------------------------------------
  void InitSearch() {
    _currmove.clear();
    _stats.Clear();
    _tt.ResetCounters();

    _depth    = 0;
    _movenum  = 0;
    _seldepth = 0;

    _drawScore[ColorToMove()] = -_contempt;
    _drawScore[!ColorToMove()] = _contempt;
  }
};

} // namespace clubfoot

#endif // CLUBFOOT_H
