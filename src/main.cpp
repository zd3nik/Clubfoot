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

#include "senjo/src/UCIAdapter.h"
#include "senjo/src/Output.h"
#include "ClubFoot.h"

using namespace clubfoot;

int main(int /*argc*/, char** /*argv*/)
{
  ClubFoot engine;
  senjo::UCIAdapter adapter;

  if (!adapter.Start(engine)) {
    senjo::Output() << "Unable to start UCIAdapter";
    return 1;
  }

  char sbuf[16384];
  memset(sbuf, 0, sizeof(sbuf));

  while (fgets(sbuf, sizeof(sbuf), stdin)) {
    char* cmd = sbuf;
    senjo::NormalizeString(cmd);
    if (!adapter.DoCommand(cmd)) {
      break;
    }
  }

  return 0;
}
