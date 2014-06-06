// Sophisticated Connector
#pragma once

#include "SimpleConnector.h"
#include <limits>

// ok, since this is taking way too long 
// we'll optimize it using the LKH implementation
// So

// Define full matrix:
// ATSP problem
// every polygon edge will be zero
// every polyline edge will be zero
// and back from the starting point will be equal weight and very high
// thus it will not optimize for the return way (it's equal from every point)

class TourElement {
public:
  TourElement(ElementPtr * el, Point_2 p_entry, Point_2 p_exit) :
    elem(el),
    entry_point(p_entry),
    exit_point(p_exit)
  {}
  ElementPtr * elem;
  Point_2 entry_point;
  Point_2 exit_point;
};

typedef std::vector<ElementPtr *> ElementVector;
typedef std::vector<TourElement *> Tour;

class TSPConnector : public Connector {
private:
  float curr_min;
  int path_size;
  Tour best_tour;
  // vector<TourElement *> current_tour;
  void recursive_check(TourElement * from, TourElement * next, 
                       const ElementVector & unvisited_elems, Tour current_tour, 
                       float length, int level);
  vector<int> create_distance_matrix_row(Tree_ElementPtr::iterator row_it, 
                                                int element_index, int column_index);

public:
  void connect();
  void connect(std::vector<FilledSegment>::iterator begin, std::vector<FilledSegment>::iterator end);
  void connect_segments();
  void create_distance_matrix();
  TSPConnector();
  TSPConnector(VectorElementTree * vet) : Connector(vet) {
    curr_min = 9999999999;
    cout << "curr min " << curr_min << endl;
    path_size = vet->element_tree.size() - 1;
  };
};

void TSPConnector::recursive_check(TourElement * from, TourElement * next, 
  const ElementVector & unvisited_elems, Tour current_tour, 
  float length, int level) {
  level += 1;
  current_tour.push_back(next);
  if(from) length += CGAL::squared_distance(from->exit_point, next->entry_point);
  cout << "length: " << length << " curr_min " << curr_min << " " << level << " " << path_size << endl;
  if(length >= curr_min) {
    return;
  }
  if(level == path_size) {
    // We have a new best path, set as best path
    best_tour = current_tour;
    curr_min = length;
    cout << "Current Min: " << curr_min << endl;
    return;
  }
  // append current next element to tour
  for(int i = 0; i < unvisited_elems.size(); ++i) {
    ElementVector copy = unvisited_elems;
    copy.erase(copy.begin() + i);
    // decide action depending on act unvisited elem
    if(unvisited_elems[i]->get_type() == EL_POLYLINE) {
      recursive_check(next, 
        new TourElement(unvisited_elems[i], unvisited_elems[i]->getFromIndex(0), unvisited_elems[i]->getFromIndex(-1)), 
        copy, current_tour, length, level);
      recursive_check(next, 
        new TourElement(unvisited_elems[i], unvisited_elems[i]->getFromIndex(-1), unvisited_elems[i]->getFromIndex(0)), 
        copy, current_tour, length, level);
    } else {
      for(int j = 0; j < unvisited_elems[i]->getSize(); ++j) {
        recursive_check(next, new TourElement(unvisited_elems[i], 
          unvisited_elems[i]->getFromIndex(j), 
          unvisited_elems[i]->getFromIndex(j)), copy, current_tour, length, level);
      }
    }
  }
}

vector<int> TSPConnector::create_distance_matrix_row(Tree_ElementPtr::iterator row_it, 
                                              int element_index, int column_index) {
  auto it = ++element_tree->begin();
  auto it_end = element_tree->end();
  Point_2 current_point = (*row_it)->getFromIndex(element_index);
  vector<int> row;
  for(; it != it_end; ++it) {
    if(*it != *row_it) {
      cout << "Equal elements" << endl;
      if((*it)->get_type() == EL_POLYLINE) {
        row.push_back(ceil(CGAL::squared_distance(current_point, (*it)->getFromIndex(0))));
        row.push_back(ceil(CGAL::squared_distance(current_point, (*it)->getFromIndex(-1))));
      } else {
        for(int i = 0; i < (*it)->getSize(); i++) {
          row.push_back(ceil(CGAL::squared_distance(current_point, (*it)->getFromIndex(i))));
        }
      }
    } else {
      // we're on the same element now! 
      // special properties?!
      if((*it)->get_type() == EL_POLYLINE) {
        if(element_index == 0) {
          row.push_back(9999999);
          row.push_back(0); // zero distance to travel to next point
        } else {
          row.push_back(0);
          row.push_back(9999999); // zero distance to travel to next point          
        }
      } else {
        for(int i = 0; i < (*it)->getSize(); i++) {
          if(i == (element_index + 1) % (*it)->getSize()) {
            row.push_back(0); // cycle through
          } else {
            row.push_back(9999999); // never jump around
          }
        }
      }
    }
  }
  return row;
}

void TSPConnector::create_distance_matrix() {
  // 2 dimensional iteration through the complete tree
  // always calculating the squared distance between all possible cities

  auto it = ++element_tree->begin();
  auto it_end = element_tree->end();
  // float[][] result = new float[]
  vector< vector<int> > matrix;
  int act_col = 0;
  for(; it != it_end; ++it) {
    if((*it)->get_type() == EL_POLYLINE) {
      matrix.push_back(create_distance_matrix_row(it, 0, act_col));
      act_col++;
      matrix.push_back(create_distance_matrix_row(it, -1, act_col));
      act_col++;
    } else {
      for(int i = 0; i < (*it)->getSize(); i++) {
        matrix.push_back(create_distance_matrix_row(it, i, act_col));
        act_col++;
      }
    }
  }

  cout << "Die Matrix: " << endl;
  for(vector<int> v : matrix) {
    for(int d : v) {
      cout << d << " ";
    }
    cout << endl;
  }
}

void TSPConnector::connect(std::vector<FilledSegment>::iterator begin, std::vector<FilledSegment>::iterator end) {
  path_size = std::distance(begin, end);
  curr_min = 9999999999;

  ElementVector unvisited_elems;
  for(auto it = begin; it != end; ++it) {
    unvisited_elems.push_back(&(*it));
  }
  for(int i = 0; i < unvisited_elems.size(); ++i) { // check all different start point options
    ElementVector copy = unvisited_elems;
    copy.erase(copy.begin() + i);
    if(unvisited_elems[i]->get_type() == EL_POLYLINE) {
      recursive_check(nullptr, new TourElement(unvisited_elems[i], 
        unvisited_elems[i]->getFromIndex(0), 
        unvisited_elems[i]->getFromIndex(-1)), copy, Tour{}, 0, 0);
      recursive_check(nullptr, new TourElement(unvisited_elems[i], 
        unvisited_elems[i]->getFromIndex(-1), 
        unvisited_elems[i]->getFromIndex(0)), copy, Tour{}, 0, 0);      
    }
    else {
      for(int j = 0; j < unvisited_elems[i]->getSize(); ++j) {
        cout << "iterating poly" << j << endl;
        recursive_check(nullptr, new TourElement(unvisited_elems[i], 
          unvisited_elems[i]->getFromIndex(j), 
          unvisited_elems[i]->getFromIndex(j)), copy, Tour{}, 0, 0);
      }
    }
  }
  for(int i = 0; i < best_tour.size(); ++i) {
    auto tour_el = best_tour[i];
    if(i < best_tour.size() - 1) {
      tour_el->elem->to = best_tour[i + 1]->elem;
    }
    if(i > 0) {
      tour_el->elem->from = best_tour[i - 1]->elem;
    }
    tour_el->elem->entry_point = tour_el->entry_point;
    tour_el->elem->exit_point = tour_el->exit_point;
    cout << "entry_point " << tour_el->entry_point << " exit_point " << tour_el->exit_point << endl;
    tour_el->elem->visited = true;
  }

}
void TSPConnector::connect_segments() {
  for(auto it = ++element_tree->begin(); it != element_tree->end(); ++it) {
    if((*it)->get_type() == EL_FILLED_POLYGON) {
      FilledPolygonElementPtr * ptr = static_cast<FilledPolygonElementPtr *>(*it);
      if(ptr->segments.size()) {
        connect(ptr->segments.begin(), ptr->segments.end());
      }  
    } 
  }
}

void TSPConnector::connect() {
  ElementVector unvisited_elems;
  // flatten tree
  for(auto it = ++element_tree->begin(); it != element_tree->end(); ++it) {
    unvisited_elems.push_back(*it);
  }
  for(int i = 0; i < unvisited_elems.size(); ++i) { // check all different start point options
    ElementVector copy = unvisited_elems;
    copy.erase(copy.begin() + i);
    if(unvisited_elems[i]->get_type() == EL_POLYLINE) {
      recursive_check(nullptr, new TourElement(unvisited_elems[i], 
        unvisited_elems[i]->getFromIndex(0), 
        unvisited_elems[i]->getFromIndex(-1)), copy, Tour{}, 0, 0);
      recursive_check(nullptr, new TourElement(unvisited_elems[i], 
        unvisited_elems[i]->getFromIndex(-1), 
        unvisited_elems[i]->getFromIndex(0)), copy, Tour{}, 0, 0);      
    }
    else {
      for(int j = 0; j < unvisited_elems[i]->getSize(); ++j) {
        cout << "iterating poly" << j << endl;
        recursive_check(nullptr, new TourElement(unvisited_elems[i], 
          unvisited_elems[i]->getFromIndex(j), 
          unvisited_elems[i]->getFromIndex(j)), copy, Tour{}, 0, 0);
      }
    }
  }

  // project the found connections on the tree...

  for(int i = 0; i < best_tour.size(); ++i) {
    auto tour_el = best_tour[i];
    if(i < best_tour.size() - 1) {
      tour_el->elem->to = best_tour[i + 1]->elem;
    }
    if(i > 0) {
      tour_el->elem->from = best_tour[i - 1]->elem;
    }
    tour_el->elem->entry_point = tour_el->entry_point;
    tour_el->elem->exit_point = tour_el->exit_point;
    cout << "entry_point " << tour_el->entry_point << " exit_point " << tour_el->exit_point << endl;
    tour_el->elem->visited = true;
  }
  for(auto it = ++element_tree->begin(); it != element_tree->end(); ++it) {
    cout << (*it)->to << " -> " << (*it)->from << " endl" << endl;
  }

}