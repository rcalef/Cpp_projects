// $Id: numbers.cpp,v 1.1 2013-07-26 17:06:53-07 - - $

#include <cstdlib>

using namespace std;

#include "numbers.h"
#include "util.h"

ostream &operator<< (ostream &out, const degrees &that) {
   out << that._degree_value << "deg";
   return out;
}

ostream &operator<< (ostream &out, const points &that) {
   out << that._point_value << "pt";
   return out;
}

ostream &operator<< (ostream &out, const inches &that) {
   out << that._inch_value << "in";
   return out;
}

ostream &operator<< (ostream &out, const xycoords &coords) {
   out << "(" << coords.first << "," << coords.second << ")";
   return out;
}

