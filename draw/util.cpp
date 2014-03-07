// $Id: util.cpp,v 1.1 2013-07-26 17:12:34-07 - - $

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

using namespace std;

#include "demangle.h"
#include "util.h"

int sys_info::exit_status = EXIT_SUCCESS;
string sys_info::execname; // Must be initialized from main().

void sys_info_error (const string &condition) {
   throw logic_error ("main() has " + condition
               + " called sys_info::set_execname()");
}

void sys_info::set_execname (const string &argv0) {
   if (execname.size() != 0) sys_info_error ("already");
   int slashpos = argv0.find_last_of ('/') + 1;
   execname = argv0.substr (slashpos);
   cout << boolalpha;
   cerr << boolalpha;
   TRACE ('u', "execname = " << execname);
}

const string &sys_info::get_execname() {
   if (execname.size() == 0) sys_info_error ("not yet");
   return execname;
}

void sys_info::set_exit_status (int status) {
   if (execname.size() == 0) sys_info_error ("not yet");
   exit_status = status;
}

int sys_info::get_exit_status() {
   if (execname.size() == 0) sys_info_error ("not yet");
   return exit_status;
}

const string datestring() {
   time_t clock = time (NULL);
   struct tm *tm_ptr = localtime (&clock);
   char timebuf[128];
   strftime (timebuf, sizeof timebuf,
             "%a %b %e %H:%M:%S %Z %Y", tm_ptr);
   return timebuf;
}


list<string> split (const string &line, const string &delimiters) {
   list<string> words;
   int end = 0;
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   TRACE ('u', words);
   return words;
}

ostream &complain() {
   sys_info::set_exit_status (EXIT_FAILURE);
   cerr << sys_info::get_execname() << ": ";
   return cerr;
}

void syscall_error (const string &object) {
   complain() << object << ": " << strerror (errno) << endl;
}

template <typename item_t>
ostream &operator<< (ostream &out, const list<item_t> &vec) {
   bool printed_one = false;
   for (typename list<item_t>::const_iterator itor = vec.begin();
        itor != vec.end(); ++itor) {
      if (printed_one) cout << " ";
      out << *itor;
      printed_one = true;
   }
   return out;
}

template <typename item_t>
string to_string (const item_t &that) {
   ostringstream stream;
   stream << that;
   return stream.str();
}

template <typename item_t>
item_t from_string (const string &that) {
   stringstream stream;
   stream << that;
   item_t result;
   bool converted = stream >> result  // Is string is a valid item_t?
                 && stream >> std::ws // Flush trailing white space.
                 && stream.eof();     // Must now be at end of stream.
   if (! converted) {
      throw range_error (demangle (result)
            + " from_string (" + that + ")");
   }
   return result;
}

#include "templates.tcc"

