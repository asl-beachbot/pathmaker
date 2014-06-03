// Sophisticated Connector
#include "SimpleConnector.h"


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
  double curr_min;
  int path_size;
  Tour best_tour;
  // vector<TourElement *> current_tour;
  double curr_dist;
  void recursive_check(TourElement * from, TourElement * next, 
    const ElementVector & unvisited_elems, Tour current_tour, float length, int level);
public:
  void connect();
  TSPConnector(VectorElementTree * vet) : Connector(vet) {
    curr_min = 99999999;
    path_size = vet->element_tree.size() - 1;
  };
};

void TSPConnector::recursive_check(TourElement * from, TourElement * next, 
  const ElementVector & unvisited_elems, Tour current_tour, 
  float length, int level) {
  // auto f = static_cast<PolylineElementPtr * > from;
  // auto n = static_cast<PolylineElementPtr * > next;
  level += 1;
  current_tour.push_back(next);
  if(from) length += CGAL::squared_distance(from->exit_point, next->entry_point);
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
    recursive_check(nullptr, new TourElement(unvisited_elems[i], 
      unvisited_elems[i]->getFromIndex(0), 
      unvisited_elems[i]->getFromIndex(-1)), copy, Tour{}, 0, 0);
    recursive_check(nullptr, new TourElement(unvisited_elems[i], 
      unvisited_elems[i]->getFromIndex(-1), 
      unvisited_elems[i]->getFromIndex(0)), copy, Tour{}, 0, 0);
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