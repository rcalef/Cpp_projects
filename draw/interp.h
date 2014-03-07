// $Id: interp.h,v 1.11 2013-07-22 13:54:20-07 - - $

#ifndef __INTERP_H__
#define __INTERP_H__

#include <iostream>
#include <map>

using namespace std;

#include "object.h"
#include "trace.h"

class interpreter {
   public:
      typedef map <string, object_ptr> object_map;
      typedef list<string> parameters;
      interpreter (const string &, ostream &);
      void interpret (parameters &);
      ~interpreter();

   private:
      interpreter() = delete;
      interpreter (const interpreter &) = delete;
      interpreter &operator= (const interpreter &) = delete;

      typedef void (interpreter::*interpreterfn) (parameters &);
      typedef object_ptr (interpreter::*factoryfn) (parameters &);
      typedef map <string, interpreterfn> interp_map_t;
      typedef map <string, factoryfn> factory_map_t;

      static interp_map_t interp_map;
      static factory_map_t factory_map;
      ostream &outfile;
      int pagenr;
      object_map objmap;
      string infilename;
      double page_xoffset;
      double page_yoffset;

      void do_define (parameters &);
      void do_draw (parameters &);
      void do_newpage (parameters &);
      void do_dump (parameters &);
      void prolog();
      void startpage();
      void endpage();
      void epilog();

      object_ptr make_object (parameters &);
      object_ptr make_text (parameters &);
      object_ptr make_ellipse (parameters &);
      object_ptr make_circle (parameters &);
      object_ptr make_polygon (parameters &);
      object_ptr make_rectangle (parameters &);
      object_ptr make_square (parameters &);
      object_ptr make_line (parameters &);
};

#endif

