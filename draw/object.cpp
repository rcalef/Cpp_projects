// $Id: object.cpp,v 1.10 2013-08-04 18:19:58-07 - - $

#include <typeinfo>

using namespace std;

#include "object.h"
#include "util.h"

#define NEW_TRACE(OBJ) TRACE ('c', "new: " << endl << *this)

object::object() {
   NEW_TRACE (*this);
}

text::text (const string &font, const points &size, const string &data):
      fontname(font), fontsize(size), textdata(data) {
   NEW_TRACE (*this);
}

shape::shape (const points &thick): thick(thick) {
   NEW_TRACE (*this);
}

ellipse::ellipse (const inches &width, const inches &height,
                  const points &thick):
      shape(thick), dimension ({width, height}) {
   NEW_TRACE (*this);
}

circle::circle (const inches &diameter, const points &thick):
      ellipse (diameter, diameter, thick) {
   NEW_TRACE (*this);
}

polygon::polygon (const coordlist &coords, const points &thick):
      shape(thick), coordinates(coords) {
   NEW_TRACE (*this);
}

rectangle::rectangle (const inches &width, const inches &height,
                      const points &thick):
      polygon (coordlist ({{0, height}, {width, 0}, {0, -height}}),
               thick) {
   NEW_TRACE (*this);
}

square::square (const inches &width, const points &thick):
      rectangle (width, width, thick) {
   NEW_TRACE (*this);
}

line::line (const inches &length, const points &thick):
      polygon (coordlist ({{length, 0}}), thick) {
   NEW_TRACE (*this);
}


#define DELETE_TRACE(OBJ) TRACE ('c', "delete: " << endl << *this)
object   ::~object()    { DELETE_TRACE (*this); }
text     ::~text()      { DELETE_TRACE (*this); }
shape    ::~shape()     { DELETE_TRACE (*this); }
ellipse  ::~ellipse()   { DELETE_TRACE (*this); }
circle   ::~circle()    { DELETE_TRACE (*this); }
polygon  ::~polygon()   { DELETE_TRACE (*this); }
rectangle::~rectangle() { DELETE_TRACE (*this); }
square   ::~square()    { DELETE_TRACE (*this); }
line     ::~line()      { DELETE_TRACE (*this); }

#define DRAW_TRACE(COORDS,ANGLE,OBJ) \
        TRACE ('d', "draw: " << COORDS << " " << ANGLE << endl << OBJ)

void text::draw (ostream &out, const xycoords &coords,
                 const degrees &angle) {
   DRAW_TRACE (coords, angle, *this);
   string indent = "   ";
   out << "gsave" << endl;
   out << indent << "/" << fontname << " findfont" << endl
   << indent << fontsize.point_value() << " scalefont setfont" << endl;
   out << indent << coords.first.point_value() << ' '
         << coords.second.point_value() << " translate" << endl;
   out << indent << angle.degree_value() << " rotate" << endl;
   out << indent << "0 0 moveto" << endl;
   out << indent << "(";
   string::const_iterator itor = textdata.begin();
   string::const_iterator end = textdata.end();
   for (; itor != end; ++itor){
      if(*itor == '(' || *itor == ')' || *itor == '\\') out << '\\';
      out << *itor;
   }
   out << ")" << endl;
   out << indent << "show" << endl;
   out << "grestore" << endl;
}

void ellipse::draw (ostream &out, const xycoords &coords,
                    const degrees &angle) {
   DRAW_TRACE (coords, angle, *this);
   string indent = "   ";
   out << "gsave" << endl;
   out << indent <<  "newpath" << endl;
   out << indent << " /save matrix currentmatrix def" << endl;
   out << indent << coords.first.point_value() << ' '
         << coords.second.point_value() << " translate" << endl;
   out << indent << angle.degree_value() << " rotate" << endl;
   double xscale = 1.0;
   double yscale = 1.0;
   double radius{};
   double width = dimension.first.point_value();
   double height = dimension.second.point_value();
   if(width < height){
      yscale = width / height;
      radius = height / 2.0;
   }else{
      xscale = height / width;
      radius = width / 2.0;
   }
   out << indent << xscale << ' ' << yscale << " scale" << endl;
   out << indent << "0 0 " << radius <<" 0 360 arc" << endl;
   out << indent << "save setmatrix" << endl;
   out << indent << thick.point_value() << " setlinewidth" << endl;
   out << indent << "stroke" << endl;
   out <<"grestore" << endl;
}

void polygon::draw (ostream &out, const xycoords &coords,
                    const degrees &angle) {
   DRAW_TRACE (coords, angle, *this);
   string indent = "   ";
   out <<"gsave" << endl;
   out << indent << "newpath" << endl;
   out << indent << coords.first.point_value() << ' '
         << coords.second.point_value() << " translate" << endl;
   out << indent << angle.degree_value() << " rotate" << endl;
   out << indent << "0 0 moveto" << endl;
   coordlist::const_iterator itor = coordinates.begin();
   coordlist::const_iterator end = coordinates.end();
   for(;itor != end; ++itor){
      out << indent << itor->first.point_value() << ' '
            << itor->second.point_value() << " rlineto" << endl;
   }
   out << indent << "closepath" << endl;
   out << indent << thick.point_value() << " setlinewidth" << endl;
   out << indent << "stroke" << endl;
   out << "grestore" << endl;
}


void object::show (ostream &out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream &out) const {
   object::show (out);
   out << fontname << "[" << fontsize << "] \"" << textdata << "\"";
}

void shape::show (ostream &out) const {
   object::show (out);
   out << thick << ": ";
}

void ellipse::show (ostream &out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream &out) const {
   shape::show (out);
   out << "{" << coordinates << "}";
}

ostream &operator<< (ostream &out, const object &obj) {
   obj.show (out);
   return out;
}

