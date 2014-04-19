// This class defines possible elements for the tree and 
// a factory ?

#pragma once

#include "SVGParserAdapter.h"
#include "CustomPolylinesGraphicsItem.h"
#include "tree.h"
#include <iterator>

#include "CGAL_Headers.h"

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
  PolygonGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&element);
    return;
  }
  Point_2 getFromIndex(int i) {
    return element[i];
  }

  ElementType get_type() { return EL_POLYGON; }
  void print() {
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
    cout << * convex_hull << endl;
    return convex_hull;
    // Polygon_2(
    // )
  };

  void print() {
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

  void print() {
    std::cout << "PolyList" << std::endl;
  }
  Polygon_2 * convexHull() {
    return NULL;
  }

};

class VectorElementTree {
private:
  bool isInside(ElementPtr * elem, ElementPtr * tested_elem) {
    if(elem->get_type() != EL_POLYLINE) {
      // This is a Polygon, does it contain the points?
      // Also check convex hull of polygon!!
      if(tested_elem->get_type() != EL_POLYLINE) {
        switch(CGAL::bounded_side_2(
          elem->convexHull()->vertices_begin(),
          elem->convexHull()->vertices_end(), tested_elem->getFromIndex(0))){
          case CGAL::ON_BOUNDED_SIDE:
          case CGAL::ON_BOUNDARY:
            // if bounded, or on boundary:
            // this is definitly not enough
            // subtrees have to be moved etcetera
            cout << "On Bounded Side" << endl;
            return true;
            break; // unnecessary?!
          case CGAL::ON_UNBOUNDED_SIDE:
            // Outside of poly. Outside of children.
            return false;
        }
      }
      // it->skip_children(); (skips children for one increment)
      //it->convexHull().onBounded
      return false; 
    }
  }
  Tree_ElementPtr::iterator findSpot(ElementPtr * elem_ptr) {
    // if(element_tree.size() == 1) {

    // }  // return top elem if empty tree
    Tree_ElementPtr::breadth_first_iterator it = element_tree.begin_breadth_first();
    Tree_ElementPtr::breadth_first_iterator it_end = element_tree.end_breadth_first();
    Tree_ElementPtr::iterator curr_parent = element_tree.begin();
    elem_ptr->print();
    // cout << "Length: " << it - it_end << endl;
    for(; it != it_end; ++it) {
      cout << "Checking all polys" <<  endl;
      it->print();

      // if(it == curr_parent) {continue;}
      // if(isInside(&(*it), elem_ptr)) {
      //   curr_parent = it;
      //   cout << "Curr Parent " << &(*curr_parent) << endl;

      //   // it = Tree_ElementPtr::breadth_first_iterator(it.node->first_child);
      // } else {  
      //   //it.skip_children();
      // }
    }
    // found some parent?
    Tree_ElementPtr::iterator new_element_iter;

    cout << "Inserting Element at " << &(*curr_parent) << endl;
    new_element_iter = element_tree.insert(curr_parent, *elem_ptr);
    // iterate over siblings: are they children of the new node?
    // If yes: reparent!
    // Tree_ElementPtr::sibling_iterator sit = element_tree.begin(curr_parent);
    // Tree_ElementPtr::sibling_iterator sit_end = element_tree.end(curr_parent);
    // for(; sit != sit_end; ++sit) {
    //   if(&(*sit) == elem_ptr) { continue; }
    //   if(isInside(elem_ptr, &(*sit))) {
    //     // reparent subtree
    //     element_tree.insert(new_element_iter, *sit);
    //   }
    // }
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
  PolygonElementPtr * playfield;
  VectorElementTree() {
  };
  void print_tree() {
    Tree_ElementPtr::iterator sib2 = element_tree.begin();
    Tree_ElementPtr::iterator end2 = element_tree.end();
    while(sib2!=end2) {
      cout << "Depth: " << element_tree.depth(sib2) << endl;

      for(int i = 0; i < element_tree.depth(sib2); ++i)
        cout << " ";
      sib2->print();
      ++sib2;
    }
  }

  void createAndSortTree(ParsedSVG * ps) {
    // Here we have the list of all SVG elements
    // There is no CGAL representation of Polylines
    // Polylines are std::list<Point_2>
    cout << "Creating and sorting tree" << endl;
    cout << "Looping" << element_tree.size() << endl;
    Tree_ElementPtr::iterator top = element_tree.begin();
    playfield = new PolygonElementPtr(Polygon_2());
    playfield->print();
    element_tree.insert(top, *playfield); // Parent of all`
    for(VectorElement ve : ps->elements) {
      ElementPtr * elem_ptr = this->getElementRepresentation(&ve);
      // find parent tree iter if possible
      findSpot(elem_ptr);
      cout << "Looping" << element_tree.size() << endl;
    }
    this->print_tree();
  };
};
