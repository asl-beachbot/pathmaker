#pragma once

#include "CGAL_Headers.h"
#include <json/json.h>
#include <boost/format.hpp>

using boost::format; using boost::str;

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
  virtual std::string toString() {};
  bool unvisited() {
    return !this->visited;
  }
  virtual Json::Value toJSON() {};
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
  Json::Value toJSON() {
    Json::Value val;
    val["type"] = "POLYGON";
    val["type_int"] = EL_POLYGON;
    auto it = element.vertices_begin();
    auto it_end = element.vertices_end();
    Json::Value coords(Json::arrayValue);
    for(; it != it_end; ++it) {
      Json::Value c(Json::arrayValue);
      c.append(it->x());
      c.append(it->y());
      coords.append(c);
    }
    val["coords"] = coords;
    return val;
  }
  std::string toString() {
    std::string res;
    int idx = entry_point_index;
    int len = element.size();
    for(int i = 0; i <= len; ++i) {
      res += str(format("%1% %2% %3%\n") % element[idx + i % len].x() % element[idx + i % len].y() % 0b00010000);
    }
    return res;
  }
};

class FilledPolygonElementPtr : public ElementPtr {
public:

  // int visited_vertices[];
  Polygon_with_holes_2 element;
  std::list<Polygon_2> segments;
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
  std::list<PolygonGraphicsI *> segments_graphx;
  QPen segments_pen;
  void set_graphx() {
    QColor segments_pen_color(0, 100, 100);
    segments_pen = QPen(segments_pen_color, 2);
    this->graphx = new PolygonWithHolesGraphicsI(&element);
    this->graphx->setEdgesPen(pen);
    auto it = segments.begin();
    auto it_end = segments.end();
    for( ; it != it_end; ++it ) {
      cout << "adding segment: " << *it << endl;
      segments_graphx.push_back(new PolygonGraphicsI(&(*it)));
      segments_graphx.back()->setEdgesPen(segments_pen);
    }
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
  Json::Value toJSON() {
    // Polygon, holes, and segments to JSON
    Json::Value val;
    val["type"] = "FILLED_POLYGON";
    val["type_int"] = EL_FILLED_POLYGON;
    auto it = element.outer_boundary().vertices_begin();
    auto it_end = element.outer_boundary().vertices_end();
    Json::Value coords(Json::arrayValue);
    for(; it != it_end; ++it) {
      Json::Value c(Json::arrayValue);
      c.append(it->x());
      c.append(it->y());
      coords.append(c);
    }
    val["coords"] = coords;

    auto it_holes = element.holes_begin();
    auto it_end_holes = element.holes_end();
    Json::Value holes(Json::arrayValue);
    for(; it_holes != it_end_holes; ++it_holes) {
      Polygon_2 hole = (*it_holes);
      auto hole_it = hole.vertices_begin();
      auto hole_it_end = hole.vertices_end();
      Json::Value hole_coords(Json::arrayValue);
      for(; hole_it != hole_it_end; ++hole_it) {
        Json::Value c(Json::arrayValue);
        c.append(hole_it->x());
        c.append(hole_it->y());
        hole_coords.append(c);
      }
      holes.append(hole_coords);
    }
    val["holes"] = holes;
    auto it_segments = segments.begin();
    auto it_end_segments = segments.end();
    Json::Value segments_json(Json::arrayValue);
    for(; it_segments != it_end_segments; ++it_segments) {
      auto segment_it = it_segments->vertices_begin();
      auto segment_it_end = it_segments->vertices_end();
      Json::Value segment_coords(Json::arrayValue);
      for(; segment_it != segment_it_end; ++segment_it) {
        Json::Value c(Json::arrayValue);
        c.append(segment_it->x());
        c.append(segment_it->y());
        segment_coords.append(c);
      }
      segments_json.append(segment_coords);
    }
    val["segments"] = segments_json;

    return val;
  }
  std::string toString() {
    std::string res;
    int idx = entry_point_index;
    auto outer = element.outer_boundary();
    int len = outer.size();
    for(int i = 0; i <= len; ++i) {
      res += str(format("%1% %2% %3%\n") % outer[idx + i % len].x() % outer[idx + i % len].y() % 0b00010000);
    }
    return res;
    return res;
  }
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
  Json::Value toJSON() {
    Json::Value val;
    val["type"] = "POLYLINE";
    val["type_int"] = EL_POLYLINE;
    auto it = element.begin();
    auto it_end = element.end();
    Json::Value coords(Json::arrayValue);
    for(; it != it_end; ++it) {
      Json::Value c(Json::arrayValue);
      c.append(it->x());
      c.append(it->y());
      coords.append(c);
    }
    val["coords"] = coords;
    return val;
  }
  std::string toString() {
    std::string res;
    for(Point_2 p : element) {
      res += str(format("%1% %2% %3%\n")  % p.x() % p.y() % 0b00010000);
    }
    return res;
  }
};
