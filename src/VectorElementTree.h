// This class defines possible elements for the tree and 
// a factory ?

#pragma once

#include "SVGParserAdapter.h"
#include "tree.h"
#include <iterator>

#include "FillProcedures.h"
#include "CGAL_Headers.h"

#include "VectorTreeElements.h"

#ifdef WITH_GUI
#include <QColor>
#include <QPen>
#endif

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
  void insertIntoTree(ElementPtr * elem) {
      this->findSpot(elem);
  };
  void fillPolys() {
    Tree_ElementPtr::iterator it = element_tree.begin();
    Tree_ElementPtr::iterator it_end = element_tree.end();
    SpiralFillProcedure * spiral_singleton = &SpiralFillProcedure::getInstance();
    for(; it != it_end; ++it) {
      if((*it)->get_type() == EL_FILLED_POLYGON) {
        FilledPolygonElementPtr * el_ptr = static_cast<FilledPolygonElementPtr *>((*it));
        el_ptr->fill_elements = spiral_singleton->fill(&(el_ptr->element));
        for(ElementPtr * e : el_ptr->fill_elements) {
          findSpot(e);
        }
      }
    }
  }
};
