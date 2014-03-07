// $Id: numbers.h,v 1.6 2013-07-23 19:08:23-07 - - $

#ifndef __NUMBERS_H__
#define __NUMBERS_H__

#include <iostream>
#include <utility>

using namespace std;

#include "trace.h"

const double PTS_PER_INCH = 72;

class degrees {
   friend ostream &operator<< (ostream &, const degrees &);
   private:
      double _degree_value;
   public:
      inline degrees(): _degree_value(0) {}
      inline explicit degrees (double init): _degree_value(init) {}
      inline double degree_value() const { return _degree_value; }
};

class points {
   friend ostream &operator<< (ostream &, const points &);
   private:
      double _point_value;
   public:
      inline points(): _point_value(0) {}
      inline explicit points (double init): _point_value(init) {}
      inline double point_value() const { return _point_value; }
};

class inches {
   friend ostream &operator<< (ostream &, const inches &);
   private:
      double _inch_value;
   public:
      inline inches(): _inch_value (0) {}
      inline explicit inches (double init): _inch_value(init) {}
      inline inches operator-() const { return inches (-_inch_value); }
      inline double inch_value() const { return _inch_value; }
      inline double point_value() const {
         return _inch_value * PTS_PER_INCH;
      }
};

typedef pair <inches, inches> xycoords;

ostream &operator<< (ostream &, const xycoords &);

#endif

