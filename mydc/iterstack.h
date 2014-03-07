// $Id: iterstack.h,v 1.8 2013-01-23 19:47:39-08 - - $

// 
// The class std::stack does not provide an iterator, which is
// needed for this class.  So, like std::stack, class iterstack
// is implemented on top of a container.
// 
// We use private inheritance because we want to restrict
// operations only to those few that are approved.  All functions
// are merely inherited from the container, with only ones needed
// being exported as public.
// 
// No implementation file is needed because all functions are
// inherited, and the convenience functions that are added are
// trivial, and so can be inline.
//
// Any underlying container which supports the necessary operations
// could be used, such as vector, list, or deque.
// 

#ifndef __ITERSTACK_H__
#define __ITERSTACK_H__

#include <vector>

using namespace std;

template <typename value_type>
class iterstack: private vector<value_type> {
   public:
      using vector<value_type>::const_reference;
      using vector<value_type>::const_reverse_iterator;
      using vector<value_type>::push_back;
      using vector<value_type>::pop_back;
      using vector<value_type>::clear;
      using vector<value_type>::back;
      using vector<value_type>::size;
      using vector<value_type>::empty;
      using vector<value_type>::rbegin;
      using vector<value_type>::rend;
      //using vector<value_type>::crbegin;
     // using vector<value_type>::crend;
      inline void push (const value_type &value) { push_back (value); }
      inline void pop() { pop_back(); }
      inline const value_type &top() const { return back(); }
};

#endif

