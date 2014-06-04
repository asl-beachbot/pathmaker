#pragma once

#include "CGAL_Headers.h"
#include <json/json.h>
#include <boost/format.hpp>
#include <stdlib.h>

using boost::format; using boost::str;

typedef std::vector<unsigned char> RakeVector;

#ifdef WITH_GUI
#include <QColor>
#include <QPen>
#endif

enum ElementType {
  EL_POLYGON,
  EL_FILLED_POLYGON,
  EL_POLYLINE
};

enum Rake {
  RAKE_ZERO = 0,
  RAKE_SMALL = 0 | 1 << 3,
  RAKE_MEDIUM = 0x1c,
  RAKE_LARGE = 0x3e, 
  RAKE_FULL = 0x7f
};

enum FillMethods {
  WIGGLE_FILL = 1,
  SPIRAL_FILL = 2
};


class ElementPtr {
public:
  bool visited;
  bool fill_element;
  bool post_processed_result;
  bool manually_modified;
  int line_width;
  RakeVector rake_states;

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
  unsigned int id;
  ElementPtr() :  
    visited(false), 
    to(NULL), 
    from(NULL), 
    fill_element(false), 
    post_processed_result(false),
    manually_modified(false)
  {
    id = rand() % 10000000;
  };
  ~ElementPtr() {cout << "delete called" << endl;} 
  virtual Point_2 getFromIndex(int i) = 0;
  virtual int getSize() = 0;
  virtual void set_graphx() = 0;
  virtual ElementType get_type() = 0;
  virtual Polygon_2 * convexHull() = 0;
  virtual void print() = 0;
  virtual std::string toString() = 0;
  bool unvisited() {
    return !this->visited;
  }
  virtual Json::Value toJSON() = 0;
};


class PolygonElementPtr : public ElementPtr {
public:
  Polygon_2 element;
  // int visited_vertices[];
  PolygonElementPtr(Polygon_2 poly, int lw = Rake::RAKE_MEDIUM) {
    if(!poly.is_clockwise_oriented()) {
       // think about turning direction of polygon here!
      // std::reverse(poly.vertex_begin(), poly.vertex_end());
      cout << "Polygon counterclockwise" << endl;
    } else {
      cout << "Polygon clockwise" << endl;      
    }
    this->element = poly;
    this->rake_states = RakeVector(poly.size(), lw);
    this->visited = false;
    this->line_width = lw;
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
    return element[i % element.size()];
  }
  int getSize() {
    return element.size();
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
    val["id"] = id;
    Polygon_2 scaled_elem = transform(
      Transformation(CGAL::SCALING, GlobalOptions::getInstance().scale_for_disp), 
      element);
    auto it = scaled_elem.vertices_begin();
    auto it_end = scaled_elem.vertices_end();
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
      // poor mans circulator
      res += str(format("%1% %2% %3%\n") % element[idx + i % len].x() % element[idx + i % len].y() % (int)rake_states[idx + i % len]);
    }
    return res;
  }
};

class FilledSegment : public PolygonElementPtr {
public:
  FilledSegment(Polygon_2 poly) : PolygonElementPtr(poly) {};
  int fill_method; // fill type: 1 = Skeleton, 2 = wiggle
  Direction_2 direction; // only for wiggle fill
};

class FilledPolygonElementPtr : public ElementPtr {
public:

  // int visited_vertices[];
  Polygon_with_holes_2 element;
  std::vector<FilledSegment> segments;
  int fill_method; // fill type: 1 = Skeleton, 2 = wiggle
  Direction_2 direction; // only for wiggle fill

  FilledPolygonElementPtr(Polygon_with_holes_2 poly, int lw = Rake::RAKE_MEDIUM) :
    element(poly){
      this->line_width = lw;
      fill_method = GlobalOptions::getInstance().fill_method;
    // This probably will have to be a bit more complicated!
    // this->rake_states = RakeVector(poly.size(), line_width);

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
  std::list<Polygon_2> scaled_segments;
  QPen segments_pen;
  void set_graphx() {
    QColor segments_pen_color(0, 100, 100);
    segments_pen = QPen(segments_pen_color, 0);
    this->graphx = new PolygonWithHolesGraphicsI(&element);
    this->graphx->setEdgesPen(pen);
    auto it = segments.begin();
    auto it_end = segments.end();
    Transformation t = Transformation(CGAL::SCALING, GlobalOptions::getInstance().scale_for_disp);
    for( ; it != it_end; ++it ) {
      Polygon_2 ps = transform(t, it->element);
      scaled_segments.push_back(ps);
      segments_graphx.push_back(new PolygonGraphicsI(&scaled_segments.back()));
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
  int getSize() {
    return element.outer_boundary().size();
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
    val["id"] = id;
    Transformation disp_trafo = Transformation(CGAL::SCALING, GlobalOptions::getInstance().scale_for_disp);
    Polygon_2 scaled_outer = transform(disp_trafo, element.outer_boundary());
    auto it = scaled_outer.vertices_begin();
    auto it_end = scaled_outer.vertices_end();
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
      Polygon_2 hole = transform(disp_trafo, (*it_holes));
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
      Polygon_2 scaled_segment = transform(disp_trafo, it_segments->element);
      auto segment_it = scaled_segment.vertices_begin();
      auto segment_it_end = scaled_segment.vertices_end();
      Json::Value segment_coords(Json::arrayValue);
      for(; segment_it != segment_it_end; ++segment_it) {
        Json::Value c(Json::arrayValue);
        c.append(segment_it->x());
        c.append(segment_it->y());
        segment_coords.append(c);
      }
      Json::Value segment_value;
      segment_value["fill_method"] = it_segments->fill_method;
      segment_value["direction_dx"] = it_segments->direction.dx();
      segment_value["direction_dy"] = it_segments->direction.dy();
      segment_value["coords"] = segment_coords;
      segments_json.append(segment_value);
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
  }
};

class PolyLineElementPtr : public ElementPtr {
public:
  // Change implementation to Curve_2!
  PolyLine_P element;
  std::list< PolyLine_P > graphx_elem;
  // int visited_vertices[];
  PolyLineElementPtr(PolyLine_P polyline, int lw = Rake::RAKE_MEDIUM) : element(polyline) {
    this->line_width = lw;
    this->rake_states = RakeVector(polyline.size(), lw);
  };
  PolyLineElementPtr(std::list<Point_2> polyline, int lw = Rake::RAKE_MEDIUM) {
    this->element = PolyLine_P{std::begin(polyline),
                               std::end(polyline)};
    this->rake_states = RakeVector(polyline.size(), lw);
    this->line_width = lw;

  };
  #ifdef WITH_GUI
  PolylinesGraphicsI * graphx;
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
    if(i >= 0) {
      return element[i];
    } else {
      return element[element.size() + i];
    }
  }
  int getSize() {
    return element.size();
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
    std::istream_iterator< Point_2 >  in_start();
    std::istream_iterator< Point_2 >  in_end;
    std::ostream_iterator< Point_2 >  out( std::cout, "\n" );
    Polygon_2 ch;
    cout << "Convex Hull" << endl;
    auto op_it = CGAL::convex_hull_2(element.begin(), element.end(), std::iterator<Point_2> output);
    for(Point_2 p : output) {
      cout << p << endl;
    }
    return NULL;
  }
  Json::Value toJSON() {
    Json::Value val;
    val["type"] = "POLYLINE";
    val["type_int"] = EL_POLYLINE;
    val["manually_modified"] = manually_modified;
    val["stroke_width"] = line_width;
    
    val["id"] = id;
    auto it = element.begin();
    auto it_end = element.end();
    Json::Value coords(Json::arrayValue);
    Transformation disp_trafo = Transformation(CGAL::SCALING, GlobalOptions::getInstance().scale_for_disp);
    for(; it != it_end; ++it) {
      Json::Value c(Json::arrayValue);
      Point_2 scaled = (*it).transform(disp_trafo);
      c.append(scaled.x());
      c.append(scaled.y());
      coords.append(c);
    }
    val["coords"] = coords;
    return val;
  }
  std::string toString() {
    std::string res;
    assert(element.size() == rake_states.size());
    for(int i = 0; i < element.size(); ++i) {
      res += str(format("%1% %2% %3%\n")  % element[i].x() % element[i].y() % (int)rake_states[i]);
    }
    return res;
  }
};
