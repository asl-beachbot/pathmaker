#pragma once

#include "CGAL_Headers.h"

#ifdef WITH_GUI
#include <QColor>
#include <QPen>
#endif

enum ElementType {
  EL_POLYGON,
  EL_FILLED_POLYGON,
  EL_POLYLINE
};

class ElementPtr {
public:
  bool visited;
  #ifdef WITH_GUI
  QPen pen;
  void setColorFromDepth(int depth) {
    QColor pen_color(depth * 40, 100, 0);
    pen = QPen(pen_color, 0);
  }
  #endif

  Point_2 entry_point;
  Point_2 exit_point;
  int entry_point_index;
  int exit_point_index;

  ElementPtr * to;
  ElementPtr * from;

  ElementPtr() :  visited(false), to(NULL), from(NULL) {};
  ~ElementPtr() {cout << "delete called" << endl;};
  virtual Point_2 getFromIndex(int i) {};
  virtual void set_graphx() {};
  virtual ElementType get_type() {};
  virtual Polygon_2 * convexHull() {};
  virtual void print() {};
  bool unvisited() {
    return !this->visited;
  }
};


class PolygonElementPtr : public ElementPtr {
public:
  Polygon_2 element;

  // int visited_vertices[];
  PolygonElementPtr(Polygon_2 poly) {
    this->element = poly;
    this->visited = false;
  };
  #ifdef WITH_GUI
  PolygonGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&element);
    this->graphx->setEdgesPen(pen);
    return;
  }
  #endif
  Point_2 getFromIndex(int i) {
    return element[i];
  }
  ElementType get_type() { return EL_POLYGON; }
  void print() {
    std::cout << "Polygon " << element << std::endl;
  }
  Polygon_2 * convexHull() {
    return &element;
  }
};

class FilledPolygonElementPtr : public ElementPtr {
public:

  // int visited_vertices[];
  Polygon_with_holes_2 element;

  FilledPolygonElementPtr(Polygon_with_holes_2 poly) : element(poly) {
    // convex_hull = 
    // std::list<Point_2> convex_hull_list;
    // convex_hull_list::iterator convex_hull_iter = convex_hull_list.begin();
    // CGAL::convex_hull_2(
    //   CGAL::element.outer_boundary().vertices_begin(),
    //   CGAL::element.outer_boundary().vertices_end(),
    //   convex_hull_iter
    // )

    // Polygon_2(
    // )

  };
  #ifdef WITH_GUI
  PolygonWithHolesGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonWithHolesGraphicsI(&element);
    this->graphx->setEdgesPen(pen);
    return;
  }
  #endif
  std::list< ElementPtr * > fill_elements;
  ElementType get_type() { return EL_FILLED_POLYGON; };
  Point_2 getFromIndex(int i) {
    return element.outer_boundary()[i];
  }

  Polygon_2 * convexHull() {
    // TODO check if convex Hull is working!
    return &this->element.outer_boundary();
    std::list<Point_2> convex_hull_list;
    // std::list<Point_2>::iterator convex_hull_iter = convex_hull_list.begin();
    CGAL::convex_hull_2(
      element.outer_boundary().vertices_begin(),
      element.outer_boundary().vertices_end(),
      back_inserter(convex_hull_list)
    );
    Polygon_2 * convex_hull = new Polygon_2(convex_hull_list.begin(),
                                            convex_hull_list.end());
    cout << * convex_hull << endl;
    return convex_hull;
    // Polygon_2(
    // )
  };

  void print() {
    std::cout << "Polygon With Holes " << element << std::endl;
  };

};

class PolyLineElementPtr : public ElementPtr {
public:
  std::list<Point_2>  element;
  std::list< std::list < Point_2 > > graphx_elem;
  // int visited_vertices[];
  PolyLineElementPtr(std::list<Point_2> polyline) : element(polyline) {};
  #ifdef WITH_GUI
  CGAL::Qt::CustomPolylinesGraphicsItem<std::list<std::list<Point_2> > > * graphx;
  void set_graphx() {
    graphx_elem.push_back(element);
    this->graphx = new PolylinesGraphicsI(&graphx_elem);
    this->graphx->setEdgesPen(pen);
    return;
  }
  #endif
  // Start and endpoints:
  // What if it is a spiral ? Start in the middle
  // would be preferred!
  // Solution: Calculate convex hull
  // And check if first or last point are inside
  // if inside: Choose as start point!

  Point_2 getFromIndex(int i) {
    if(i == 0) {
      return element.front();
    }
    else return element.front();
  }
  ElementType get_type() { return EL_POLYLINE; }

  void print() {
    std::cout << "Polyline ";
    for(Point_2 p : element) {
      cout << p << " ";
    }
    cout << std::endl;
  }
  Polygon_2 * convexHull() {
    return NULL;
  }
};
