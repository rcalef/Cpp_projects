// $Id: main.cpp,v 1.1 2013-07-22 12:27:01-07 - - $
//CMPS 109 Summer 2013, Assignment 2
//Submitted by: Robert Calef (rcalef@ucsc.edu)

#include <deque>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

using namespace std;

#include "bigint.h"
#include "iterstack.h"
#include "util.h"
#include "scanner.h"
#include "trace.h"

typedef iterstack<bigint> bigint_stack;

void do_arith (bigint_stack &stack, const char oper) {
   if(stack.size() < 2) throw ydc_exn("ydc: stack empty");
   bigint right = stack.top(); \
   stack.pop(); \
   TRACE ('d', "right = " << right); \
   bigint left = stack.top(); \
   stack.pop(); \
   TRACE ('d', "left = " << left); \
   bigint result;
   switch (oper) {
      case '+': result = left + right; break;
      case '-': result = left - right; break;
      case '*': result = left * right; break;
      case '/': result = left / right; break;
      case '%': result = left % right; break;
      case '^': result = pow (left, right); break;
      default: throw invalid_argument (
                     string ("do_arith operator is ") + oper);
   }
   TRACE ('d', "result = " << result); \
   stack.push (result); \
}

void do_clear (bigint_stack &stack, const char) {
   TRACE ('d', "");
   stack.clear();
}

void do_dup (bigint_stack &stack, const char) {
   bigint top = stack.top();
   TRACE ('d', top);
   stack.push (top);
}


void do_printall (bigint_stack &stack, const char) {
   bigint_stack::const_reverse_iterator itor = stack.rbegin();
   const bigint_stack::const_reverse_iterator end = stack.rend();
   for (; itor != end; ++itor) cout << *itor << endl;
}

void do_print (bigint_stack &stack, const char) {
   cout << stack.top() << endl;
}

void do_debug (bigint_stack &stack, const char) {
   (void) stack; // SUPPRESS: warning: unused parameter 'stack'
   cout << "Y not implemented" << endl;
}

class ydc_quit: public exception {};
void do_quit (bigint_stack &, const char) {
   throw ydc_quit ();
}

typedef void (*function_t) (bigint_stack &, const char);
typedef map <string, function_t> fn_map;
fn_map do_functions = {
   {"+", do_arith},
   {"-", do_arith},
   {"*", do_arith},
   {"/", do_arith},
   {"%", do_arith},
   {"^", do_arith},
   {"Y", do_debug},
   {"c", do_clear},
   {"d", do_dup},
   {"f", do_printall},
   {"p", do_print},
   {"q", do_quit},
};


//
// scan_options
//    Options analysis:  The only option is -Dflags. 
//

void scan_options (int argc, char **argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
   if (optind < argc) {
      complain() << "operand not permitted" << endl;
   }
}

int main (int argc, char **argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   bigint_stack operand_stack;
   scanner input;
   try {
      for (;;) {
         try {
            token_t token = input.scan();
            if (token.symbol == SCANEOF) break;
            switch (token.symbol) {
               case NUMBER:
                  operand_stack.push (token.lexinfo);
                  break;
               case OPERATOR: {
                  fn_map::const_iterator fn
                           = do_functions.find (token.lexinfo);
                  if (fn == do_functions.end()) {
                     throw ydc_exn (octal (token.lexinfo[0])
                                    + " is unimplemented");
                  }
                  fn->second (operand_stack, token.lexinfo.at(0));
                  break;
                  }
               default:
                  break;
            }
         }catch (ydc_exn &exn) {
            cout << exn.what() << endl;
         }
      }
   }catch (ydc_quit &) {
      // Intentionally left empty.
   }
   operand_stack.clear();
   return sys_info::get_status ();
}

