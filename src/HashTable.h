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

#ifndef CLUBFOOT_HASHTABLE_H
#define CLUBFOOT_HASHTABLE_H

#include "senjo/src/Platform.h"
#include "Types.h"
#include "Move.h"

namespace clubfoot
{

//----------------------------------------------------------------------------
//! Piece/Square hash codes
//----------------------------------------------------------------------------
extern const uint64_t _HASH[14][128];

//----------------------------------------------------------------------------
//! \brief 64-bit transposition table entry
//! NOTE: positionKey could be reduced to 32 bit (only store 32 high order
//! bits of position Key for collision detection), freeing up 32 bits for
//! other data.
//----------------------------------------------------------------------------
struct HashEntry
{
public:
  enum Flag {
    Checkmate   = 0x01,
    Stalemate   = 0x02,
    UpperBound  = 0x04,
    ExactScore  = 0x08,
    LowerBound  = 0x10
  };

  uint64_t positionKey;
  uint32_t moveBits;
  int16_t  score;
  uint8_t  depth;
  uint8_t  flags;
};

//----------------------------------------------------------------------------
//! \brief The transposition table
//! This implementation assumes the L1 cache line size is 64 bits, which is
//! only large enough to hold one entry.  Some engines assume a 128 bit cache
//! entry size and store multiple entries (also usually 64 bit) per table
//! slot.  It would be a relatively trivial matter to make this configurable,
//! but for the sake of simplicity this implementation will stick to one 64-bit
//! entry per table slot.
//----------------------------------------------------------------------------
class TranspositionTable
{
public:
  //--------------------------------------------------------------------------
  //! Constructor
  //--------------------------------------------------------------------------
  TranspositionTable() : keyMask(0), entries(NULL) { }

  //--------------------------------------------------------------------------
  //! Destructor
  //--------------------------------------------------------------------------
  ~TranspositionTable() {
    delete[] entries;
    entries = NULL;
    keyMask = 0;
  }

  //--------------------------------------------------------------------------
  //! Resize the table, this also clears the contents of the table
  //! \param mbytes The maximum number of megabytes the table will hold
  //! \return false if the requested size could not be allocated
  //--------------------------------------------------------------------------
  bool Resize(const size_t mbytes) {
    delete[] entries;
    entries = NULL;
    keyMask = 0;

    if (!mbytes) {
      return true;
    }

    // convert mbytes to bytes
    const size_t bytes = (mbytes * 1024 * 1024);

    // how many hash entries can we fit into the requeste number of bytes?
    const size_t count = (bytes / sizeof(HashEntry));

    // get high bit of 'count + 1'
    // for example, if 'count + 1' in binary is: 100110101
    //                    the high bit would be: 100000000
    // NOTE: there are faster ways to do this on modern processors
    size_t highBit = 1;
    for (size_t tmp = ((count + 1) >> 1); tmp; tmp >>= 1) {
      highBit <<= 1;
    }

    // if highBit is 0 we've shifted beyond size_t bit count (e.g. too big!)
    if (!highBit) {
      return false;
    }

    // highBit is the number of entries we'll store
    // highBit - 1 is the bit mask we use to map position keys to a table slot
    // example highBit in binary: 100000000
    //                      mask: 011111111
    keyMask = (highBit - 1);
    if (!keyMask) {
      return false;
    }

    // allocate it
    if (!(entries = new HashEntry[keyMask + 1])) {
      return false;
    }

    // initialize it
    Clear();
    return true;
  }

  //--------------------------------------------------------------------------
  //! Clear contents of the table
  //--------------------------------------------------------------------------
  void Clear() {
    ResetCounters();
    if (entries) {
      memset(entries, 0, (sizeof(HashEntry) * (keyMask + 1)));
    }
  }

  //--------------------------------------------------------------------------
  //! Get the hash entry the given position key maps to
  //! \param key The position key
  //! \return NULL if no entry exists for the given \p key
  //--------------------------------------------------------------------------
  HashEntry* Probe(const uint64_t key) {
    if (key && entries) {
      HashEntry* entry = (entries + (key & keyMask));
      if (entry->positionKey == key) {
        _hits++;
        return entry;
      }
    }
    return NULL;
  }

  //--------------------------------------------------------------------------
  //! Store the best move for a given position key in the table
  //! \param key The position key
  //! \param bestmove The best move for the given position key
  //! \param depth The search depth used to obtain bestmove
  //! \param flag The HashEntry::Flag to assign to this entry
  //--------------------------------------------------------------------------
  void Store(const uint64_t key,
             const Move& bestmove,
             const int depth,
             const int flag)
  {
    assert(bestmove.IsValid());
    assert(abs(bestmove.GetScore()) < Infinity);
    assert((depth >= 0) && (depth < 256));
    assert((flag == HashEntry::LowerBound) ||
           (flag == HashEntry::UpperBound) ||
           (flag == HashEntry::ExactScore));

    if (key && entries) {
      _stores++;
      HashEntry* entry   = (entries + (key & keyMask));
      entry->positionKey = key;
      entry->moveBits    = bestmove.GetBits();
      entry->score       = static_cast<int16_t>(bestmove.GetScore());
      entry->depth       = static_cast<uint8_t>(depth);
      entry->flags       = static_cast<uint8_t>(flag);
    }
  }

  //--------------------------------------------------------------------------
  //! Store a checkmate position
  //! \param key The position key
  //--------------------------------------------------------------------------
  void StoreCheckmate(const uint64_t key) {
    if (key && entries) {
      _checkmates++;
      HashEntry* entry   = (entries + (key & keyMask));
      entry->positionKey = key;
      entry->moveBits    = 0;
      entry->score       = Infinity;
      entry->depth       = 0;
      entry->flags       = HashEntry::Checkmate;
    }
  }

  //--------------------------------------------------------------------------
  //! Store a stalemate position
  //! \param key The position key
  //--------------------------------------------------------------------------
  void StoreStalemate(const uint64_t key) {
    if (key && entries) {
      _stalemates++;
      HashEntry* entry   = (entries + (key & keyMask));
      entry->positionKey = key;
      entry->moveBits    = 0;
      entry->score       = 0;
      entry->depth       = 0;
      entry->flags       = HashEntry::Stalemate;
    }
  }

  //--------------------------------------------------------------------------
  //! Set all transposition table counters to zero
  //--------------------------------------------------------------------------
  void ResetCounters() {
    _stores = 0;
    _hits = 0;
    _checkmates = 0;
    _stalemates = 0;
  }

  //--------------------------------------------------------------------------
  //! Get number of position+bestmove entries stored since last reset
  //! \return The number of position+bestmove entries stored since last reset
  //--------------------------------------------------------------------------
  uint64_t GetStores() const {
    return _stores;
  }

  //--------------------------------------------------------------------------
  //! Get number of successful probes since last reset
  //! \return The number of successful probes since last reset
  //--------------------------------------------------------------------------
  uint64_t GetHits() const {
    return _hits;
  }

  //--------------------------------------------------------------------------
  //! Get the number of checkmates stored
  //! \return The number of checkmates stored
  //--------------------------------------------------------------------------
  uint64_t GetCheckmates() const {
    return _checkmates;
  }

  //--------------------------------------------------------------------------
  //! Get the number of stalemates stored
  //! \return The number of stalemates stored
  //--------------------------------------------------------------------------
  uint64_t GetStalemates() const {
    return _stalemates;
  }

private:
  static uint64_t _stores;
  static uint64_t _hits;
  static uint64_t _checkmates;
  static uint64_t _stalemates;

  size_t     keyMask;
  HashEntry* entries;
};

} // namespace clubfoot

#endif // HASHTABLE_H
