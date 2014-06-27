// Sophisticated Connector
#pragma once

#include "SimpleConnector.h"
#include <limits>
#include <fstream>
#include <sstream>
#include <string>

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

class TourCity {
// For keeping reference of order of traversal
public:
  TourCity(ElementPtr * e, int node_index):
    element(e), node_index(node_index) {}
  int node_index;
  ElementPtr * element;
};


typedef std::vector<ElementPtr *> ElementVector;
typedef std::vector<TourElement *> Tour;

class TSPConnector : public Connector {
private:
  float curr_min;
  int path_size;
  VectorElementTree * vet;
  Tour best_tour;
  std::vector<TourCity> city_order;
  // vector<TourElement *> current_tour;
  void recursive_check(TourElement * from, TourElement * next, 
                       const ElementVector & unvisited_elems, Tour current_tour, 
                       float length, int level);
  vector<int> create_distance_matrix_row(Tree_ElementPtr::iterator row_it, 
                                                int element_index, int column_index);
  bool isChild(Tree_ElementPtr::iterator it, Tree_ElementPtr::iterator maybe_child);
public:
  void connect();
  void connect(std::vector<FilledSegment>::iterator begin, std::vector<FilledSegment>::iterator end);
  void connect_segments();
  void create_distance_matrix();
  void solve_with_LKH(vector< vector<int> > m);
  void project_result_to_tree(std::vector<int> solution);
  vector<int> create_startpoint_distance_matrix_row(Point_2 sp);
  TSPConnector();
  TSPConnector(VectorElementTree * vet);
};