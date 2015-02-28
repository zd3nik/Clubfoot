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
  std::list<senjo::EngineOption> GetOptions() const;
  bool SetEngineOption(const std::string& optionName,
                       const std::string& optionValue);
  void Initialize();
  bool IsInitialized() const;
  const char* SetPosition(const char* fen);
  const char* MakeMove(const char* str);
  std::string GetFEN() const;
  void PrintBoard() const;
  bool WhiteToMove() const;
  void ClearSearchData();
  void PonderHit();
  void GetStats(int* depth,
                int* seldepth = NULL,
                uint64_t* nodes = NULL,
                uint64_t* qnodes = NULL,
                uint64_t* msecs = NULL,
                int* movenum = NULL,
                char* move = NULL,
                const size_t movelen = 0) const;

  //--------------------------------------------------------------------------
  //! Override senjo::ChessEngine::Quit to do some cleanup
  //--------------------------------------------------------------------------
  void Quit() {
    // stop searching and exit the timer thread
    senjo::ChessEngine::Quit();

    // free up memory allocated for the transposition table
    SetHashSize(0);
  }

  //--------------------------------------------------------------------------
  //! Constructor
  //--------------------------------------------------------------------------
  ClubFoot::ClubFoot()
    : ply(0),
      child(NULL),
      parent(NULL)
  {
    // don't call Initialize() here
    // multiple ClubFoot classes will be constructed in the '_node' array
  }

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
  // global variables
  //--------------------------------------------------------------------------
  static bool                _initialized;    // is the engine initialized?
  static bool                _ext;            // check extensions
  static bool                _iid;            // internal iterative deepening
  static bool                _lmr;            // late move reductions
  static bool                _nmp;            // null move pruning
  static char                _hist[0x100000]; // move performance history
  static int                 _board[128];     // piece positions
  static int                 _depth;          // current root search depth
  static int                 _movenum;        // current root search move number
  static int                 _seldepth;       // current selective search depth
  static std::string         _currmove;       // current root search move
  static int64_t             _hashSize;       // transposition table byte size
  static uint64_t            _execs;          // number of Exec calls
  static uint64_t            _qnodes;         // number of QSearch calls
  static uint64_t            _nullMoves;      // number of ExecNullMove calls
  static uint64_t            _nmCutoffs;      // number of null moves cutoffs
  static ClubFoot            _node[MaxPlies]; // the node stack
  static std::set<uint64_t>  _seen;           // position keys already seen
  static TranspositionTable  _tt;             // info about visited positions
  static senjo::EngineOption _optClearHash;   // clear hash option
  static senjo::EngineOption _optHash;        // hash size option
  static senjo::EngineOption _optEXT;         // check extensions option
  static senjo::EngineOption _optIID;         // intrnl iterative deepening opt
  static senjo::EngineOption _optLMR;         // late move reductions option
  static senjo::EngineOption _optNMP;         // null move pruning option

  //--------------------------------------------------------------------------
  // global constants
  //--------------------------------------------------------------------------
  static const int KING_SQR[128];      // king-square value table
  static const int PIECE_SQR[12][128]; // piece-square value table

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
  int       eval;            // positional evaluation from White's perspective
  int       pieceCount;      // how many pieces does the side to move have?
  int       extended;        // plies the search at this node was extended
  int       reduced;         // plies the search at this node was reduced
  int       nullMoveOk;      // ok to try null move at this node?
  int       moveCount;       // number of moves in this nodes 'moves' array
  int       moveIndex;       // which move in 'moves' array this node is on
  int       pvCount;         // move count in this node's principal variation
  char      openFile[2][8];  // files with no pawns (per color)
  ClubFoot* child;           // the node 1 ply after this node
  ClubFoot* parent;          // the node 1 ply before this one
  Move      killer[2];       // recent moves to cause beta cutoff at this node
  Move      moves[MaxMoves]; // moves generated for the position at this node
  Move      pv[MaxPlies];    // principal variation at this node

  //--------------------------------------------------------------------------
  //! Get the value of a given piece type
  //--------------------------------------------------------------------------
  static int ValueOf(const int piece) {
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
  static int Touch(const int square) {
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
  Color ColorToMove() const {
    return static_cast<Color>(COLOR_OF(state));
  }

  //--------------------------------------------------------------------------
  //! Set the size of the transposition table - this clears the table data
  //--------------------------------------------------------------------------
  void SetHashSize(const int64_t mbytes) {
    if (!_tt.Resize(mbytes)) {
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
  void IncHistory(const Move& move, const bool inCheck, const int depth) {
    if (!inCheck && (depth > 0)) {
      const int idx = move.GetHistoryIndex();
      const int val = (_hist[idx] + depth);
      _hist[idx] = static_cast<char>(std::min<int>(val, 100));
    }
  }

  //--------------------------------------------------------------------------
  //! Decrement performance history for the given move
  //--------------------------------------------------------------------------
  void DecHistory(const Move& move, const bool inCheck) {
    if (!inCheck) {
      const int idx = move.GetHistoryIndex();
      const int val = (_hist[idx] - 1);
      _hist[idx] = static_cast<char>(std::max<int>(val, -100));
    }
  }

  //--------------------------------------------------------------------------
  //! Add a move to the killer list for this node - replaces oldest (if any).
  //--------------------------------------------------------------------------
  void AddKiller(const Move& move) {
    if (move != killer[0]) {
      killer[1] = killer[0];
      killer[0] = move;
    }
  }

  //--------------------------------------------------------------------------
  //! Is the given move one of the killer moves at this node?
  //--------------------------------------------------------------------------
  bool IsKiller(const Move& move) const {
    return ((move == killer[0]) || (move == killer[1]));
  }

  //--------------------------------------------------------------------------
  //! Set a new principal variation on this node
  //! \param move First move of the new PV, remaining moves from 'child->pv'
  //--------------------------------------------------------------------------
  void UpdatePV(const Move& move) {
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

      out << "info depth " << _depth
          << " seldepth " << _seldepth
          << " nodes " << _execs
          << " time " << msecs
          << " nps " << static_cast<uint64_t>(senjo::Rate(_execs, msecs));

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
  //! Get the next move from this node's 'moves' array.
  //--------------------------------------------------------------------------
  Move* GetNextMove() {
    assert(moveIndex >= 0);
    assert((moveCount >= 0) && (moveCount < MaxMoves));

    if (moveIndex >= moveCount) {
      return NULL;
    }

    int best_index = moveIndex;
    int best_score = moves[moveIndex].Score();
    for (int i = (moveIndex + 1); i < moveCount; ++i) {
      if (moves[i].Score() > best_score) {
        best_score = moves[i].Score();
        best_index = i;
      }
    }
    if (best_index > moveIndex) {
      Move tmp = moves[moveIndex];
      moves[moveIndex] = moves[best_index];
      moves[best_index] = tmp;
    }

    return (moves + moveIndex++);
  }

  //--------------------------------------------------------------------------
  //! Is the given square attacked by the specified color?
  //--------------------------------------------------------------------------
  template<Color color>
  bool AttackedBy(const senjo::Square& sqr) const {
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
  //! Is the king of the side to move in check?
  //--------------------------------------------------------------------------
  template<Color color>
  bool KingInCheck() {
    assert(color == ColorToMove());
    if (checkState == CheckState::Unknown) {
      if (AttackedBy<!color>(king[color])) {
        checkState = InCheck;
      }
      else {
        checkState = NotInCheck;
      }
    }
    return (checkState == InCheck);
  }

  //--------------------------------------------------------------------------
  //! Find the location of the least valuable piece attacking \p to square
  //! \param to The target square
  //! \return senjo::Square::None if no piece attacking \p to square
  //--------------------------------------------------------------------------
  template<Color color>
  int SmallestAttacker(const senjo::Square& to) const {
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
      if (piece < King) {
        _board[to.Name()] = piece;
        _board[from.Name()] = 0;
        value = std::max<int>(0, (ValueOf(cap) - StaticExchange<!color>(to)));
        _board[from.Name()] = piece;
        _board[to.Name()] = cap;
      }
      else {
        value = ValueOf(cap);
      }
    }
    return value;
  }

  //--------------------------------------------------------------------------
  //! Append a new move to this node's 'moves' array
  //--------------------------------------------------------------------------
  template<Color color>
  void AddMove(const senjo::Square& from,
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
    if (IsKiller(move)) {
      move.Score() += 50;
    }
  }

  //--------------------------------------------------------------------------
  //! Is the given move illegal due to king pin?
  //--------------------------------------------------------------------------
  template<Color color>
  bool Pinned(const senjo::Square& from,
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
  bool GetCheckEvasions() {
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

    checkState = InCheck;
    return true;
  }

  //--------------------------------------------------------------------------
  //! Add pawn promotions to this node's 'moves' array
  //! If 'underpromote' template parameter is false only queen promotions added
  //--------------------------------------------------------------------------
  template<Color color, bool underpromote>
  void GetPromos(const senjo::Square& from) {
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
  void GetPawnCaps(const senjo::Square& from) {
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
  void GetPawnChecks(const senjo::Square& from) {
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
  void GetPawnMoves(const senjo::Square& from) {
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
  void GetKnightMoves(const senjo::Square& from) {
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
  void GetBishopMoves(const senjo::Square& from) {
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
  void GetRookMoves(const senjo::Square& from) {
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
  void GetQueenMoves(const senjo::Square& from) {
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
  void GetKingMoves(const senjo::Square& from) {
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
  void GenerateMoves(const int depth) {
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
  float EndGame(const Color color) const {
    return (static_cast<float>(StartMaterial-material[!color])/StartMaterial);
  }

  //--------------------------------------------------------------------------
  //! Get midgame multiplier (ratio to use on midgame-only score values)
  //--------------------------------------------------------------------------
  float ClubFoot::MidGame(const Color color) const {
    return (static_cast<float>(material[!color]) / StartMaterial);
  }

  //--------------------------------------------------------------------------
  //! Get static positional value of a given piece type on a given square
  //--------------------------------------------------------------------------
  int SquareValue(const int pc, const int sqr) const {
    assert((pc >= (White|Pawn)) && (pc <= (Black|King)));
    assert(senjo::Square(sqr).IsValid());
    if (pc < King) {
      return PIECE_SQR[pc][sqr];
    }
    const float mid = (MidGame(COLOR_OF(pc)) * KING_SQR[sqr]);
    const float end = (EndGame(COLOR_OF(pc)) * KING_SQR[sqr + 8]);
    return static_cast<int>(mid + end);
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the pawn on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  int PawnEval(const senjo::Square& sqr) {
    senjo::Square tmp;
    bool passed = true; // set to false below if not passed
    int score = SquareValue((color|Pawn), sqr.Name());
    int x = sqr.X();

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
    }

    // penalty if doubled
    for (tmp = (sqr + (color ? senjo::South : senjo::North)); tmp.IsValid();
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
      if (sqr.Y() != (color ? 1 : 6)) {
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

      // if friendly flank pawns outweigh opposing flank pawns we have a passer
      const int diff = (flanks - opFlanks);
      if (diff >= 0) {
        // remember pawn/square table also gives points for advancement
        static const int PASSER[6] = { 16, 24, 36, 52, 68, 80 };
        int bonus = PASSER[color ? (6 - sqr.Y()) : (sqr.Y() - 1)];

        // increase bonus if it has extra support
        if (diff > 0) {
          bonus += (bonus / 2);
        }

        // reduce bonus if only potentially passed
        if (opFlanks) {
          bonus /= 2;
          passed = false; // allow backward pawn penalty
        }

        // reduce bonus if blocked
        if ((tmp = sqr + (color ? senjo::South : senjo::North)).IsValid() &&
            _board[tmp.Name()])
        {
          bonus /= 2;
          passed = false; // allow backward pawn penalty
        }

        score += bonus;
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
  int KnightEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Knight), sqr.Name());

    // keep the knight close to the action
    // assuming the action is centered around the kings
    score += (8 - (sqr.DistanceTo(king[White]) + sqr.DistanceTo(king[Black])));

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the bishop on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  int BishopEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Bishop), sqr.Name());

    // stay close to fiendly king during endgame
    score += static_cast<int>(EndGame(color) * (8 - sqr.DistanceTo(king[color])));

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
  int RookEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Rook), sqr.Name());

    // stay close to fiendly king during endgame
    score += static_cast<int>(EndGame(color) * (8 - sqr.DistanceTo(king[color])));

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
  int QueenEval(const senjo::Square& sqr) {
    int score = SquareValue((color|Queen), sqr.Name());

    // the queen is almost an entirely tactical piece.  There are some positional
    // rules of thumb that could be applied tor the queen, but we'll just leave
    // it up to the piece square table

    return score;
  }

  //--------------------------------------------------------------------------
  //! Calculate the positional value of the king on the given square
  //--------------------------------------------------------------------------
  template<Color color>
  int KingEval(const senjo::Square& sqr) {
    assert(sqr == king[color]);
    senjo::Square tmp;
    int score = SquareValue((color|King), sqr.Name());
    int val;

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
    val = 0;
    switch (sqr.X()) {
    case 0: case 1: case 2:
      val += (openFile[!color][0] + openFile[!color][1] + openFile[!color][2] +
              openFile[ color][0] + openFile[ color][1] + openFile[ color][2]);
      break;
    case 3: case 4:
      val += (openFile[!color][2] + openFile[!color][3] +
              openFile[!color][4] + openFile[!color][5] +
              openFile[ color][2] + openFile[ color][3] +
              openFile[ color][4] + openFile[ color][5]);
      break;
    case 5: case 6: case 7:
      val += (openFile[!color][5] + openFile[!color][6] + openFile[!color][7] +
              openFile[ color][5] + openFile[ color][6] + openFile[ color][7]);
      break;
    default:
      assert(false);
    }
    if (val) {
      score -= static_cast<int>(MidGame(color) * 4 * val);
    }

    if (!(state & (color ? BlackCastleMask : WhiteCastleMask))) {
      // bonus for pawn shield, penalty for pawn storm
      val = 0;
      switch (sqr.Name()) {
      case (color ? senjo::Square::A8 : senjo::Square::A1):
      case (color ? senjo::Square::B8 : senjo::Square::B1):
      case (color ? senjo::Square::C8 : senjo::Square::C1):
        switch (_board[color ? senjo::Square::A7 : senjo::Square::A2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::B7 : senjo::Square::B2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::C7 : senjo::Square::C2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::A6 : senjo::Square::A3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::B6 : senjo::Square::B3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::C6 : senjo::Square::C3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        break;
      case (color ? senjo::Square::A7 : senjo::Square::A2):
      case (color ? senjo::Square::B7 : senjo::Square::B2):
      case (color ? senjo::Square::C7 : senjo::Square::C2):
        switch (_board[color ? senjo::Square::A6 : senjo::Square::A3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::B6 : senjo::Square::B3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::C6 : senjo::Square::C3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::A5 : senjo::Square::A4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::B5 : senjo::Square::B4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::C5 : senjo::Square::C4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        break;
      case (color ? senjo::Square::F8 : senjo::Square::F1):
      case (color ? senjo::Square::G8 : senjo::Square::G1):
      case (color ? senjo::Square::H8 : senjo::Square::H1):
        switch (_board[color ? senjo::Square::F7 : senjo::Square::F2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::G7 : senjo::Square::G2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::H7 : senjo::Square::H2]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::F6 : senjo::Square::F3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::G6 : senjo::Square::G3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::H6 : senjo::Square::H3]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        break;
      case (color ? senjo::Square::F7 : senjo::Square::F2):
      case (color ? senjo::Square::G7 : senjo::Square::G2):
      case (color ? senjo::Square::H7 : senjo::Square::H2):
        switch (_board[color ? senjo::Square::F6 : senjo::Square::F3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::G6 : senjo::Square::G3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::H6 : senjo::Square::H3]) {
          case (color|Pawn):    val += 8; break;
          case ((!color)|Pawn): val -= 8; break;
        }
        switch (_board[color ? senjo::Square::F5 : senjo::Square::F4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::G5 : senjo::Square::G4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        switch (_board[color ? senjo::Square::H5 : senjo::Square::H4]) {
          case (color|Pawn):    val += 4; break;
          case ((!color)|Pawn): val -= 4; break;
        }
        break;
      }
      if (val) {
        score += static_cast<int>(MidGame(color) * val);
      }
    }
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
  int Evaluate() {
    int typeCount[14] = {0};
    int pieceStack[32];
    int stackCount = 0;
    int pc;

    eval = (material[White] - material[Black]);
    pieceCount = 0;
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
        pieceStack[stackCount++] = sqr.Name();
        if (COLOR_OF(pc) == ColorToMove()) {
          pieceCount++;
        }
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
      typeCount[pc]++;
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

    // draw?
    bool whiteCanWin = (typeCount[White|Pawn] ||
                       (typeCount[White|Knight] > 1) ||
                       (typeCount[White|Bishop] > 1) ||
                        typeCount[White|Rook] ||
                        typeCount[White|Queen] ||
                       (typeCount[White|Knight] && typeCount[White|Bishop]));

    bool blackCanWin = (typeCount[Black|Pawn] ||
                       (typeCount[Black|Knight] > 1) ||
                       (typeCount[Black|Bishop] > 1) ||
                        typeCount[Black|Rook] ||
                        typeCount[Black|Queen] ||
                       (typeCount[Black|Knight] && typeCount[Black|Bishop]));

    if (!whiteCanWin && !blackCanWin || (rcount >= 100)) {
      state |= Draw;
      return (eval = 0);
    }

    // reduce winning score if "winning" side can't win
    if ((eval > 0) ? !whiteCanWin : !blackCanWin) {
      eval = std::max<int>(50, (eval / 4));
    }

    // return coarse-grain score so we don't flip flop line selection
    // based on insignificant eval differences
    return ((eval >> 3) << 3);
  }

  //--------------------------------------------------------------------------
  //! Execute a null move against the position at this node,
  //! the resulting position is applied to the given 'dest' node.
  //--------------------------------------------------------------------------
  template<Color color>
  void ExecNullMove(ClubFoot& dest) const {
    assert(ColorToMove() == color);
    assert(!AttackedBy<!color>(king[color]));
    assert(&dest != this);

    _nullMoves++;

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
  }

  //--------------------------------------------------------------------------
  //! Execute the given move against the position at this node,
  //! the resulting position is applied to the given 'dest' node.
  //--------------------------------------------------------------------------
  template<Color color>
  void Exec(const Move& move, ClubFoot& dest) const {
    assert(ColorToMove() == color);
    assert(&dest != this);

    _execs++;
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
          _HASH[color|Rook][color ? senjo::Square::A8 : senjo::Square::C1] ^
          _HASH[color|Rook][color ? senjo::Square::C8 : senjo::Square::C1]);
      break;
    }
    dest.checkState = CheckState::Unknown;
    dest.positionKey = (dest.pieceKey ^
                        _HASH[0][dest.state & FiveBits] ^
                        _HASH[0][dest.ep.Name()]);
  }

  //--------------------------------------------------------------------------
  //! Undo the last move executed at this node
  //! \param move Must be the last move executed on this node
  //--------------------------------------------------------------------------
  template<Color color>
  void Undo(const Move& move) const {
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
                        << moves[moveIndex].Score();
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

    pvCount = 0;
    _qnodes++;
    if (ply > _seldepth) {
      _seldepth = ply;
    }

    const bool check = KingInCheck<color>();
    const int standPat = color ? -Evaluate() : Evaluate();

    // mate distance pruning and standPat cutoff when not in check
    int best = (check ? (ply - Infinity) : standPat);
    if ((best >= beta) || (state & Draw) || !child) {
      return best;
    }
    if (best > alpha) {
      alpha = best;
    }

    // draw by repetition?
    if (_seen.count(positionKey)) {
      state |= Draw;
      return (eval = 0);
    }

    // TODO delta pruning

    // do we have anything for this position in the transposition table?
    Move firstMove;
    HashEntry* entry = _tt.Probe(positionKey);
    if (entry) {
      switch (entry->flags) {
      case HashEntry::Checkmate: return (ply - Infinity);
      case HashEntry::Stalemate: return 0;
      case HashEntry::UpperBound:
        firstMove.Init(entry->moveBits, entry->score);
        if (entry->score <= alpha) {
          pv[0] = firstMove;
          pvCount = 1;
          return alpha;
        }
        break;
      case HashEntry::ExactScore:
        firstMove.Init(entry->moveBits, entry->score);
        pv[0] = firstMove;
        pvCount = 1;
        if (entry->score <= alpha) {
          return alpha;
        }
        if (entry->score >= beta) {
          if (!firstMove.IsCapOrPromo()) {
            AddKiller(firstMove);
          }
          return beta;
        }
        return entry->score;
      case HashEntry::LowerBound:
        firstMove.Init(entry->moveBits, entry->score);
        if (entry->score >= beta) {
          pv[0] = firstMove;
          pvCount = 1;
          if (firstMove.IsCapOrPromo()) {
            AddKiller(firstMove);
          }
          return beta;
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
      Exec<color>(firstMove, *child);
      firstMove.Score() = -child->QSearch<!color>(-beta, -alpha, (depth - 1));
      Undo<color>(firstMove);
      if (_stop) {
        return beta;
      }
      if (firstMove.Score() > best) {
        UpdatePV(firstMove);
        if (firstMove.Score() >= beta) {
          if (!firstMove.IsCapOrPromo()) {
            AddKiller(firstMove);
          }
          if (check) {
            _tt.Store(positionKey, firstMove, 0, HashEntry::LowerBound);
          }
          return firstMove.Score();
        }
        if (firstMove.Score() > alpha) {
          alpha = firstMove.Score();
          if (check) {
            _tt.Store(positionKey, firstMove, 0, HashEntry::ExactScore);
          }
        }
        best = firstMove.Score();
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
      // don't call tt.StoreStalemate()!!!
      return standPat;
    }

    // search 'em
    Move* move;
    while ((move = GetNextMove())) {
      if (firstMove == (*move)) {
        assert(firstMove.IsValid());
        continue;
      }
      Exec<color>(*move, *child);
      move->Score() = -child->QSearch<!color>(-beta, -alpha, (depth - 1));
      Undo<color>(*move);
      if (_stop) {
        return beta;
      }
      if (move->Score() > best) {
        UpdatePV(*move);
        if (move->Score() >= beta) {
          if (!move->IsCapOrPromo()) {
            AddKiller(*move);
          }
          if (check) {
            _tt.Store(positionKey, *move, 0, HashEntry::LowerBound);
          }
          return move->Score();
        }
        if (move->Score() > alpha) {
          alpha = move->Score();
          if (check) {
            _tt.Store(positionKey, *move, 0, HashEntry::ExactScore);
          }
        }
        best = move->Score();
      }
    }

    assert(!check || (pvCount > 0));
    assert(best <= alpha);
    assert(alpha < beta);

    if (check && (alpha == orig_alpha)) {
      assert(pv[0].Score() <= alpha);
      _tt.Store(positionKey, pv[0], 0, HashEntry::UpperBound);
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

    const bool pvNode = ((alpha + 1) < beta);
    eval = -Infinity;
    extended = 0;
    reduced = 0;
    moveCount = 0;
    pvCount = 0;

    // mate distance pruning
    int best = (ply - Infinity);
    if ((best >= beta) || !child) {
      return best;
    }
    if (best > alpha) {
      alpha = best;
    }

    // draw by repetition?
    if (_seen.count(positionKey)) {
      state |= Draw;
      return (eval = 0);
    }

    // extend depth if in check and previous ply not extended
    const bool check = KingInCheck<color>();
    if (_ext && check && !parent->extended) {
      extended = 1;
      depth++;
    }

    // do we have anything for this position in the transposition table?
    Move firstMove;
    HashEntry* entry = _tt.Probe(positionKey);
    if (entry) {
      switch (entry->flags) {
      case HashEntry::Checkmate: return (ply - Infinity);
      case HashEntry::Stalemate: return 0;
      case HashEntry::UpperBound:
        firstMove.Init(entry->moveBits, entry->score);
        if (!pvNode && (entry->depth >= depth)) {
          if (entry->score <= alpha) {
            pv[0] = firstMove;
            pvCount = 1;
            return alpha;
          }
        }
        break;
      case HashEntry::ExactScore:
        firstMove.Init(entry->moveBits, entry->score);
        if (!pvNode && (entry->depth >= depth)) {
          pv[0] = firstMove;
          pvCount = 1;
          if (entry->score <= alpha) {
            return alpha;
          }
          if (entry->score >= beta) {
            if (firstMove.IsCapOrPromo()) {
              IncHistory(firstMove, check, entry->depth);
              AddKiller(firstMove);
            }
            return beta;
          }
          return entry->score;
        }
        break;
      case HashEntry::LowerBound:
        firstMove.Init(entry->moveBits, entry->score);
        if (!pvNode && (entry->depth >= depth)) {
          if (entry->score >= beta) {
            pv[0] = firstMove;
            pvCount = 1;
            if (firstMove.IsCapOrPromo()) {
              IncHistory(firstMove, check, entry->depth);
              AddKiller(firstMove);
            }
            return beta;
          }
        }
        break;
      default:
        assert(false);
      }
    }

    // internal iterative deepening if no firstMove in transposition table
    int iid = Infinity;
    if (_iid && !check && !firstMove.IsValid() && (beta < Infinity) &&
        (depth > (pvNode ? 3 : 5)))
    {
      assert(!pvCount);
      const int saved = nullMoveOk;
      nullMoveOk = 0;
      iid = Search<color>((beta - 1), beta, (depth - (pvNode ? 2 : 4)), true);
      nullMoveOk = saved;
      if (_stop || !pvCount) {
        return iid;
      }
      if (!pvNode && (iid >= beta) &&
          ((eval >= beta) || ((color ? -Evaluate() : Evaluate()) >= beta)))
      {
        return beta;
      }
      assert(pv[0].IsValid());
      firstMove = pv[0];
    }

    // null move pruning
    // if we can get a score >= beta without even making a move, return beta
    if (_nmp && nullMoveOk && (depth > 1) &&
        !pvNode &&          // never do forward pruning on pvNodes
        !check &&           // can't pass when in check
        (pieceCount > 1) && // don't pass when stalemates are likely
        (iid >= beta) &&    // don't pass if IID is < beta
        ((eval >= beta) || ((color ? -Evaluate() : Evaluate()) >= beta)))
    {
      ExecNullMove<color>(*child);
      child->nullMoveOk = 0;
      const int rdepth = std::max<int>(0, (depth - 3));
      int nmScore = (rdepth > 0)
          ? -child->Search<!color>(-beta, (1 - beta), rdepth, false)
          : -child->QSearch<!color>(-beta, (1 - beta), 0);
      if (_stop) {
        return beta;
      }
      if (nmScore >= beta) {
        pvCount = 0;
        _nmCutoffs++;
        return beta;
      }
    }
    child->nullMoveOk = 1;

    // make sure firstMove is populated
    const int orig_alpha = alpha;
    if (!firstMove.IsValid()) {
      GenerateMoves<color, false>(depth);
      if (moveCount <= 0) {
        if (check) {
          _tt.StoreCheckmate(positionKey);
          return (ply - Infinity);
        }
        _tt.StoreStalemate(positionKey);
        return 0;
      }
      firstMove = *GetNextMove();
    }

    // search first move with full alpha/beta window
    Exec<color>(firstMove, *child);
    firstMove.Score() = (depth > 1)
        ? -child->Search<!color>(-beta, -alpha, (depth - 1), !cutNode)
        : -child->QSearch<!color>(-beta, -alpha, 0);
    Undo<color>(firstMove);
    if (_stop) {
      return beta;
    }
    if (firstMove.Score() > best) {
      UpdatePV(firstMove);
      if (firstMove.Score() >= beta) {
        if (!firstMove.IsCapOrPromo()) {
          IncHistory(firstMove, check, depth);
          AddKiller(firstMove);
        }
        _tt.Store(positionKey, firstMove, depth, HashEntry::LowerBound);
        return firstMove.Score();
      }
      if (firstMove.Score() > alpha) {
        alpha = firstMove.Score();
        if (!firstMove.IsCapOrPromo()) {
          IncHistory(firstMove, check, depth);
        }
        _tt.Store(positionKey, firstMove, depth, HashEntry::ExactScore);
      }
      best = firstMove.Score();
    }
    else if (!firstMove.IsCapOrPromo()) {
      DecHistory(firstMove, check);
    }

    // generate moves if we haven't done so already
    if (moveCount <= 0) {
      GenerateMoves<color, false>(depth);
      assert(moveCount > 0);
    }
    moveIndex = 0;

    // is it ok to do late move reductions at this node?
    bool lmr_ok = (_lmr && !check && (depth > 2));
    int pvDepth = depth;
    int newDepth;

    // search remaining moves
    Move* move;
    while ((move = GetNextMove())) {
      if (firstMove == (*move)) {
        assert(firstMove.IsValid());
        continue;
      }

      Exec<color>(*move, *child);
      newDepth = (depth - 1);
      reduced = 0;

      // late move reductions
      if (lmr_ok &&
          !move->IsCapOrPromo() &&
          !IsKiller(*move) &&
          (_hist[move->GetHistoryIndex()] < 0) &&
          !child->KingInCheck<!color>())
      {
        reduced += (1 + (!pvNode && cutNode));
        newDepth -= reduced;
      }

      // first search with a null window to quickly see if it improves alpha
      move->Score() = (newDepth > 0)
          ? -child->Search<!color>(-(alpha + 1), -alpha, newDepth, true)
          : -child->QSearch<!color>(-(alpha + 1), -alpha, 0);

      // re-search at full depth?
      if (!_stop && reduced && (move->Score() > alpha)) {
        assert(depth > 1);
        reduced = 0;
        move->Score() =
            -child->Search<!color>(-(alpha + 1), -alpha, (depth - 1), true);
      }

      // re-search with full window?
      if (!_stop && (move->Score() > alpha) && (move->Score() < beta)) {
        assert(!reduced);
        move->Score() = (depth > 1)
            ? -child->Search<!color>(-beta, -alpha, (depth - 1), false)
            : -child->QSearch<!color>(-beta, -alpha, 0);
      }

      Undo<color>(*move);
      if (_stop) {
        return beta;
      }
      if (move->Score() > best) {
        UpdatePV(*move);
        pvDepth = newDepth = (depth - reduced);
        if (move->Score() >= beta) {
          if (!move->IsCapOrPromo()) {
            IncHistory(*move, check, newDepth);
            AddKiller(*move);
          }
          _tt.Store(positionKey, *move, newDepth, HashEntry::LowerBound);
          return move->Score();
        }
        if (move->Score() > alpha) {
          alpha = move->Score();
          if (!move->IsCapOrPromo()) {
            IncHistory(*move, check, newDepth);
          }
          _tt.Store(positionKey, *move, newDepth, HashEntry::ExactScore);
        }
        best = move->Score();
      }
      else if (!move->IsCapOrPromo()) {
        DecHistory(*move, check);
      }
    }

    assert(pvCount > 0);
    assert(best <= alpha);
    assert(alpha < beta);

    if (alpha == orig_alpha) {
      assert(pv[0].Score() <= alpha);
      pvDepth = (depth - reduced);
      _tt.Store(positionKey, pv[0], pvDepth, HashEntry::UpperBound);
    }

    return best;
  }

  //--------------------------------------------------------------------------
  //! \brief Update transposition table with current move
  //--------------------------------------------------------------------------
  template<Color color>
  void UpdateTT(const Move* move, const int depth, const int score) {
    if (move && (depth > 0)) {
      assert(move->Score() == score);
      _tt.Store(positionKey, *move, depth, HashEntry::ExactScore);
      if (child && (depth > 1)) {
        Exec<color>(*move, *child);
        child->UpdateTT<!color>((move + 1), (depth - 1), -score);
        Undo<color>(*move);
      }
    }
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

    // move transposition table move (if any) to front of list
    HashEntry* entry = _tt.Probe(positionKey);
    if (entry) {
      switch (entry->flags) {
      case HashEntry::Checkmate:
      case HashEntry::Stalemate:
        assert(false);
        break;
      case HashEntry::UpperBound:
      case HashEntry::ExactScore:
      case HashEntry::LowerBound: {
        Move ttMove(entry->moveBits, entry->score);
        for (int i = 0; i < moveCount; ++i) {
          if (moves[i] == ttMove) {
            if (i > 0) {
              moves[i] = moves[0];
              moves[0] = ttMove;
            }
            break;
          }
        }
        break;
      }}
    }

    // initial principal variation
    pvCount = 1;
    pv[0] = moves[0];

    // return immediately if we only have one move
    if (moveCount == 1) {
      OutputPV(pv[0].Score());
      return pv[0].ToString();
    }

    Move* move;
    bool  showPV = true;
    int   alpha;
    int   best = (color ? -Evaluate() : Evaluate());
    int   beta;
    int   delta;

    // iterative deepening
    for (int d = 0; !_stop && (d < depth); ++d) {
      _seldepth = _depth = (d + 1);
      _movenum  = 0;
      child->nullMoveOk = (d > 0);

      showPV = true;
      delta  = 25;
      alpha  = std::max<int>((best - delta), -Infinity);
      beta   = std::min<int>((best + delta), +Infinity);

      // prime transposition table with PV from previous iteration
      if (d > 0) {
        UpdateTT<color>(pv, pvCount, pv[0].Score());
      }

      moveIndex = 0;
      while ((move = GetNextMove())) {
        _currmove = move->ToString();
        _movenum++;

        // aspiration window search
        Exec<color>(*move, *child);
        while (true) {
          move->Score() = (_depth > 1)
              ? -child->Search<!color>(-beta, -alpha, (_depth - 1), false)
              : -child->QSearch<!color>(-beta, -alpha, 0);

          // expand aspiration window and re-search?
          if (!_stop && ((move->Score() >= beta) ||
                         ((move->Score() <= alpha) && (_movenum == 1))))
          {
            delta *= 4;
            if (move->Score() >= beta) {
              assert(move->Score() < Infinity);
              beta = std::min<int>((move->Score() + delta), Infinity);
              if ((Now() - _startTime) > 1000) {
                OutputPV(move->Score(), 1); // report lowerbound
              }
            }
            else {
              assert(move->Score() > -Infinity);
              alpha = std::max<int>((move->Score() - delta), -Infinity);
              if ((Now() - _startTime) > 1000) {
                OutputPV(move->Score(), -1); // report upperbound
              }
            }
            continue;
          }
          delta = 50;
          break;
        }
        Undo<color>(*move);

        // do we have a new principal variation?
        if (!_stop && ((_movenum == 1) || (move->Score() > best))) {
          UpdatePV(*move);
          OutputPV(move->Score());
          showPV = false;

          // set null aspiration window now that we have a principal variation
          best = alpha = move->Score();
          beta = (alpha + 1);

          // scoot this move to the front of the list
          for (int i = (_movenum - 1); i > 0; --i) {
            const Move tmp = moves[i - 1];
            moves[i - 1] = moves[i];
            moves[i] = tmp;
          }
        }
      }
    }

    if (showPV) {
      OutputPV(pv[0].Score());
    }

    return pv[0].ToString();
  }

  //--------------------------------------------------------------------------
  //! Initialize search variables
  //--------------------------------------------------------------------------
  void InitSearch() {
    _tt.ResetCounters();
    _currmove.clear();
    _depth      = 0;
    _movenum    = 0;
    _seldepth   = 0;
    _execs      = 0;
    _qnodes     = 0;
    _nullMoves  = 0;
    _nmCutoffs  = 0;
  }
};


} // namespace clubfoot

#endif // CLUBFOOT_H
