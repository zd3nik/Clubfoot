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

#include "senjo/src/Output.h"
#include "HashTable.h"
#include "ClubFoot.h"

using namespace senjo;

namespace clubfoot
{

//----------------------------------------------------------------------------
// for convenience
//----------------------------------------------------------------------------
static const std::string _TRUE = "true";

//----------------------------------------------------------------------------
const int ClubFoot::_KING_SQR[128] = {
    //------- MiddleGame --------     ---------- EndGame ----------
    0, 12, 12, -8, -8, -8, 12,  0,  -50,-24,-12, -8, -8,-12,-24,-50,
   -8, -8,-12,-12,-12,-12, -8, -8,  -24,-12, -8,  0,  0, -8,-12,-24,
   -8,-12,-16,-16,-16,-16,-12, -8,  -12, -8,  0,  8,  8,  0, -8,-12,
  -12,-16,-24,-24,-24,-24,-16,-12,   -8,  0,  8, 12, 12,  8,  0, -8,
  -12,-16,-24,-24,-24,-24,-16,-12,   -8,  0,  8, 12, 12,  8,  0, -8,
   -8,-12,-16,-16,-16,-16,-12, -8,  -12, -8,  0,  8,  8,  0, -8,-12,
   -8, -8,-12,-12,-12,-12, -8, -8,  -24,-12, -8,  0,  0, -8,-12,-24,
    0, 12, 12, -8, -8, -8, 12,  0,  -50,-24,-12, -8, -8,-12,-24,-50
};

//----------------------------------------------------------------------------
const int ClubFoot::_PIECE_SQR[12][128] = {
  { // White
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // Black
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // (White|Pawn)
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
   -8,  0,  0,-12,-12,  0,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  0,  0,  0,  0,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  8, 12, 12,  8,  0, -8,  0,0,0,0,0,0,0,0,
    0, 10, 12, 16, 16, 12, 10,  0,  0,0,0,0,0,0,0,0,
   12, 24, 24, 24, 24, 24, 24, 12,  0,0,0,0,0,0,0,0,
   28, 32, 32, 32, 32, 32, 32, 28,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // (Black|Pawn)
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
   28, 32, 32, 32, 32, 32, 32, 28,  0,0,0,0,0,0,0,0,
   12, 24, 24, 24, 24, 24, 24, 12,  0,0,0,0,0,0,0,0,
    0, 10, 12, 16, 16, 12, 10,  0,  0,0,0,0,0,0,0,0,
   -8,  0,  8, 12, 12,  8,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  0,  0,  0,  0,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  0,-12,-12,  0,  0, -8,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // (White|Knight)
  -24,-12, -8, -8, -8, -8,-12,-24,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
   -8,  0,  4,  4,  4,  4,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  8,  8,  8,  8,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0, 12, 12, 12, 12,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0, 16, 16, 16, 16,  0, -8,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
  -24,-12, -8, -8, -8, -8,-12,-24,  0,0,0,0,0,0,0,0,
  },
  { // (Black|Knight)
  -24,-12, -8, -8, -8, -8,-12,-24,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
   -8,  0, 16, 16, 16, 16,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0, 12, 12, 12, 12,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  8,  8,  8,  8,  0, -8,  0,0,0,0,0,0,0,0,
   -8,  0,  4,  4,  4,  4,  0, -8,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
  -24,-12, -8, -8, -8, -8,-12,-24,  0,0,0,0,0,0,0,0
  },
  { // (White|Bishop)
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
   -8,  8,  0,  0,  0,  0,  8, -8,  0,0,0,0,0,0,0,0,
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, 12, 12, 12, 12,  0,  0,  0,0,0,0,0,0,0,0,
    0, 12, 16, 16, 16, 16, 12,  0,  0,0,0,0,0,0,0,0,
   -8,  0,  0,  0,  0,  0,  0, -8,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0
  },
  { // (Black|Bishop)
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0,
   -8,  0,  0,  0,  0,  0,  0, -8,  0,0,0,0,0,0,0,0,
    0, 12, 16, 16, 16, 16, 12,  0,  0,0,0,0,0,0,0,0,
    0,  0, 12, 12, 12, 12,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0,
   -8,  8,  0,  0,  0,  0,  8, -8,  0,0,0,0,0,0,0,0,
  -12, -8,  0,  0,  0,  0, -8,-12,  0,0,0,0,0,0,0,0
  },
  { // (White|Rook)
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -4, -4, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -8, -8, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -8, -8, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -4, -4, -4,  0,  0,  0,0,0,0,0,0,0,0,
    8,  8,  8,  8,  8,  8,  8,  8,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // (Black|Rook)
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    8,  8,  8,  8,  8,  8,  8,  8,  0,0,0,0,0,0,0,0,
    0,  0, -4, -4, -4, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -8, -8, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -8, -8, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0, -4, -4, -4, -4,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0,
    0,  0,  8,  8,  8,  8,  0,  0,  0,0,0,0,0,0,0,0
  },
  { // (White|Queen)
  -50,-12, -8, -8, -8, -8,-12,-50,  0,0,0,0,0,0,0,0,
  -12,-12,  0,  0,  0,  0,-12,-12,  0,0,0,0,0,0,0,0,
  -12,  0,  4,  4,  4,  4,  0,-12,  0,0,0,0,0,0,0,0,
  -12,  0,  8,  8,  8,  8,  0,-12,  0,0,0,0,0,0,0,0,
  -12,  0, 12, 12, 12, 12,  0,-12,  0,0,0,0,0,0,0,0,
  -12, 12, 16, 16, 16, 16, 12,-12,  0,0,0,0,0,0,0,0,
  -12,  0, 12, 12, 12, 12,  0,-12,  0,0,0,0,0,0,0,0,
  -50,-12,  0,  0,  0,  0,-12,-50,  0,0,0,0,0,0,0,0
  },
  { // (Black|Queen)
  -50,-12,  0,  0,  0,  0,-12,-50,  0,0,0,0,0,0,0,0,
  -12,  0, 12, 12, 12, 12,  0,-12,  0,0,0,0,0,0,0,0,
  -12, 12, 16, 16, 16, 16, 12,-12,  0,0,0,0,0,0,0,0,
  -12,  0, 12, 12, 12, 12,  0,-12,  0,0,0,0,0,0,0,0,
  -12,  0,  8,  8,  8,  8,  0,-12,  0,0,0,0,0,0,0,0,
  -12,  0,  4,  4,  4,  4,  0,-12,  0,0,0,0,0,0,0,0,
  -12,-12,  0,  0,  0,  0,-12,-12,  0,0,0,0,0,0,0,0,
  -50,-12, -8, -8, -8, -8,-12,-50,  0,0,0,0,0,0,0,0
  }
};

//----------------------------------------------------------------------------
// static ClubFoot class variables
//----------------------------------------------------------------------------
bool                ClubFoot::_ext = false;
bool                ClubFoot::_iid = false;
bool                ClubFoot::_initialized = false;
bool                ClubFoot::_nmp = false;
bool                ClubFoot::_oneReply = false;
char                ClubFoot::_hist[0x100000] = {0};
int                 ClubFoot::_board[128] = {0};
int                 ClubFoot::_contempt = 0;
int                 ClubFoot::_delta = 0;
int                 ClubFoot::_depth = 0;
int                 ClubFoot::_drawScore[2] = {0};
int                 ClubFoot::_lmr = 0;
int                 ClubFoot::_movenum = 0;
int                 ClubFoot::_rzr = 0;
int                 ClubFoot::_seldepth = 0;
int                 ClubFoot::_tempo = 0;
int                 ClubFoot::_test = 0;
std::string         ClubFoot::_currmove;
int64_t             ClubFoot::_hashSize = 0;
ClubFoot            ClubFoot::_node[MaxPlies];
std::set<uint64_t>  ClubFoot::_seen;
Stats               ClubFoot::_stats;
Stats               ClubFoot::_totalStats;
TranspositionTable  ClubFoot::_tt;

EngineOption ClubFoot::_optHash("Hash", "1024", EngineOption::Spin, 0, 4096);
EngineOption ClubFoot::_optClearHash("Clear Hash", "", EngineOption::Button);
EngineOption ClubFoot::_optContempt("Contempt", "0", EngineOption::Spin, 0, 50);
EngineOption ClubFoot::_optDelta("Delta Pruning Margin", "500", EngineOption::Spin, 0, 9999);
EngineOption ClubFoot::_optEXT("Check Extensions", _TRUE, EngineOption::Checkbox);
EngineOption ClubFoot::_optIID("Internal Iterative Deepening", _TRUE, EngineOption::Checkbox);
EngineOption ClubFoot::_optLMR("Late Move Reduction", "1", EngineOption::Spin, 0, 3);
EngineOption ClubFoot::_optNMP("Null Move Pruning", _TRUE, EngineOption::Checkbox);
EngineOption ClubFoot::_optOneReply("One Reply Extensions", _TRUE, EngineOption::Checkbox);
EngineOption ClubFoot::_optRZR("Razoring Delta", "500", EngineOption::Spin, 0, 9999);
EngineOption ClubFoot::_optTempo("Tempo Bonus", "0", EngineOption::Spin, 0, 50);
EngineOption ClubFoot::_optTest("Experimental Feature", "0", EngineOption::Spin, 0, 9999);

//----------------------------------------------------------------------------
std::string ClubFoot::GetEngineName() const
{
  return (sizeof(void*) == 8) ? "Clubfoot" : "Clubfoot (32-bit)";
}

//----------------------------------------------------------------------------
std::string ClubFoot::GetEngineVersion() const
{
  std::string rev = MAKE_XSTR(GIT_REV);
  if (rev.size() > 7) {
    rev = rev.substr(0, 7);
  }
  return ("1.0." + rev);
}

//----------------------------------------------------------------------------
std::string ClubFoot::GetAuthorName() const
{
  return "Shawn Chidester";
}

//----------------------------------------------------------------------------
std::string ClubFoot::GetCountryName() const
{
  return "USA";
}

//----------------------------------------------------------------------------
std::list<EngineOption> ClubFoot::GetOptions() const
{
  std::list<EngineOption> opts;
  opts.push_back(_optHash);
  opts.push_back(_optClearHash);
  opts.push_back(_optContempt);
  opts.push_back(_optDelta);
  opts.push_back(_optEXT);
  opts.push_back(_optIID);
  opts.push_back(_optLMR);
  opts.push_back(_optNMP);
  opts.push_back(_optOneReply);
  opts.push_back(_optRZR);
  opts.push_back(_optTempo);
  opts.push_back(_optTest);
  return opts;
}

//----------------------------------------------------------------------------
bool ClubFoot::SetEngineOption(const std::string& optionName,
                               const std::string& optionValue)
{
  if (!stricmp(optionName.c_str(), _optHash.GetName().c_str())) {
    if (_optHash.SetValue(optionValue)) {
      SetHashSize(_optHash.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optClearHash.GetName().c_str())) {
    ClearHash();
    return true;
  }
  if (!stricmp(optionName.c_str(), _optContempt.GetName().c_str())) {
    if (_optContempt.SetValue(optionValue)) {
      _contempt = static_cast<int>(_optContempt.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optEXT.GetName().c_str())) {
    if (_optEXT.SetValue(optionValue)) {
      _ext = (_optEXT.GetValue() == _TRUE);
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optDelta.GetName().c_str())) {
    if (_optDelta.SetValue(optionValue)) {
      _delta = static_cast<int>(_optDelta.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optIID.GetName().c_str())) {
    if (_optIID.SetValue(optionValue)) {
      _iid = (_optIID.GetValue() == _TRUE);
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optLMR.GetName().c_str())) {
    if (_optLMR.SetValue(optionValue)) {
      _lmr = static_cast<int>(_optLMR.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName .c_str(), _optNMP.GetName().c_str())) {
    if (_optNMP.SetValue(optionValue)) {
      _nmp = (_optNMP.GetValue() == _TRUE);
      return true;
    }
  }
  if (!stricmp(optionName .c_str(), _optOneReply.GetName().c_str())) {
    if (_optOneReply.SetValue(optionValue)) {
      _oneReply = (_optOneReply.GetValue() == _TRUE);
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optRZR.GetName().c_str())) {
    if (_optRZR.SetValue(optionValue)) {
      _rzr = static_cast<int>(_optRZR.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optTempo.GetName().c_str())) {
    if (_optTempo.SetValue(optionValue)) {
      _tempo = static_cast<int>(_optTempo.GetIntValue());
      return true;
    }
  }
  if (!stricmp(optionName.c_str(), _optTest.GetName().c_str())) {
    if (_optTest.SetValue(optionValue)) {
      _test = static_cast<int>(_optTest.GetIntValue());
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
void ClubFoot::Initialize()
{
  ply = 0;
  child = _node;
  parent = NULL;
  for (int i = 0; i < MaxPlies; ++i) {
    _node[i].ply = (i + 1);
    _node[i].child = ((i + 1) < MaxPlies) ? &(_node[i + 1]) : NULL;
    _node[i].parent = (i > 0) ? &(_node[i - 1]) : this;
  }

  _hashSize = _optHash.GetIntValue();
  _contempt = static_cast<int>(_optContempt.GetIntValue());
  _delta    = static_cast<int>(_optDelta.GetIntValue());
  _lmr      = static_cast<int>(_optLMR.GetIntValue());
  _rzr      = static_cast<int>(_optRZR.GetIntValue());
  _tempo    = static_cast<int>(_optTempo.GetIntValue());
  _test     = static_cast<int>(_optTest.GetIntValue());
  _ext      = (_optEXT.GetValue() == _TRUE);
  _iid      = (_optIID.GetValue() == _TRUE);
  _nmp      = (_optNMP.GetValue() == _TRUE);
  _oneReply = (_optOneReply.GetValue() == _TRUE);

  ClearHistory();
  SetHashSize(_hashSize);
  SetPosition(_STARTPOS);

  _initialized = true;
}

//----------------------------------------------------------------------------
bool ClubFoot::IsInitialized() const
{
  return _initialized;
}

//----------------------------------------------------------------------------
const char* ClubFoot::SetPosition(const char* fen)
{
  if (!fen || !*fen) {
    Output() << "NULL or empty fen string";
    return NULL;
  }

  int tmpBoard[128];
  int kingPosition[2] = { -1, -1 };
  int materialTotal[2] = { 0, 0 };
  int moveCount = 0;
  int reversibleCount = 0;
  int boardState = 0;
  Square epSquare;
  uint64_t pcKey = 0;

  memset(tmpBoard, 0, sizeof(tmpBoard));

  const char* p = fen;
  for (int y = 7; y >= 0; --y, ++p) {
    for (int x = 0; x < 8; ++x, ++p) {
      switch (*p) {
      case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8':
        x += (*p - '1');
        break;
      case 'B':
        tmpBoard[SQR(x,y)] = (White|Bishop);
        materialTotal[White] += BishopValue;
        pcKey ^= _HASH[White|Bishop][SQR(x,y)];
        break;
      case 'K':
        tmpBoard[SQR(x,y)] = (White|King);
        kingPosition[White] = SQR(x,y);
        pcKey ^= _HASH[White|King][SQR(x,y)];
        break;
      case 'N':
        tmpBoard[SQR(x,y)] = (White|Knight);
        materialTotal[White] += KnightValue;
        pcKey ^= _HASH[White|Knight][SQR(x,y)];
        break;
      case 'P':
        tmpBoard[SQR(x,y)] = (White|Pawn);
        materialTotal[White] += PawnValue;
        pcKey ^= _HASH[White|Pawn][SQR(x,y)];
        break;
      case 'Q':
        tmpBoard[SQR(x,y)] = (White|Queen);
        materialTotal[White] += QueenValue;
        pcKey ^= _HASH[White|Queen][SQR(x,y)];
        break;
      case 'R':
        tmpBoard[SQR(x,y)] = (White|Rook);
        materialTotal[White] += RookValue;
        pcKey ^= _HASH[White|Rook][SQR(x,y)];
        break;
      case 'b':
        tmpBoard[SQR(x,y)] = (Black|Bishop);
        materialTotal[Black] += BishopValue;
        pcKey ^= _HASH[Black|Bishop][SQR(x,y)];
        break;
      case 'k':
        tmpBoard[SQR(x,y)] = (Black|King);
        kingPosition[Black] = SQR(x,y);
        pcKey ^= _HASH[Black|King][SQR(x,y)];
        break;
      case 'n':
        tmpBoard[SQR(x,y)] = (Black|Knight);
        materialTotal[Black] += KnightValue;
        pcKey ^= _HASH[Black|Knight][SQR(x,y)];
        break;
      case 'p':
        tmpBoard[SQR(x,y)] = (Black|Pawn);
        materialTotal[Black] += PawnValue;
        pcKey ^= _HASH[Black|Pawn][SQR(x,y)];
        break;
      case 'q':
        tmpBoard[SQR(x,y)] = (Black|Queen);
        materialTotal[Black] += QueenValue;
        pcKey ^= _HASH[Black|Queen][SQR(x,y)];
        break;
      case 'r':
        tmpBoard[SQR(x,y)] = (Black|Rook);
        materialTotal[Black] += RookValue;
        pcKey ^= _HASH[Black|Rook][SQR(x,y)];
        break;
      default:
        Output() << "Invalid character at " << p;
        return NULL;
      }
    }
    if ((y > 0) && (*p != '/')) {
      Output() << "Invalid character at " << p;
      return NULL;
    }
  }
  if (kingPosition[White] < 0) {
    Output() << "No white king in " << fen;
    return NULL;
  }
  if (kingPosition[Black] < 0) {
    Output() << "No black king in " << fen;
    return NULL;
  }

  NextWord(p);
  switch (*p++) {
  case 'b': boardState |= Black; break;
  case 'w': boardState |= White; break;
  default:
    Output() << "Expected 'w' or 'b' at " << p;
    return NULL;
  }
  if (*p && !isspace(*p++)) {
    Output() << "Invalid character at " << p;
    return NULL;
  }

  NextWord(p);
  while (*p && !isspace(*p)) {
    switch (*p++) {
    case '-': break;
    case 'K': boardState |= WhiteShort; continue;
    case 'Q': boardState |= WhiteLong;  continue;
    case 'k': boardState |= BlackShort; continue;
    case 'q': boardState |= BlackLong;  continue;
    default:
      Output() << "Unexpected castle rights at " << p;
      return NULL;
    }
    break;
  }
  if (*p && !isspace(*p++)) {
    Output() << "Invalid character at " << p;
    return NULL;
  }

  NextWord(p);
  if (IS_X(p[0]) && IS_Y(p[1])) {
    const int x = TO_X(*p++);
    const int y = TO_Y(*p++);
    epSquare.Assign(x, y);
    if (y != ((boardState & Black) ? 2 : 5)) {
      Output() << "Invalid en passant square: " << epSquare.ToString();
      return NULL;
    }
  }
  else if (*p == '-') {
    p++;
  }
  if (*p && !isspace(*p)) {
    Output() << "Invalid character at " << p;
    return NULL;
  }

  NextWord(p);
  if (isdigit(*p)) {
    while (*p && isdigit(*p)) {
      reversibleCount = ((reversibleCount * 10) + (*p++ - '0'));
    }
  }
  else if (*p == '-') {
    p++;
  }

  NextWord(p);
  if (isdigit(*p)) {
    while (*p && isdigit(*p)) {
      moveCount = ((moveCount * 10) + (*p++ - '0'));
    }
  }

  _seen.clear();
  memcpy(_board, tmpBoard, sizeof(_board));
  memcpy(king, kingPosition, sizeof(king));
  memcpy(material, materialTotal, sizeof(material));

  mcount      = ((moveCount * 2) + (state & Black));
  rcount      = reversibleCount;
  state       = boardState;
  ep          = epSquare;
  checkState  = CheckState::Unknown;
  pieceKey    = pcKey;
  positionKey = (pcKey ^ _HASH[0][state & FiveBits] ^ _HASH[0][ep.Name()]);

  if (WhiteToMove()
      ? AttackedBy<White>(king[Black])
      : AttackedBy<Black>(king[White]))
  {
    Output() << "Side to move can take enemy king!";
    return NULL;
  }

  Evaluate();

  return p;
}

//----------------------------------------------------------------------------
const char* ClubFoot::MakeMove(const char* str)
{
  if (!str ||
      !IS_X(str[0]) || !IS_Y(str[1]) ||
      !IS_X(str[2]) || !IS_Y(str[3]))
  {
    return NULL;
  }

  int from  = SQR(TO_X(str[0]), TO_Y(str[1]));
  int to    = SQR(TO_X(str[2]), TO_Y(str[3]));
  int pc    = _board[from];
  int cap   = _board[to];
  int promo = 0;

  const char* p = (str + 4);
  switch (*p) {
  case 'b': promo = (ColorToMove()|Bishop); p++; break;
  case 'n': promo = (ColorToMove()|Knight); p++; break;
  case 'q': promo = (ColorToMove()|Queen);  p++; break;
  case 'r': promo = (ColorToMove()|Rook);   p++; break;
  default:
    break;
  }

  if ((*p && !isspace(*p)) ||
      !pc ||
      (from == to) ||
      (COLOR_OF(pc) != ColorToMove()) ||
      (cap && (COLOR_OF(cap) == ColorToMove())) ||
      ((Black|cap) == (Black|King)) ||
      ((Black|promo) == (Black|Pawn)) ||
      ((Black|promo) == (Black|King)) ||
      (promo && ((Black|pc) != (Black|Pawn))))
  {
    return NULL;
  }

  if (WhiteToMove()) {
    GenerateMoves<White, false>(1);
  }
  else {
    GenerateMoves<Black, false>(1);
  }

  for (; moveIndex < moveCount; ++moveIndex) {
    const Move& move = moves[moveIndex];
    if ((move.GetFromName() == from) &&
        (move.GetToName() == to) &&
        (move.GetPromo() == promo))
    {
      break;
    }
  }
  if (moveIndex >= moveCount) {
    return NULL;
  }

  if (WhiteToMove()) {
    Exec<White>(moves[moveIndex], *this);
  }
  else {
    Exec<Black>(moves[moveIndex], *this);
  }

  return p;
}

//----------------------------------------------------------------------------
std::string ClubFoot::GetFEN() const
{
  char fen[256];
  char* p = fen;
  int empty = 0;
  int type;

  // piece positions
  for (int y = 7; y >= 0; --y) {
    for (int x = 0; x < 8; ++x) {
      if ((type = _board[SQR(x,y)]) && empty) {
        *p++ = ('0' + empty);
        empty = 0;
      }
      switch (type) {
      case (White|Pawn):   *p++ = 'P'; break;
      case (White|Knight): *p++ = 'N'; break;
      case (White|Bishop): *p++ = 'B'; break;
      case (White|Rook):   *p++ = 'R'; break;
      case (White|Queen):  *p++ = 'Q'; break;
      case (White|King):   *p++ = 'K'; break;
      case (Black|Pawn):   *p++ = 'p'; break;
      case (Black|Knight): *p++ = 'n'; break;
      case (Black|Bishop): *p++ = 'b'; break;
      case (Black|Rook):   *p++ = 'r'; break;
      case (Black|Queen):  *p++ = 'q'; break;
      case (Black|King):   *p++ = 'k'; break;
      default:
        empty++;
      }
    }
    if (empty) {
      *p++ = ('0' + empty);
      empty = 0;
    }
    if (y > 0) {
      *p++ = '/';
    }
  }

  // color to move
  *p++ = ' ';
  *p++ = (WhiteToMove() ? 'w' : 'b');

  // castling
  *p++ = ' ';
  if (state & CastleMask) {
    if (state & WhiteShort) *p++ = 'K';
    if (state & WhiteLong)  *p++ = 'Q';
    if (state & BlackShort) *p++ = 'k';
    if (state & BlackLong)  *p++ = 'q';
  }
  else {
    *p++ = '-';
  }

  // en passant square
  *p++ = ' ';
  if (ep.IsValid()) {
    *p++ = ('a' + ep.X());
    *p++ = ('1' + ep.Y());
  }
  else {
    *p++ = '-';
  }

  // reversible half-move count and full move count
  snprintf(p, (sizeof(fen) - strlen(fen)), " %d %d",
           rcount, ((mcount + 1) / 2));
  return fen;
}

//----------------------------------------------------------------------------
void ClubFoot::PrintBoard() const
{
  Output out;
  out << '\n';

  for (int y = 7; y >= 0; --y) {
    for (int x = 0; x < 8; ++x) {
      switch (_board[SQR(x, y)]) {
      case (White|Pawn):   out << " P"; break;
      case (White|Knight): out << " N"; break;
      case (White|Bishop): out << " B"; break;
      case (White|Rook):   out << " R"; break;
      case (White|Queen):  out << " Q"; break;
      case (White|King):   out << " K"; break;
      case (Black|Pawn):   out << " p"; break;
      case (Black|Knight): out << " n"; break;
      case (Black|Bishop): out << " b"; break;
      case (Black|Rook):   out << " r"; break;
      case (Black|Queen):  out << " q"; break;
      case (Black|King):   out << " k"; break;
      default:
        out << (((x ^ y) & 1) ? " -" : "  ");
      }
    }
    switch (y) {
    case 7:
      out << (WhiteToMove() ? "  White to move" : "  Black to move");
      break;

    case 6:
      if (state & Draw) {
        out << "  DRAW";
      }
      break;

    case 5:
      out << "  Move Number       : " << ((mcount + 1) / 2);
      break;

    case 4:
      out << "  Reversible Moves  : " << rcount;
      break;

    case 3:
      out << "  Castling Rights   : ";
      if (state & WhiteShort) out << 'K';
      if (state & WhiteLong)  out << 'Q';
      if (state & BlackShort) out << 'k';
      if (state & BlackLong)  out << 'q';
      break;

    case 2:
      out << "  En Passant Square : " << ep.ToString();
      break;

    case 0:
      out << "  Static Evaluation : " << (ColorToMove() ? -standPat : standPat);
      break;
    }
    out << '\n';
  }

  out << '\n';
}

//----------------------------------------------------------------------------
bool ClubFoot::WhiteToMove() const
{
  return !(state & Black);
}

//----------------------------------------------------------------------------
void ClubFoot::ClearSearchData()
{
  ClearHash();
  ClearHistory();
  ClearKillers();
}

//----------------------------------------------------------------------------
void ClubFoot::PonderHit()
{
  // ponder not supported
}

//----------------------------------------------------------------------------
void ClubFoot::Quit() {
  // stop searching and exit the timer thread
  ChessEngine::Quit();

  // free up memory allocated for the transposition table
  SetHashSize(0);
}

//----------------------------------------------------------------------------
void ClubFoot::ResetStatsTotals() {
  _totalStats.Clear();
}

//----------------------------------------------------------------------------
void ClubFoot::ShowStatsTotals() const {
  Output() << "--- Averaged Stats";
  _totalStats.Average().Print();
}

//----------------------------------------------------------------------------
void ClubFoot::GetStats(int* depth,
                        int* seldepth,
                        uint64_t* nodes,
                        uint64_t* qnodes,
                        uint64_t* msecs,
                        int* movenum,
                        char* move,
                        const size_t movelen) const
{
  if (depth) {
    *depth = _depth;
  }
  if (seldepth) {
    *seldepth = _seldepth;
  }
  if (nodes) {
    *nodes = (_stats.snodes + _stats.qnodes);
  }
  if (qnodes) {
    *qnodes = _stats.qnodes;
  }
  if (msecs) {
    *msecs = (Now() - _startTime);
  }
  if (movenum) {
    *movenum = _movenum;
  }
  if (move && movelen) {
    snprintf(move, movelen, "%s", _currmove.c_str());
  }
}

//----------------------------------------------------------------------------
uint64_t ClubFoot::MyPerft(const int depth)
{
  if (!_initialized) {
    Output() << "Engine not initialized";
    return 0;
  }

  InitSearch();

  const int d = std::min<int>(depth, MaxPlies);
  const uint64_t count = WhiteToMove() ? PerftSearchRoot<White>(d)
                                       : PerftSearchRoot<Black>(d);

  const uint64_t msecs = (Now() - _startTime);
  Output() << "Perft " << count << ' ' << Rate((count / 1000), msecs)
           << " KLeafs/sec";

  return count;
}

//----------------------------------------------------------------------------
std::string ClubFoot::MyGo(const int depth,
                           const int /*movestogo*/,
                           const uint64_t /*movetime*/,
                           const uint64_t /*wtime*/, const uint64_t /*winc*/,
                           const uint64_t /*btime*/, const uint64_t /*binc*/,
                           std::string* /*ponder*/)
{
  if (!_initialized) {
    Output() << "Engine not initialized";
    return std::string();
  }

  InitSearch();

  int d = std::min<int>(depth, MaxPlies);
  if (d <= 0) {
    d = MaxPlies;
  }
  std::string bestmove = (WhiteToMove() ? SearchRoot<White>(d)
                                        : SearchRoot<Black>(d));

  _totalStats += _stats;
  if (_debug) {
    Output() << "--- Stats";
    Output() << _tt.GetStores() << " stores, " << _tt.GetHits() << " hits, "
             << _tt.GetCheckmates() << " checkmates, "
             << _tt.GetStalemates() << " stalemates";

    _stats.Print();
  }

  return bestmove;
}

} // namespace clubfoot
