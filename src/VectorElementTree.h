// This class defines possible elements for the tree and 
// a factory ?

#pragma once

#include "SVGParserAdapter.h"
#include "CustomPolylinesGraphicsItem.h"
#include "tree.h"
#include <iterator>

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

class VectorElementTree {
public:
  typedef tree < ElementPtr * > Tree_ElementPtr;

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
      else {
        // Check Polyline:
        // Only Interesting: First and last point!
        Point_2 start = static_cast<PolyLineElementPtr*>(tested_elem)->element.front();
        Point_2 end = static_cast<PolyLineElementPtr*>(tested_elem)->element.back();
        Polygon_2 * test_hull = elem->convexHull();
        int start_bounded = CGAL::bounded_side_2(
          test_hull->vertices_begin(),
          test_hull->vertices_end(), start);
        switch(start_bounded) {
          case CGAL::ON_BOUNDED_SIDE:
          case CGAL::ON_BOUNDARY:
            cout << "Inside: " << elem << endl;
            return true;
            break;
        }
        int end_bounded = CGAL::bounded_side_2(
          test_hull->vertices_begin(),
          test_hull->vertices_end(), end);
        switch(end_bounded) {
          case CGAL::ON_BOUNDED_SIDE:
          case CGAL::ON_BOUNDARY:
            cout << "Inside: " << elem << endl;
            return true;
            break;
        }
        return false;
        delete test_hull;
      }

      // it->skip_children(); (skips children for one increment)
      //it->convexHull().onBounded
      return false;
    }
  }
  Tree_ElementPtr::iterator findSpot(ElementPtr * elem_ptr) {
    // if(element_tree.size() == 1) {

    // }  // return top elem if empty tree
    Tree_ElementPtr::breadth_first_iterator it = ++element_tree.begin_breadth_first(); // skip playfield
    Tree_ElementPtr::breadth_first_iterator it_end = element_tree.end_breadth_first();
    Tree_ElementPtr::iterator curr_parent = element_tree.begin();
    elem_ptr->print();
    // cout << "Length: " << it - it_end << endl;
    for(; it != it_end; ++it) {
      cout << "Checking all polys" <<  endl;
      (*it)->print();

        if(it == curr_parent) {continue;} // check if parent is playfield
        if(isInside((*it), elem_ptr)) {
          curr_parent = it;
          cout << "Curr Parent " << (*curr_parent) << endl;
          if(it.node->first_child) {
            // it = Tree_ElementPtr::breadth_first_iterator(it.node->first_child);
          } else {
            break; // end of tree
          }
        } else {
          // it.skip_children();
        }
    }
    // found some parent?
    Tree_ElementPtr::iterator new_element_iter;

    cout << "Inserting Element at " << (*curr_parent) << endl;
    new_element_iter = element_tree.append_child(curr_parent, elem_ptr);
    // iterate over siblings: are they children of the new node?
    // If yes: reparent!
    Tree_ElementPtr::sibling_iterator sit = element_tree.begin(curr_parent);
    Tree_ElementPtr::sibling_iterator sit_end = element_tree.end(curr_parent);
    // for(; sit != sit_end; ++sit) {
    //   if((*sit) == elem_ptr) { continue; }
    //   if(isInside(elem_ptr, (*sit))) {
    //     // reparent subtree
    //     cout << "reparenting" << endl;
    //     Tree_ElementPtr::iterator it = (Tree_ElementPtr::iterator) sit;
    //     element_tree.append_child(new_element_iter, it);
    //     element_tree.erase(sit);
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
  PolyLineElementPtr * playfield;
  VectorElementTree() {
  };
  void print_tree() {
    Tree_ElementPtr::iterator sib2 = element_tree.begin();
    Tree_ElementPtr::iterator end2 = element_tree.end();
    while(sib2!=end2) {
      for(int i = 0; i < element_tree.depth(sib2); ++i)
        cout << " ";
      cout << (*sib2);
      // (*sib2)->print();
      cout << endl;
      ++sib2;
    }
  }
#ifdef WITH_GUI
  View * window;

  void addWindow(View * window) {
    this->window = window;
  }
  void drawTreeOnCanvas() {
    Tree_ElementPtr::iterator it = element_tree.begin();
    Tree_ElementPtr::iterator it_end = element_tree.end();
    for(; it != it_end; ++it) {
      (*it)->setColorFromDepth(element_tree.depth(it));
      char text[10];
      sprintf(text, "D: %d",element_tree.depth(it));
      switch((*it)->get_type()){
        case EL_FILLED_POLYGON:
          static_cast<FilledPolygonElementPtr * >(*it)->set_graphx();
          window->addItem(static_cast<FilledPolygonElementPtr * >(*it)->graphx);
          if(static_cast<FilledPolygonElementPtr * >(*it)->element.outer_boundary().size() > 0) {
            window->addText(text,
              static_cast<FilledPolygonElementPtr * >(*it)->getFromIndex(0).x(),
              static_cast<FilledPolygonElementPtr * >(*it)->getFromIndex(0).y());
          }
          break;
        case EL_POLYGON:
          static_cast<PolygonElementPtr * >(*it)->set_graphx();
          window->addItem(static_cast<PolygonElementPtr * >(*it)->graphx);
          if(static_cast<PolygonElementPtr * >(*it)->element.size() > 0) {
            window->addText(text,
              static_cast<PolygonElementPtr * >(*it)->getFromIndex(0).x(),
              static_cast<PolygonElementPtr * >(*it)->getFromIndex(0).y());
          }
          break;
        case EL_POLYLINE:
          static_cast<PolyLineElementPtr * >(*it)->set_graphx();
          window->addItem(static_cast<PolyLineElementPtr * >(*it)->graphx);
          if(static_cast<PolyLineElementPtr * >(*it)->element.size() > 0) {
            window->addText(text,
              static_cast<PolyLineElementPtr * >(*it)->getFromIndex(0).x(),
              static_cast<PolyLineElementPtr * >(*it)->getFromIndex(0).y());
          }

          break;
      }
    }
  }
  int addLine(Point_2 from, Point_2 to, std::list<std::list<Point_2> > * lines_list) {
    std::list<Point_2> list;// = std::list<Point_2>();
    list.push_back(from);
    list.push_back(to);
    lines_list->push_back(list);
    return 0; // TODO Change return stuff
  }
  PolylinesGraphicsI * connect_lines_gi;
  std::list<std::list<Point_2> > connect_lines;

  void drawConnections() {
    connect_lines_gi = new PolylinesGraphicsI(&connect_lines);
    connect_lines_gi->setEdgesPen(QPen(QColor(255,0,0), 2));
    window->addItem(connect_lines_gi);
    connect_lines_gi->show();
    Tree_ElementPtr::iterator it = ++element_tree.begin();
    Tree_ElementPtr::iterator it_end = element_tree.end();

    for(; it != it_end; ++it) {
      if((*it)->from == NULL) {
        break; // it = start iterator
      }
    }
    ElementPtr * elem = (*it);
    while(elem->to != NULL) {
      cout << elem->exit_point.x() << " " << elem->exit_point.y() << " -> " << elem->to->entry_point.x() << " " << elem->to->entry_point.y() << endl;
      this->addLine(elem->exit_point, elem->to->entry_point, &connect_lines);
      elem = elem->to;
    }
    connect_lines_gi->modelChanged();
  }
#endif
  void createAndSortTree(ParsedSVG * ps) {
    // Here we have the list of all SVG elements
    // There is no CGAL representation of Polylines
    // Polylines are std::list<Point_2>
    cout << "Creating and sorting tree" << endl;
    cout << "Looping" << element_tree.size() << endl;
    Tree_ElementPtr::iterator top = element_tree.begin();
    std::list<Point_2> playfield_list;
    playfield_list.push_back(Point_2(0, 0));
    playfield = new PolyLineElementPtr(playfield_list);
    playfield->print();
    element_tree.insert(top, playfield); // Parent of all`
    for(VectorElement ve : ps->elements) {
      ElementPtr * elem_ptr = this->getElementRepresentation(&ve);
      // find parent tree iter if possible
      elem_ptr->print();
      findSpot(elem_ptr);
      cout << "Looping" << element_tree.size() << endl;
    }
    this->print_tree();
  };

};
