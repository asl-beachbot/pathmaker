// Connector Implementation

#include "VectorElementTree.h"
#include "CGAL_Headers.h"

typedef  VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef  VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef  VectorElementTree Tree;

class Connector {
protected:
  Tree * tree;
  Tree_ElementPtr * element_tree;
public:
	virtual void connect () = 0;
};

class SimpleConnector : public Connector {
private:
  Point_2 find_closest_point_on_element(Point_2 exit_point, ElementPtr * p2, bool connect) {
    if(p2 == *element_tree->begin()) {return Point_2(0, 0);}
    p2->entry_point_index = 1; // fix...

    if(p2->get_type() != EL_POLYLINE) {
      float distance, temp_dist;
      Polygon_2::Vertex_const_iterator p2_begin, p2_end;
      if(p2->get_type() == EL_POLYGON) {
        p2_begin = static_cast<PolygonElementPtr * >(p2)->element.vertices_begin();
        p2_end = static_cast<PolygonElementPtr * >(p2)->element.vertices_end();
      } else {
        p2_begin = static_cast<FilledPolygonElementPtr * >(p2)->element.outer_boundary().vertices_begin();
        p2_end = static_cast<FilledPolygonElementPtr * >(p2)->element.outer_boundary().vertices_end();
      }
      Point_2 point2 = *p2_begin;
      distance = CGAL::squared_distance(exit_point, *p2_begin);
      for(;p2_begin != p2_end; ++p2_begin) {
        temp_dist = CGAL::squared_distance(exit_point, *p2_begin);
        if(temp_dist < distance) {
          distance = temp_dist;
          point2 = *p2_begin;
          // *entry_point_index = std::distance(p2_begin, p2->vertices_begin());
          //entry_point_index = new int(1);
        }
      }
      if(connect) {
        p2->entry_point = point2;
        p2->exit_point = point2;
      }
      return point2;
    }
    else {
        // Only check front and back!
        Point_2 p2_begin = static_cast<PolyLineElementPtr * >(p2)->element.front();
        Point_2 p2_end = static_cast<PolyLineElementPtr * >(p2)->element.back();
        float distance_begin = CGAL::squared_distance(exit_point, p2_begin);
        float distance_end = CGAL::squared_distance(exit_point, p2_end);
        if(distance_begin > distance_end) {
          if(connect) {
            p2->entry_point = p2_end;
            p2->exit_point = p2_begin;
          }
          return p2_end;
        }
        else {
          if(connect) {
            p2->entry_point = p2_begin;
            p2->exit_point = p2_end;
          }
          return p2_begin;
        }
    }

  }
  int connect_recursive(TreeIterator node, TreeIterator connect_from) {
    // Recursive algorithm to connect all Polygons
    // Steps:
    // 1. Find a "kernel" from where the BeachBot ideally should start
    // 2. Drive kernel, then move on one stage further out
    // 3. Check number of "undriven" polygons inside the one we're at
    // 4. If more than 0 find the closest point to enter (from where we are)
    //    Entering should happen while driving on the polygon where at
    // 5. Drive inside again and repeat step 1 - 5

    // There are no "half-driven" Polygons at the moment
    // No state keeping etcs
    cout << "Working on connection" << endl;
    Tree_ElementPtr::sibling_iterator child_it = this->element_tree->child(node, 0);
    if(child_it == this->element_tree->end()) { // invalid pointer == no children
      (*connect_from)->to = (*node);
      (*node)->from = (*connect_from);
      cout << "Connecting " << *connect_from << " to " << *node << endl;
      (*node)->visited = true;

      // this->addLine(node->poly[0], connect_from->poly[0]);
      this->connect_recursive(this->element_tree->parent(node), node);
      int entry_point_index;
      Point_2 next_entry = this->find_closest_point_on_element((*connect_from)->exit_point, (*node), true);
      (*node)->entry_point = next_entry;
      (*node)->entry_point_index = entry_point_index;

      // this->addLine(next_entry, (*connect_from)->entry_point, &this->poly_connector_lines);

      return 1;
    }
    for(;child_it != child_it.end();++child_it) {
      cout << "child visited?" << endl;
      Tree_ElementPtr::sibling_iterator fn_it = this->element_tree->child(node, 0);
      Tree_ElementPtr::sibling_iterator fn_it_end = element_tree->end(fn_it);
      Point_2 closest_point;
      float cur_dist = 1000000;
      Tree_ElementPtr::sibling_iterator next_element = child_it;

      if(!(*child_it)->visited) {
        cout << "unvisited child !" << endl;
        for(; fn_it != fn_it_end; ++fn_it) {
          cout << "checking for nearest" ;
          if((*fn_it)->visited) {continue;}
          closest_point = this->find_closest_point_on_element((*connect_from)->exit_point, (*fn_it), false);
          cout << "Cur dist " << CGAL::squared_distance((*connect_from)->exit_point, closest_point) << endl;
          if(CGAL::squared_distance((*connect_from)->exit_point, closest_point) < cur_dist) {
            cur_dist = CGAL::squared_distance((*connect_from)->exit_point, closest_point);
            cout << cur_dist << " as " << *next_element << endl;
            next_element = fn_it;
          }
          cout << *next_element << " from " << *connect_from << endl;
        }
        this->connect_recursive(next_element, connect_from);
        return 1;
      }
    }
    // Always starting at a kernel
    // first one is NULL
    if(connect_from != NULL) {
      (*connect_from)->to = *node;
      (*node)->from = *connect_from;
      int entry_point_index;
      cout << "Connecting " << *connect_from << " to " << *node << endl;
      Point_2 next_entry = this->find_closest_point_on_element((*connect_from)->entry_point, (*node), true);
      // (*node)->entry_point = next_entry;
      // (*node)->entry_point_index = entry_point_index;

      //Line_2 line = Line_2(connect_from->entry_point, next_entry);
      // this->checkPolyIntersection(line);

      // this->addLine(next_entry, connect_from->entry_point, &this->poly_connector_lines);

      // this->addLine(node->poly[0], connect_from->poly[0]);
    } else {
      // first entry point randomly on polygon
      (*node)->from = NULL;
      switch((*node)->get_type()) {
        case EL_POLYGON:
          (*node)->entry_point = static_cast<PolygonElementPtr *>(*node)->element[0];
          (*node)->exit_point = (*node)->entry_point;
          break;
        case EL_FILLED_POLYGON:
          (*node)->entry_point = static_cast<FilledPolygonElementPtr *>(*node)->element.outer_boundary()[0];
          (*node)->exit_point = (*node)->entry_point;
          break;
        case EL_POLYLINE:
          (*node)->entry_point = static_cast<PolyLineElementPtr *>(*node)->element.front();
          (*node)->exit_point = static_cast<PolyLineElementPtr *>(*node)->element.back();

          break;
      }
      (*node)->entry_point_index = 0;
    }
    cout << "Next Entry Index: " << (*node)->entry_point_index << " " << (*node)->entry_point << endl;
    (*node)->visited = true;
    // TODO check if this is correct (leave out first "playfield"!)
    if(node == this->element_tree->begin()) {
      (*node)->to = NULL;
      (*node)->from = NULL;
      return 1;
    } else {
      return this->connect_recursive(this->element_tree->parent(node), node);
    }
  }

public:
  SimpleConnector(VectorElementTree * vet) {
    this->tree = vet;
  }
	void connect() {
    element_tree = &(this->tree->element_tree);
    cout << (*element_tree->begin()) << endl;
    Tree_ElementPtr::post_order_iterator test_node = element_tree->begin_post();
    connect_recursive(test_node, NULL);
  }
};