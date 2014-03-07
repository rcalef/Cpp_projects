// $Id: bigint.cpp,v 1.1 2013-07-22 12:27:00-07 - - $
//CMPS 109 Summer 2013 Assignemnt 2
//Submitted by: Robert Calef (rcalef@ucsc.edu)

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>

using namespace std;

#include "bigint.h"
#include "trace.h"

#define CDTOR_TRACE TRACE ('~', this << " -> " << big_value)

bigint::bigint (): negative(false), big_value() {
}

bigint::bigint (const bigint &that): negative (that.negative),
   big_value() {
   bigvalue_t::const_iterator itor = that.big_value.begin();
   bigvalue_t::const_iterator end = that.big_value.end();
   for(;itor != end; ++itor){
      this->big_value.push_back(*itor);
   }
}

bigint &bigint::operator= (const bigint &that) {
   if (this == &that) return *this;
   this->negative = that.negative;
   big_value.clear();
   bigvalue_t::const_iterator itor = that.big_value.begin();
   bigvalue_t::const_iterator end = that.big_value.end();
   for(;itor != end; ++itor){
       this->big_value.push_back(*itor);
   }
   return *this;
}

bigint::~bigint() {
   big_value.clear();
}

bigint::bigint (long that): negative (that < 0 ? true : false),
      big_value(){
   do {
       int digit = that % 10;
       big_value.push_back(digit);
       that /= 10;
   } while (that > 0);
   this->trim_zeros();
}

bigint::bigint (const string &that): big_value() {
   negative = false;
   if (*that.begin() == '_') negative = true;
   string::const_reverse_iterator itor = that.rbegin();
   string::const_reverse_iterator end = that.rend();
   //Stop before underscore if negative
   if(negative) --end;
   for (; itor != end; ++itor) big_value.push_back ((*itor) - '0');
   this->trim_zeros();
}

void bigint::trim_zeros(){
   bigvalue_t::const_reverse_iterator itor = big_value.rbegin();
   bigvalue_t::const_reverse_iterator end = big_value.rend();
   for(; itor != end; ++itor){
      if(*itor == 0) big_value.pop_back();
      else break;
   }
   if(big_value.empty()) negative = false;
}

int bigint::abs_compare (const bigint &that) const{
   if (big_value.size() < that.big_value.size()) return -1;
   if (that.big_value.size() < big_value.size()) return 1;
   //Sizes are equal, check each digit starting at high order
   int itor = big_value.size() - 1; //Access last element
   for(; itor >= 0; --itor){
       if (big_value[itor] < that.big_value[itor]) return -1;
       if (that.big_value[itor] < big_value[itor]) return 1;
   }
   return 0;
}



bigint bigint::do_add (const bigint &that) const{
   unsigned int this_size = this->big_value.size();
   unsigned int that_size = that.big_value.size();
   unsigned int max = this_size < that_size ? that_size : this_size;
   int carry = 0;
   bigint sum;
   for(unsigned int itor = 0; itor < max; ++itor){
      int left = 0;
      int right = 0;
      if (itor < this_size) left = (big_value[itor]);
      if (itor < that_size) right = (that.big_value[itor]);
      int result = left + right + carry;
      carry = result / 10;
      sum.big_value.push_back(result % 10);
   }
   if (carry != 0) sum.big_value.push_back(carry);
   return sum;
}

bigint bigint::do_sub (const bigint &that) const{
   //By nature of bigint::op-(binary), do_sub is always
   //called with left (this) larger than right (that)
   int borrow = 0;
   bigint diff;
   for(unsigned int itor = 0; itor < big_value.size(); ++itor){
      int left = big_value[itor] - borrow;
      int right = 0;
      if (itor < that.big_value.size()) right = that.big_value[itor];
      //If left < right, need to "borrow" from next digit.
      if(left < right){
         left += 10;
         borrow = 1;
      }else borrow = 0;
      int result = left - right;
      diff.big_value.push_back(result);
   }
   return diff;
}

bigint bigint::do_bigmul (const bigint &that) const{
   bigint product;
   unsigned int left_max = big_value.size();
   unsigned int right_max = that.big_value.size();
   product.big_value.assign((left_max + right_max), 0);
   for(unsigned int i = 0; i < left_max; ++i){
      int carry = 0;
      for(unsigned int j = 0; j < right_max; ++j){
         int result = product.big_value[i+j]
                       + (big_value[i] * that.big_value[j]) + carry;
         product.big_value[i+j] = result % 10;
         carry = result / 10;
      }
      //Add in any carry leftover from final evaluation
      product.big_value[i + right_max] = carry;
   }
   return product;
}

bigint bigint::operator+ (const bigint &that) const {
   bigint sum;
   if(negative == that.negative){
      sum = this->do_add(that);
      sum.negative = negative;
   }else{
      int cmp = this->abs_compare(that);
      if (cmp >= 0){
         sum = this->do_sub(that);
         sum.negative = negative;
      }else {
         sum = that.do_sub(*this);
         sum.negative = that.negative;
      }
   }
   sum.trim_zeros();
   return sum;
}

bigint bigint::operator- (const bigint &that) const {
   bigint diff;
   if(negative != that.negative){
      diff = this->do_add(that);
      diff.negative = negative;
   }else{
      int cmp = this->abs_compare(that);
      if(cmp >= 0){
         diff = this->do_sub(that);
         diff.negative = negative;
      }else{
         diff = that.do_sub(*this);
         diff.negative = !negative;
      }
   }
   diff.trim_zeros();
   return diff;
}

bigint bigint::operator- () const {
   bigint neg = *this;
   neg.negative = !negative;
   return neg;
}

long bigint::to_long () const {
   long output = 0;
   if(big_value.empty()) return output;
   long placeVal = 1;
   for(unsigned int itor = 0; itor < big_value.size(); ++itor){
      output += big_value[itor] * (placeVal);
      placeVal *= 10;
   }
   if(negative) output *= -1;
   if (output <= numeric_limits<long>::min()
    || output > numeric_limits<long>::max())
       throw range_error ("to_long: out of range");
   return output;
}

bool abs_less (const bigint &left, const bigint &right) {
   return (left.abs_compare(right) == -1);
}

//
// Multiplication algorithm.
//
bigint bigint::operator* (const bigint &that) const {
   bigint product = this->do_bigmul(that);
   if(negative == that.negative) product.negative = false;
   else product.negative = true;
   product.trim_zeros();
   return product;
}

//
// Division algorithm.
//

void mul_by_2 (bigint &bigval) {
   if(bigval.big_value.empty()) return;
   int carry = 0;
   for(unsigned int itor = 0; itor < bigval.big_value.size(); ++itor){
      int result = (bigval.big_value[itor] * 2) + carry;
      bigval.big_value[itor] = result % 10;
      carry = result / 10;
   }
   if (carry != 0) bigval.big_value.push_back(carry);
}

void div_by_2 (bigint &bigval) {
   if(bigval.big_value.empty()) return;
   for(unsigned int itor = 0; itor < bigval.big_value.size(); ++itor){
      int result = bigval.big_value[itor] / 2;
      if(itor < bigval.big_value.size() - 1
            && bigval.big_value[itor + 1] % 2 == 1) result += 5;
      bigval.big_value[itor] = result;
   }
   bigval.trim_zeros();
}


bigint bigint::divide (const bigint &that, const div_rem_op op) const {
   if (that == 0) throw domain_error ("divide by 0");
   typedef bigint unumber;
   static unumber zero = 0;
   if (that == 0) throw domain_error ("unumber::divide");
   unumber divisor = that;
   unumber quotient = 0; 
   unumber remainder = *this;
   unumber power_of_2 = 1;
   while (abs_less (divisor, remainder)) {
      mul_by_2 (divisor);
      mul_by_2 (power_of_2);
   }
   while (abs_less (zero, power_of_2)) {
      if (! abs_less (remainder, divisor)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      div_by_2 (divisor);
      div_by_2 (power_of_2);
   }
   switch (op) {
      case QUOTIENT: return quotient;
      case REMAINDER: return remainder;
   }
}

bigint bigint::operator/ (const bigint &that) const {
   bigint value = divide (that, QUOTIENT);
   if(negative != that.negative) value.negative = true;
   return value;
}

bigint bigint::operator% (const bigint &that) const {
   bigint value = divide (that, REMAINDER);
   if(negative != that.negative) value.negative = true;
   return value;
}

bool bigint::operator== (const bigint &that) const {
   if(negative != that.negative
         || big_value.size() != that.big_value.size()) return false;
   for (unsigned int itor = 0; itor < big_value.size(); ++itor){
      if(big_value[itor] != that.big_value[itor]) return false;
   }
   return true;
}

bool bigint::operator< (const bigint &that) const {
   if(negative && !that.negative) return true;
   if(!negative && that.negative) return false;
   if(negative){
      if(big_value.size() > that.big_value.size()) return true;
      else if (big_value.size() < that.big_value.size()) return false;
   }else{
      if(big_value.size() > that.big_value.size()) return false;
      else if (big_value.size() < that.big_value.size()) return true;
   }
   //At this point, both values are negative or both values are
   //positive, and have the same size, so compare by digit.
   for(long itor = big_value.size() - 1; itor >= 0; --itor){
      if(negative){
         if(big_value[itor] < that.big_value[itor]) return false;
         if(big_value[itor] > that.big_value[itor]) return true;
      }else{
         if(big_value[itor] > that.big_value[itor]) return false;
         if(big_value[itor] < that.big_value[itor]) return true;
      }
   }
   //If we reach this point, both values are equal.
   return false;
}

ostream &operator<< (ostream &out, const bigint &that) {
   if(that.big_value.empty()) out << 0;
   if(that.negative) out << '-';
   bigvalue_t::const_reverse_iterator itor = that.big_value.rbegin();
   bigvalue_t::const_reverse_iterator end = that.big_value.rend();
   //charCount keeps track of how many digits have been printed per
   //line, terminates the line with a '\' after printing 70th digit
   int charCount = 1;
   for(; itor != end; ++itor){
       out << int(*itor);
       if(charCount % 69 == 0) out << "\\" << endl;
       ++charCount;
   }
   return out;
}


bigint pow (const bigint &base, const bigint &exponent) {
   TRACE ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
   TRACE ('^', "result = " << result);
   return result;
}
