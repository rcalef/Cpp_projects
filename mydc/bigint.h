// $Id: bigint.h,v 1.5 2013-07-05 20:19:45-07 - - $
//CMPS 109 Summer 2013, Assignment 2
//Submitted by: Robert Calef (rcalef@ucsc.edu)

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>

using namespace std;

#include "trace.h"

typedef unsigned char digit_t;
typedef vector<digit_t> bigvalue_t;

//
// Define class bigint
//
class bigint {
      friend ostream &operator<< (ostream &, const bigint &);
      friend void mul_by_2 (bigint &);
      friend void div_by_2 (bigint &);
      friend bool abs_less (const bigint &, const bigint &);
   private:
      bool negative;
      bigvalue_t big_value;
      enum div_rem_op {QUOTIENT, REMAINDER};
      void trim_zeros();
      bigint divide (const bigint &, const div_rem_op) const;
      bigint do_add (const bigint &) const;
      bigint do_sub (const bigint &) const;
      bigint do_bigmul (const bigint &) const;
      int abs_compare (const bigint &) const;
   public:
      //
      // Override implicit members.
      //
      bigint ();
      bigint (const bigint &);
      bigint &operator= (const bigint &);
      ~bigint ();
      //
      // Extra ctors to make bigints.
      //
      bigint (const long);
      bigint (const string &);
      //
      // Basic add/sub operators.
      //
      bigint operator+ (const bigint &) const;
      bigint operator- (const bigint &) const;
      bigint operator- () const;
      long to_long () const;
      //
      // Extended operators implemented with add/sub.
      //
      bigint operator* (const bigint &) const;
      bigint operator/ (const bigint &) const;
      bigint operator% (const bigint &) const;
      //
      // Comparison operators.
      //
      bool operator== (const bigint &) const;
      bool operator<  (const bigint &) const;
};


bigint pow (const bigint &base, const bigint &exponent);

inline bool operator!= (const bigint &left, const bigint &right) {
   return ! (left == right);
}

inline bool operator<= (const bigint &left, const bigint &right) {
   return ! (right < left);
}

inline bool operator>  (const bigint &left, const bigint &right) {
   return right < left;
}

inline bool operator>= (const bigint &left, const bigint &right) {
   return ! (left < right);
}

//
// Operators with a left side of long.
//

inline bigint operator+ (long left, const bigint &right) {
   return bigint (left) + right;
}

inline bigint operator- (long left, const bigint &right) {
   return bigint (left) - right;
}

inline bigint operator* (long left, const bigint &right) {
   return bigint (left) * right;
}

inline bigint operator/ (long left, const bigint &right) {
   return bigint (left) / right;
}

inline bigint operator% (long left, const bigint &right) {
   return bigint (left) % right;
}

inline bool operator== (long left, const bigint &right) {
   return bigint (left) == right;
}

inline bool operator< (long left, const bigint &right) {
   return bigint (left) < right;
}

#endif

