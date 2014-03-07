// $Id: trace.cpp,v 1.1 2013-07-26 17:12:34-07 - - $

#include <climits>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

#include "trace.h"

typedef vector<char> boolvec;
boolvec traceflags::flags (UCHAR_MAX + 1, false);
const boolvec trueflags (UCHAR_MAX + 1, true);

void traceflags::setflags (const string &optflags) {
   for (string::const_iterator itor = optflags.begin();
        itor != optflags.end(); ++itor) {
      if (*itor == '@') {
         flags = trueflags;
      }else {
         flags[*itor] = true;
      }
   }
   // Note that TRACE can trace setflags.
   TRACE ('t',  "optflags = " << optflags);
}

//
// getflag -
//    Check to see if a certain flag is on.
//

bool traceflags::getflag (char flag) {
   // Bug alert:
   // Don't TRACE this function or the stack will blow up.
   bool result = flags[flag];
   return result;
}

