// $Id: object.h,v 1.13 2013-07-24 17:22:19-07 - - $

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <utility>

using namespace std;

#include "demangle.h"
#include "numbers.h"

//
// Objects constitute a single-inheritance hierarchy, summarized
// here, with the superclass listed first, and subclasses indented
// under their immediate superclass.
//
// object
//    text
//    shape
//       ellipse
//          circle
//       polygon
//          rectangle
//             square
//          line
//

class object;
typedef list<xycoords> coordlist;
typedef shared_ptr<object> object_ptr; 

//
// Abstract base class for all shapes in this system.
//

class object {
   friend ostream &operator<< (ostream &out, const object &);
   private:
      object (const object &) = delete;
      object &operator= (const object &) = delete;
   protected:
      inline object(); // Only subclass may instantiate.
   public:
      virtual ~object();
      virtual void draw (ostream &, const xycoords &,
                         const degrees &angle) = 0;
      virtual void show (ostream &) const;
};


//
// Class for printing text.
//

class text: public object {
   protected:
      string fontname;
      points fontsize;
      string textdata;
      text() = delete;
   public:
      virtual ~text();
      text (const string &fontname, const points &fontsize,
            const string &textdata);
      virtual void draw (ostream &, const xycoords &,
                         const degrees &angle);
      virtual void show (ostream &) const;
};

//
// Shape of a geometric object.
//

class shape: public object {
   protected:
      points thick;
      shape() = delete;
      shape (const points &thick);
      virtual void show (ostream &) const;
   public:
      virtual ~shape();
};

//
// Classes for ellipse and circle.
//

class ellipse: public shape {
   protected:
      xycoords dimension;
   public:
      virtual ~ellipse();
      ellipse (const inches &width, const inches &height,
               const points &thick);
      virtual void draw (ostream &, const xycoords &,
                         const degrees &angle);
      virtual void show (ostream &) const;
};

class circle: public ellipse {
   public:
      virtual ~circle();
      circle (const inches &diameter, const points &thick);
};


//
// Class polygon.
//

class polygon: public shape {
   protected:
      const coordlist coordinates;
   public:
      virtual ~polygon();
      polygon (const coordlist &coords, const points &thick);
      virtual void draw (ostream &, const xycoords &,
                         const degrees &angle);
      virtual void show (ostream &) const;
};

//
// Classes rectangle, square, and line..
//

class rectangle: public polygon {
   public:
      virtual ~rectangle();
      rectangle (const inches &width, const inches &height,
                 const points &thick);
};

class square: public rectangle {
   public:
      virtual ~square();
      square (const inches &width, const points &thick);
};

class line: public polygon {
   public:
      virtual ~line();
      line (const inches &length, const points &thick);
};

ostream &operator<< (ostream &out, const object &);

#endif

