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
#include "Stats.h"

using namespace senjo;

namespace clubfoot
{

//----------------------------------------------------------------------------
void Stats::Clear()
{
  statCount     = 1;
  snodes        = 0;
  qnodes        = 0;
  chkExts       = 0;
  hashExts      = 0;
  oneReplyExts  = 0;
  execs         = 0;
  qexecs        = 0;
  deltaCount    = 0;
  rzrCount      = 0;
  rzrCutoffs    = 0;
  iidCount      = 0;
  iidBeta       = 0;
  nullMoves     = 0;
  nmCutoffs     = 0;
  lateMoves     = 0;
  lmCandidates  = 0;
  lmReductions  = 0;
  lmDoubleRed   = 0;
  lmResearches  = 0;
  lmConfirmed   = 0;
  lmAlphaIncs   = 0;
}

//----------------------------------------------------------------------------
Stats& Stats::operator+=(const Stats& other) {
  statCount     += 1;
  snodes        += other.snodes;
  qnodes        += other.qnodes;
  chkExts       += other.chkExts;
  hashExts      += other.hashExts;
  oneReplyExts  += other.oneReplyExts;
  execs         += other.execs;
  qexecs        += other.qexecs;
  deltaCount    += other.deltaCount;
  rzrCount      += other.rzrCount;
  rzrCutoffs    += other.rzrCutoffs;
  iidCount      += other.iidCount;
  iidBeta       += other.iidBeta;
  nullMoves     += other.nullMoves;
  nmCutoffs     += other.nmCutoffs;
  lateMoves     += other.lateMoves;
  lmCandidates  += other.lmCandidates;
  lmReductions  += other.lmReductions;
  lmDoubleRed   += other.lmDoubleRed;
  lmResearches  += other.lmResearches;
  lmConfirmed   += other.lmConfirmed;
  lmAlphaIncs   += other.lmAlphaIncs;
  return *this;
}

//----------------------------------------------------------------------------
static uint64_t Avg(const uint64_t total, const uint64_t count) {
  if (count) {
    return ((total + (count - 1)) / count);
  }
  return total;
}

//----------------------------------------------------------------------------
Stats Stats::Average() const {
  Stats avg;
  avg.snodes        = Avg(snodes,       statCount);
  avg.qnodes        = Avg(qnodes,       statCount);
  avg.chkExts       = Avg(chkExts,      statCount);
  avg.hashExts      = Avg(hashExts,     statCount);
  avg.oneReplyExts  = Avg(oneReplyExts, statCount);
  avg.execs         = Avg(execs,        statCount);
  avg.qexecs        = Avg(qexecs,       statCount);
  avg.deltaCount    = Avg(deltaCount,   statCount);
  avg.rzrCount      = Avg(rzrCount,     statCount);
  avg.rzrCutoffs    = Avg(rzrCutoffs,   statCount);
  avg.iidCount      = Avg(iidCount,     statCount);
  avg.iidBeta       = Avg(iidBeta,      statCount);
  avg.nullMoves     = Avg(nullMoves,    statCount);
  avg.nmCutoffs     = Avg(nmCutoffs,    statCount);
  avg.lateMoves     = Avg(lateMoves,    statCount);
  avg.lmCandidates  = Avg(lmCandidates, statCount);
  avg.lmReductions  = Avg(lmReductions, statCount);
  avg.lmDoubleRed   = Avg(lmDoubleRed,  statCount);
  avg.lmResearches  = Avg(lmResearches, statCount);
  avg.lmConfirmed   = Avg(lmConfirmed,  statCount);
  avg.lmAlphaIncs   = Avg(lmAlphaIncs,  statCount);
  return avg;
}

//----------------------------------------------------------------------------
void Stats::Print() {
  if (chkExts || oneReplyExts || hashExts) {
    Output() << chkExts << " check extensions, "
             << oneReplyExts << " one reply extensions, "
             << hashExts << " hashed extensions";
  }

  Output() << execs << " execs, "
           << qexecs << " qexecs (" << Percent(qexecs, execs) << "%)";

  const uint64_t searches = (snodes + qnodes);
  Output() << snodes << " searches (" << Percent(snodes, searches) << "%), "
           << qnodes << " qsearches (" << Percent(qnodes, searches) << "%)";

  if (deltaCount) {
    Output() << deltaCount << " delta pruned ("
             << Percent(deltaCount, qnodes) << "%)";
  }

  if (rzrCount) {
    Output() << rzrCount << " razor attempts, "
             << rzrCutoffs << " confirmed ("
             << Percent(rzrCutoffs, rzrCount) << "%)";
  }

  if (nullMoves) {
    Output() << nullMoves << " null moves, "
             << nmCutoffs << " cutoffs ("
             << Percent(nmCutoffs, nullMoves) << "%)";
  }

  if (iidCount) {
    Output() << iidCount << " IID searches, " << iidBeta << " failed high ("
             << Percent(iidBeta, iidCount) << "%)";
  }

  Output() << lateMoves << " late moves ("
           << Percent(lateMoves, execs) << "%), "
           << lmAlphaIncs << " increase alpha ("
           << Percent(lmAlphaIncs, lateMoves) << "%)";

  if (lmCandidates) {
    Output() << lmCandidates << " lmr candidates ("
             << Percent(lmCandidates, lateMoves) << "%), "
             << lmReductions << " reduced ("
             << Percent(lmReductions, lateMoves) << "%), "
             << lmDoubleRed << " double ("
             << Percent(lmDoubleRed, lmReductions) << "%)";
    Output() << lmResearches << " lmr alpha increases ("
             << Percent(lmResearches, lmReductions) << "%), "
             << lmConfirmed << " confirmed ("
             << Percent(lmConfirmed, lmResearches) << "%)";
  }
}

} // namespace clubfoot
