// $Id: interp.cpp,v 1.16 2013-08-04 19:19:29-07 - - $

#include <list>
#include <map>
#include <memory>
#include <string>
#include <sstream>

using namespace std;

#include "interp.h"
#include "object.h"
#include "util.h"


interpreter::interp_map_t interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"newpage", &interpreter::do_newpage},
   {"dump"   , &interpreter::do_dump   },
};

interpreter::factory_map_t interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"line"     , &interpreter::make_line     },
};

interpreter::interpreter(const string &filename, ostream &outfile):
   outfile(outfile),
   pagenr(1),
   objmap(),
   infilename(filename),
   page_xoffset (inches(0.25).point_value()),
   page_yoffset (inches(0.25).point_value())
{
   prolog();
   startpage();
}

interpreter::~interpreter() {
   endpage();
   epilog();
   if (traceflags::getflag ('i')) {
      parameters dump {"dump"};
      do_dump (dump);
   }
}

string shift (list<string> &words) {
   if (words.size() == 0) throw runtime_error ("syntax error");
   string front = words.front();
   words.pop_front();
   return front;
}


void interpreter::interpret (parameters &params) {
   TRACE ('i', params);
   string command = shift (params);
   interp_map_t::const_iterator itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   (this->*func) (params);
}

void interpreter::do_define (parameters &params) {
   TRACE ('i', params);
   string name = shift (params);
   objmap.insert (object_map::value_type (name, make_object (params)));
}

void interpreter::do_draw (parameters &params) {
   TRACE ('i', params);
   string name = shift (params);
   object_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such object");
   }
   degrees angle = degrees (0);
   if (params.size() == 3) {
      angle = degrees (from_string<double> (params.back()));
      params.pop_back();
   }
   if (params.size() != 2) throw runtime_error ("syntax error");
   xycoords coords (inches (from_string<double> (params.front())),
                    inches (from_string<double> (params.back())));
   itor->second->draw (outfile, coords, angle);
}

void interpreter::do_newpage (parameters &params) {
   if (params.size() != 0) throw runtime_error ("syntax error");
   endpage();
   ++pagenr;
   startpage();
}

void interpreter::do_dump (parameters &params) {
   traceflags::setflags ("*");
   TRACE ('*', params);
   for (object_map::const_iterator itor = objmap.cbegin();
        itor != objmap.cend(); ++itor) {
      cerr << itor->first << ": " << *itor->second << endl;
   }
}


void interpreter::prolog() {
   outfile << "%!PS-Adobe-3.0" << endl;
   outfile << "%%Creator: " << sys_info::get_execname() << endl;
   outfile << "%%CreationDate: " << datestring() << endl;
   outfile << "%%PageOrder: Ascend" << endl;
   outfile << "%%Orientation: Portrait" << endl;
   outfile << "%%SourceFile: " << infilename << endl;
   outfile << "%%EndComments" << endl;
}

void interpreter::startpage() {
   outfile << endl;
   outfile << "%%Page: " << pagenr << " " << pagenr << endl;
   outfile << page_xoffset << " " << page_yoffset
           << " translate" << endl;
   outfile << "/Courier findfont 10 scalefont setfont" << endl;
   outfile << "0 0 moveto (" << infilename << ":"
           << pagenr << ") show" << endl;

}

void interpreter::endpage() {
   outfile << endl;
   outfile << "showpage" << endl;
   outfile << "grestoreall" << endl;
}

void interpreter::epilog() {
   outfile << endl;
   outfile << "%%Trailer" << endl;
   outfile << "%%Pages: " << pagenr << endl;
   outfile << "%%EOF" << endl;

}


object_ptr interpreter::make_object (parameters &command) {
   TRACE ('f', command);
   string type = shift (command);
   factory_map_t::const_iterator itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such object");
   }
   factoryfn func = itor->second;
   return (this->*func) (command);
}

object_ptr interpreter::make_text (parameters &command) {
   TRACE ('f', command);
   string first = shift(command);
   string font = "";
   double size = 12.0; //Default size is 12 for text
   //If conversion succeeds, then next string on list is the fontname,
   //otherwise the string that couldn't be converted to a double is
   //the fontname.
   try{
      size = from_string<double>(first);
      font = shift(command);
   }
   catch (runtime_error &error){
      font = first;
   }
   //At this point, the rest of the words are text to be printed.
   string textdata{};
   parameters::const_iterator itor = command.begin();
   parameters::const_iterator end = command.end();
   --end;
   for(; itor != end; ++itor){
      textdata += *itor + ' ';
   }textdata += *itor;//remove trailing space
   return make_shared<text> (font, points(size), textdata);
}

object_ptr interpreter::make_ellipse (parameters &command) {
   TRACE ('f', command);
   if (command.size() > 3) throw runtime_error("syntax error");
   double width = from_string<double>(shift(command));
   double height = from_string<double>(shift(command));
   double thick = 2.0; //Default thickness of 2
   if(command.size() != 0) thick = from_string<double>(shift(command));
   return make_shared<ellipse> (inches(width), inches(height),
         points(thick));
}

object_ptr interpreter::make_circle (parameters &command) {
   TRACE ('f', command);
   if (command.size() > 2) throw runtime_error("syntax error");
   double diameter = from_string<double>(shift(command));
   double thick = 2.0; //Default thickness of 2
   if(command.size() != 0) thick = from_string<double>(shift(command));
   return make_shared<circle> (inches(diameter), points(thick));
}

object_ptr interpreter::make_polygon (parameters &command) {
   TRACE ('f', command);
   double thick = 2.0; //Default thickness of 2
   if (command.size() % 2 == 1){
      thick = from_string<double>(command.back());
      command.pop_back();
   }
   coordlist poly_points{};
   while(!command.empty()){
      double x = from_string<double>(shift(command));
      double y = from_string<double>(shift(command));
      poly_points.push_back({inches(x), inches(y)});
   }
   return make_shared<polygon> (poly_points, points(thick));
}

object_ptr interpreter::make_rectangle (parameters &command) {
   TRACE ('f', command);
   if(command.size() > 3) throw runtime_error ("syntax error");
   double width = from_string<double>(shift(command));
   double height = from_string<double>(shift(command));
   double thick = 2.0; //Default thickness of 2
   if(command.size() != 0) thick = from_string<double>(shift(command));
   return make_shared<rectangle> (inches(width), inches(height),
         points(thick));
}

object_ptr interpreter::make_square (parameters &command) {
   TRACE ('f', command);
   if(command.size() > 2) throw runtime_error ("syntax error");
   double width = from_string<double>(shift(command));
   double thick = 2; //Default thickness of 2
   if(command.size() != 0) thick = from_string<double>(shift(command));
   return make_shared<square> (inches(width), points(thick));
}

object_ptr interpreter::make_line (parameters &command) {
   TRACE ('f', command);
   if(command.size() > 2) throw runtime_error ("syntax error");
   double length = from_string<double>(shift(command));
   double thick = 2.0; //Default thickness of 2
   if(command.size() != 0) thick = from_string<double>(shift(command));
   return make_shared<line> (inches(length), points(thick));
}

