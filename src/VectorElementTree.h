// This class defines possible elements for the tree and 
// a factory ?

#pragma once

#include "SVGParserAdapter.h"
#include "CustomPolylinesGraphicsItem.h"
#include "tree.h"
#include <iterator>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/Qt/PolygonGraphicsItem.h>
#include <CGAL/Qt/PolygonWithHolesGraphicsItem.h>
#include <CGAL/squared_distance_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                    Point_2;
typedef K::Segment_2                  Segment_2;
typedef K::Vector_2                   Vector_2;

typedef K::Line_2                     Line_2;
typedef CGAL::Polygon_2<K>            Polygon_2;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes_2;

typedef CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> PolygonWithHolesGraphicsI;
typedef CGAL::Qt::PolygonGraphicsItem<Polygon_2> PolygonGraphicsI;
typedef CGAL::Qt::CustomPolylinesGraphicsItem<std::list<std::list<Point_2> > > PolylinesGraphicsI;

enum ElementType {
  EL_POLYGON,
  EL_FILLED_POLYGON,
  EL_POLYLINE
};

class ElementPtr {
public:
  bool visited;

  Point_2 entry_point;
  Point_2 exit_point;
  int entry_point_index;
  int exit_point_index;

  ElementPtr * to;
  ElementPtr * from;

  ElementPtr() :  visited(false) {};
  virtual Point_2 getFromIndex(int i) {};
  virtual void set_graphx() {};
  virtual ElementType get_type() {};
  virtual Polygon_2 * convexHull() {};
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
  PolygonGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&element);
    return;
  }
  Point_2 getFromIndex(int i) {
    return element[i];
  }

  ElementType get_type() { return EL_POLYGON; }
  void print_poly() {
    std::cout << "Polygon " << std::endl;
  }
  Polygon_2 * convexHull() {
    return &element;
  }
};

typedef tree< ElementPtr > Tree_ElementPtr;

class FilledPolygonElementPtr : public ElementPtr {
public:
  Polygon_with_holes_2 element;

  // int visited_vertices[];
  PolygonWithHolesGraphicsI * graphx;

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

  void set_graphx() {
    this->graphx = new PolygonWithHolesGraphicsI(&element);
    return;
  }
  ElementType get_type() { return EL_FILLED_POLYGON; };
  Point_2 getFromIndex(int i) {
    return element.outer_boundary()[i];
  }

  Polygon_2 * convexHull() {

    std::list<Point_2> convex_hull_list;
    // std::list<Point_2>::iterator convex_hull_iter = convex_hull_list.begin();
    CGAL::convex_hull_2(
      element.outer_boundary().vertices_begin(),
      element.outer_boundary().vertices_end(),
      back_inserter(convex_hull_list)
    );
    Polygon_2 * convex_hull = new Polygon_2(convex_hull_list.begin(), 
                                        convex_hull_list.end());

    return convex_hull;
    // Polygon_2(
    // )
  };

  void print_poly() {
    std::cout << "PolygonWithHolesGraphicsI " << std::endl;
  };

};

class PolyLineElementPtr : public ElementPtr {
public:
  std::list<Point_2>  element;

  // int visited_vertices[];
  PolyLineElementPtr(std::list<Point_2> polyline) : element(polyline) {};
  PolylinesGraphicsI * graphx;
  void set_graphx() {
    // this->graphx = new PolylinesGraphicsI(&element);
    return;
  }

  Point_2 getFromIndex(int i) {
    if(i == 0) {
      return element.front();
    }
    else return element.front();
  }
  ElementType get_type() { return EL_POLYLINE; }

  void print_poly() {
    std::cout << "Polygon " << std::endl;
  }
  Polygon_2 * convexHull() {
    return NULL;
  }

};

class VectorElementTree {
private:
  Tree_ElementPtr::iterator findSpot(ElementPtr * elem_ptr) {
    if(element_tree.size() == 1) { return element_tree.begin(); }  // return top elem if empty tree
    Tree_ElementPtr::breadth_first_iterator it = ++element_tree.begin_breadth_first();
    Tree_ElementPtr::breadth_first_iterator it_end = element_tree.end_breadth_first();
    for(; it != it_end; ++it) {
      if(it->get_type() != EL_POLYLINE) {
        // This is a Polygon, does it contain the points?
        // Also check convex hull of polygon!!
        if(elem_ptr->get_type() != EL_POLYLINE) {
          switch(CGAL::bounded_side_2(
            it->convexHull()->vertices_begin(),
            it->convexHull()->vertices_end(), elem_ptr->getFromIndex(0))){
            case CGAL::ON_BOUNDED_SIDE:
            case CGAL::ON_BOUNDARY:
              // if bounded, or on boundary:
              return it;
              break; // unnecessary?!
            case CGAL::ON_UNBOUNDED_SIDE:
              // Outside of poly. Outside of children.
              it. skip_children();
          }
        }
        // it->skip_children(); (skips children for one increment)
        //it->convexHull().onBounded
      }
    }
    return NULL;
  }
  ElementPtr * getElementRepresentation(VectorElement * ve) {
    if(ve->closed && ve->filled) {
      Polygon_2 outer = Polygon_2(ve->vertices.begin(), ve->vertices.end());
      std::list<Polygon_2> holes;
      for(std::list<Point_2> hole_vertices : ve->holes) {
        holes.push_back(Polygon_2(hole_vertices.begin(), hole_vertices.end()));
      }
      Polygon_with_holes_2 polygon_with_holes = Polygon_with_holes_2(outer, holes.begin(), holes.end());
      cout << "Polygon With Holes "<< polygon_with_holes << endl;
      return new FilledPolygonElementPtr(polygon_with_holes);
    }
    else if (ve->closed) {
      Polygon_2 polygon = Polygon_2(ve->vertices.begin(), ve->vertices.end());
      cout << "Polygon " << polygon << endl;
      return new PolygonElementPtr(polygon);
    }
    else {
      cout << "Polyline" << endl;
      return new PolyLineElementPtr(ve->vertices);
    }
  }
public:
  Tree_ElementPtr element_tree;
  PolygonElementPtr *   playfield;
  VectorElementTree() {
  };
  void createAndSortTree(ParsedSVG * ps) {
    // Here we have the list of all SVG elements
    // There is no CGAL representation of Polylines
    // Polylines are std::list<Point_2>
    Tree_ElementPtr::iterator top = element_tree.begin();
    playfield = new PolygonElementPtr(Polygon_2());
    element_tree.insert(top, *playfield); // Parent of all`
    for(VectorElement ve : ps->elements) {
      ElementPtr * elem_ptr = this->getElementRepresentation(&ve);
      // find parent tree iter if possible
      findSpot(elem_ptr);
    }
  };
};
