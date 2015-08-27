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

#ifndef CLUBFOOT_STATS_H
#define CLUBFOOT_STATS_H

#include "Platform.h"

namespace clubfoot
{

struct Stats
{
  Stats() { Clear(); }

  void Clear();
  void Print();
  Stats Average() const;
  Stats& operator+=(const Stats& other);

  uint64_t snodes;        // Search() calls
  uint64_t qnodes;        // QSearch() calls
  uint64_t chkExts;       // check extensions
  uint64_t oneReplyExts;  // one reply extensions
  uint64_t hashExts;      // extensions from hash
  uint64_t execs;         // Exec() calls
  uint64_t qexecs;        // delta candidates
  uint64_t deltaCount;    // delta prunings
  uint64_t futility;      // futility prunings
  uint64_t rzrCount;      // razoring attempts
  uint64_t rzrEarlyOut;   // razoring early descent into qsearch
  uint64_t rzrCutoffs;    // successful razorings
  uint64_t iidCount;      // IID searches
  uint64_t nullMoves;     // ExecNullMove() calls
  uint64_t nmCutoffs;     // null moves cutoffs
  uint64_t nmrCandidates; // null move reduction attempts
  uint64_t nmReductions;  // null move reductions
  uint64_t nmrBackfires;  // null move reduction backfires
  uint64_t lateMoves;     // late move count
  uint64_t lmCandidates;  // lmr candidates
  uint64_t lmReductions;  // late move reductions
  uint64_t lmResearches;  // lmReductions re-searched at full depth
  uint64_t lmConfirmed;   // lmResearches alpha increases confirmed
  uint64_t lmAlphaIncs;   // late moves that increase alpha
  uint64_t statCount;     // number of stats summed into this instance
};

} // namespace clubfoot

#endif // STATS_H
