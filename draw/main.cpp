// $Id: main.cpp,v 1.3 2013-07-26 17:44:05-07 - - $

#include <fstream>
#include <iostream>

using namespace std;

#include "interp.h"
#include "trace.h"
#include "util.h"

//
// Parse a file.  Read lines from input file, parse each line,
// and interpret the command.
//

void parsefile (const string &infilename,
                     istream &infile, ostream &outfile) {
   interpreter::object_map objmap;
   interpreter interp (infilename, outfile);
   for (int linenr = 1;; ++linenr) {
      try {
         string line;
         getline (infile, line);
         if (infile.eof()) break;
         if (line.size() == 0) continue;
         for (;;) {
            TRACE ('m', line);
            int last = line.size() - 1;
            if (line[last] != '\\') break;
            line[last] = ' ';
            string contin;
            getline (infile, contin);
            if (infile.eof()) break;
            line += contin;
         }
         interpreter::parameters words = split (line, " \t");
         if (words.size() == 0 || words.front()[0] == '#') continue;
         TRACE ('m', words);
         outfile << "%%Command[" << linenr << "]: " << line << endl;
         interp.interpret (words);
         outfile << endl;
      }catch (runtime_error &error) {
         complain() << infilename << ":" << linenr << ": "
                    << error.what() << endl;
      }
   }
   TRACE ('m', infilename << " EOF");
}


//
// Strip off the dirname portion and the suffix and tack on .ps.
//
string get_outfilename (const string &infilename) {
      string suffix = ".dr";
      int slashpos = infilename.find_last_of ('/') + 1;
      string outname = infilename.substr (slashpos);
      int baselen = outname.size();
      int suffixlen = suffix.size();
      int difflen = baselen - suffixlen;
      if (baselen > suffixlen && outname.substr (difflen) == suffix) {
         outname = outname.substr (0, difflen);
      }
      return outname + ".ps";
}

//
// Scan the option -@ and check for operands.
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
}


//
// Main function.  Iterate over files if given, use cin if not.
//
int main (int argc, char **argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   if (optind == argc) {
      parsefile ("-", cin, cout);
   }else {
      for (int argi = optind; argi < argc; ++argi) {
         const string infilename = argv[argi];
         ifstream infile (infilename.c_str());
         if (infile.fail()) {
            syscall_error (infilename);
            continue;
         }
         const string outfilename = get_outfilename (infilename);
         ofstream outfile (outfilename.c_str());
         if (outfile.fail()) {
            syscall_error (outfilename);
            continue;
         }
         TRACE ('m', infilename << " => " << outfilename);
         parsefile (infilename, infile, outfile);
         // fstream objects auto closed when destroyed
      }
   }
   return sys_info::get_exit_status();
}

