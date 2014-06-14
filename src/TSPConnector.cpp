// TSPConnector.cpp

// Sophisticated Connector
#include "TSPConnector.h"

#include "CGAL_Headers.h"

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

TSPConnector::TSPConnector(VectorElementTree * vet) : Connector(vet) {
    curr_min = 9999999999;
    cout << "curr min " << curr_min << endl;
    path_size = vet->element_tree.size() - 1;
}


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

void TSPConnector::solve_with_LKH(vector< vector <int> > m) {
  // Write out matrix
  int size = m.size();
  ofstream ss_out("pathmaker_tsp.atsp");
  ss_out << "NAME: pathmaker_tsp" << endl;
  ss_out << "TYPE: ATSP" << endl;
  ss_out << "DIMENSION: " << size << endl;
  ss_out << "EDGE_WEIGHT_TYPE : EXPLICIT" << endl;
  ss_out << "EDGE_WEIGHT_FORMAT : FULL_MATRIX" << endl;
  ss_out << "EDGE_WEIGHT_SECTION" << endl;

  for(std::vector<int> v : m) {
    for(int m : v) {
      ss_out << m << " ";
    }
    ss_out << endl;
  }
  ss_out << "EOF" << endl;

  int status = system("./src/include/LKH-2.0.2/LKH pathmaker.par");
  if(status != EXIT_SUCCESS) {
    cout << "ERROR: TSP FAILED" << endl;
    return;
  }
  // read result file back in
  vector<int> tour;

  ifstream result("pathmaker_tour");
  std::string line;
  while (std::getline(result, line))
  {
    if(line.find_first_of(":") == std::string::npos) {
    // This should be a number
     if(line == "EOF") {
        // end of file reached
        break;
      }
      if(line == "-1") {
        break;
      }
      if(line == "TOUR_SECTION") continue;
      std::istringstream iss(line);
      int a;
      if (!(iss >> a)) { break; } // error
      else {
        tour.push_back(a);
      }
    }
  }
  cout << "Best Tour Found: " << endl;
  for(int t : tour) {
    cout << " " << t;
  }
  cout << endl;
  // erase start (it's not in the tree etc.)
  tour.erase(tour.begin() + 0);
  project_result_to_tree(tour);
}

void TSPConnector::project_result_to_tree(std::vector<int> solution) {
  // We have a result from the TSP 
  // ideally the LKH
  // Which is a list of cities in the order it 
  // should be traversed. Now it has to be projected
  // to our element tree

  cout << "Projecting result to tree" << endl;
  int start_traverse = 0;
  bool startpoint = false;
  int index_add = -2; // TODO: change
  if(startpoint) { 
    // startpoint has always index 0
    // which means that TSP will put it out as 1
    for(int i = 0; i < solution.size(); ++i) {
      if(solution[i] == 1) {
        start_traverse = i;
        break;
      }
    }
    index_add = -2;
  } else {
    // find first occurence of element thats not equal to the prev
    bool wraps = (city_order[solution.front() + index_add].element == 
                  city_order[solution.back() + index_add].element);
    if(wraps) {
      ElementPtr * prev_elem = nullptr;
      for(int i = 0; i < solution.size(); ++i) {
        int curr_ind = solution.at(i % solution.size()) + index_add;
        ElementPtr * curr_elem = city_order[curr_ind].element;
        if(prev_elem && curr_elem != prev_elem) {
          start_traverse = i;
        } else {
        }
      }      
    } else {
      start_traverse = 0;
    }
  }
  ElementPtr * prev_elem = nullptr;
  for(int i = start_traverse; i < solution.size() + start_traverse; ++i) {
    cout << "I: " << i << endl;
    // if no startpoint defined, just start anywhere
    int curr_ind = solution.at(i % solution.size()) + index_add;
    ElementPtr * curr_elem = city_order[curr_ind].element;
    // polygon / polyline changed?
    if(curr_elem != prev_elem && curr_elem->visited == false) {
      curr_elem->from = prev_elem;
      if(prev_elem) prev_elem->to = curr_elem;
      curr_elem->entry_point = curr_elem->getFromIndex(city_order[curr_ind].node_index);
      curr_elem->entry_point_index = city_order[curr_ind].node_index;
      if(curr_elem->get_type() == EL_POLYLINE) {
        if(curr_elem->entry_point_index == 0) {
          curr_elem->exit_point = curr_elem->getFromIndex(-1); // back is exit
          curr_elem->exit_point_index = -1;
        } else {
          curr_elem->exit_point = curr_elem->getFromIndex(0);
          curr_elem->exit_point_index = 0;
        }
      } else {
        curr_elem->exit_point = curr_elem->entry_point;
        curr_elem->exit_point_index = city_order[curr_ind].node_index;
      }
      if(curr_elem->enforced_connections.size() > 0) {
        cout << "Current El enf Cons size: " << curr_elem->enforced_connections.size() << endl;
        auto enforced_conn = curr_elem->enforced_connections[0];
        auto next_elem = enforced_conn.pair_partner;
        curr_elem->to = next_elem;
        curr_elem->exit_point_index = enforced_conn.index;
        curr_elem->exit_point = curr_elem->getFromIndex(enforced_conn.index);
        curr_elem->visited = true;
        auto prev_curr = curr_elem;
        while(next_elem->enforced_connections.size() == 2) {
          // connect these elements
          cout << "2 Enforced Connections " << endl;
          next_elem->from = prev_curr;
          
          ElementPtr::EnforcedConnection back, forward;
          if(next_elem->enforced_connections[0].pair_partner == prev_curr) {
            back = next_elem->enforced_connections[0];
            forward = next_elem->enforced_connections[1];
          } else {            
            back = next_elem->enforced_connections[1];
            forward = next_elem->enforced_connections[0];
          }

          cout << "Backward: " << back.pair_partner << " " << back.index << endl;
          cout << "Forward: " << forward.pair_partner << " " << forward.index << endl;

          next_elem->entry_point_index = back.index;
          next_elem->entry_point = next_elem->getFromIndex(back.index);

          next_elem->to = forward.pair_partner;
          next_elem->exit_point_index = forward.index;
          next_elem->exit_point = next_elem->getFromIndex(forward.index);
          next_elem->visited = true;

          prev_curr = next_elem;
          next_elem = forward.pair_partner;
        }
        next_elem->from = prev_curr;
        auto next_enforced_conn = next_elem->enforced_connections[0];
        next_elem->entry_point_index = next_enforced_conn.index;
        next_elem->entry_point = next_elem->getFromIndex(next_enforced_conn.index);
        if(next_elem->get_type() == EL_POLYLINE)
          next_elem->exit_point_index = next_enforced_conn.index == 0 ? -1 : 0;
        else 
          next_elem->exit_point_index = next_enforced_conn.index;
        next_elem->exit_point = next_elem->getFromIndex(next_elem->exit_point_index);
        next_elem->visited = true;
        curr_elem = next_elem;
      }
      curr_elem->visited = true;
      prev_elem = curr_elem;
    }
  }
}

bool elementIsReached(ElementPtr * el1, ElementPtr * el2) {
  auto ef1 = el1->enforced_connections[0];
  auto prev_el = el1;
  ElementPtr * next = ef1.pair_partner;
  cout << "El " << el1 << " Reaching: " << el2 << endl;
  cout << "Next: " << ef1.pair_partner << endl;
  int i = 0;
  while(i < 10000) {
    if(next->enforced_connections.size() == 2) {
      if(next->enforced_connections[0].pair_partner == prev_el)
        next = next->enforced_connections[1].pair_partner;
      else 
        next = next->enforced_connections[0].pair_partner;
      cout << "Next: " << next << endl;
    } else {
      // we reached an "end"
      if(next == el2) {
        cout << "Element Reached" << endl;
        return true;
      }
      cout << "Element Not Reached" << endl;
      return false;
    }
    i++; // this is an unnecessary cycle protection
  }
}

vector<int> TSPConnector::create_distance_matrix_row(Tree_ElementPtr::iterator row_it, 
                                              int element_index, int column_index) {
  vector<int> row;
  row.push_back(1); // push back weight for going back to the start
  cout << "\n\n";
  this->city_order.push_back(TourCity(*row_it, element_index));
  auto it = ++element_tree->begin();
  auto it_end = element_tree->end();
  Point_2 current_point;

  if((*row_it)->get_type() != EL_POLYLINE && (*row_it)->enforced_connections.size() == 0) {
    // This is the magic part, where the current city 
    // gets shifted for one! 
    current_point = (*row_it)->getFromIndex(element_index + 1);
  } else {
    current_point = (*row_it)->getFromIndex(element_index);
  }
  for(; it != it_end; ++it) {
    cout << *it << endl;
    cout << "Enforced Connections: " << (*it)->enforced_connections.size() << endl;
    for(auto e :  (*it)->enforced_connections) {
      cout << e.pair_partner << " " << e.index << endl;
    }
    cout << endl;
    if(*it != *row_it) {
      if((*it)->enforced_connections.size() == 1) {
        auto ef = (*it)->enforced_connections[0];
        if((*row_it)->enforced_connections.size() > 0 && elementIsReached(*row_it, *it)) {
          // these are like connected with a line == weight 0
          row.push_back(0);
        } else {    
          if((*it)->get_type() == EL_POLYLINE) {
            // there is just one point reachable
            if(ef.index == 0) row.push_back(ceil(sqrt(CGAL::squared_distance(current_point, (*it)->getFromIndex(-1))) * 100));
            else row.push_back(ceil(sqrt(CGAL::squared_distance(current_point, (*it)->getFromIndex(0))) * 100));
          } else {
            row.push_back(ceil(sqrt(CGAL::squared_distance(current_point, (*it)->getFromIndex(ef.index))) * 100));
          }
        }
      } else if((*it)->enforced_connections.size() == 2) {
        // do nothing! this city cannot be reached from the outside
      } else {
        if((*it)->get_type() == EL_POLYLINE) {
          row.push_back(ceil(sqrt(CGAL::squared_distance(current_point, (*it)->getFromIndex(0))) * 100));
          row.push_back(ceil(CGAL::squared_distance(current_point, (*it)->getFromIndex(-1)) * 100));
        } else {
          for(int i = 0; i < (*it)->getSize(); i++) {
            row.push_back(ceil(sqrt(CGAL::squared_distance(current_point, (*it)->getFromIndex(i))) * 100));
          }
        }
      }
    } else {
      // we're on the same element now! 
      // special properties?!
      if((*row_it)->enforced_connections.size() == 0) {
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
      } else {
        row.push_back(9999999);
      }
    }
  }
  return row;
}

vector<int> TSPConnector::create_startpoint_distance_matrix_row(Point_2 sp) {
  vector<int> row;
  row.push_back(9999999);
  for(auto it = ++element_tree->begin(); it != element_tree->end(); ++it) {
    if((*it)->enforced_connections.size() == 1) {
      auto ef = (*it)->enforced_connections[0];
      if((*it)->get_type() == EL_POLYLINE) {
        // there is just one point reachable
        if(ef.index == 0) row.push_back(ceil(sqrt(CGAL::squared_distance(sp, (*it)->getFromIndex(-1))) * 100));
        else row.push_back(ceil(sqrt(CGAL::squared_distance(sp, (*it)->getFromIndex(0))) * 100));
      } else {
        row.push_back(ceil(CGAL::squared_distance(sp, (*it)->getFromIndex(ef.index)) * 100));
      }
    } else if((*it)->enforced_connections.size() == 2) {
      // do nothing! this city cannot be reached from the outside
    } else {
      if((*it)->get_type() == EL_POLYLINE) {
        row.push_back(ceil(sqrt(CGAL::squared_distance(sp, (*it)->getFromIndex(0))) * 100));
        row.push_back(ceil(sqrt(CGAL::squared_distance(sp, (*it)->getFromIndex(-1))) * 100));
      } else {
        for(int i = 0; i < (*it)->getSize(); i++) {
          row.push_back(ceil(sqrt(CGAL::squared_distance(sp, (*it)->getFromIndex(i))) * 100));
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
  if(tree->startpoint_elem) {
    matrix.push_back(create_startpoint_distance_matrix_row(tree->startpoint_elem->getFromIndex(-1)));
  } else {
    matrix.push_back(create_startpoint_distance_matrix_row(Point_2(0,0)));
  }
  for(; it != it_end; ++it) {
    if((*it)->enforced_connections.size() == 1) {
      auto ef = (*it)->enforced_connections[0];
      if((*it)->get_type() == EL_POLYLINE) {
        // there is just one point reachable
        if(ef.index == 0) matrix.push_back(create_distance_matrix_row(it, -1, act_col));
        else matrix.push_back(create_distance_matrix_row(it, 0, act_col));
        act_col++;
      } else {
        matrix.push_back(create_distance_matrix_row(it, ef.index, act_col));
        act_col++;
      }
    } else if((*it)->enforced_connections.size() == 2) {
      // do nothing! this city cannot be reached from the outside
    } else {
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
  }

  cout << "The Matrix: " << endl;
  for(vector<int> v : matrix) {
    for(int d : v) {
      cout << d << " ";
    }
    cout << endl;
  }
  solve_with_LKH(matrix);
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
  // for(auto it = ++element_tree.begin(); it != element_tree.end(); ++it) {
  //   if()
  // }
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