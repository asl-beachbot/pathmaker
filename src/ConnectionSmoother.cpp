// ConnectionSmoother.cpp

#include "ConnectionSmoother.h"

std::vector<BezierCP> ConnectionSmoother::getBezierCPS(
  Point_2 p1,
  Direction_2 d1,
  Point_2 p2,
  Direction_2 d2) {
  std::vector<spiro_cp> spiro_cps;
  spiro_cps.push_back({p1.x(), p1.y(), '{'});
  auto pm1 = p1 + d1.vector() * 50;
  spiro_cps.push_back({pm1.x(), pm1.y(), '['});
    
  auto pm2 = p2 - d2.vector() * 50;
  spiro_cps.push_back({pm2.x(), pm2.y(), ']'});

  spiro_cps.push_back({p2.x(), p2.y(), '}'});
  std::vector<BezierCP> v;
  v = s.TaggedSpiroCPsToBezier(spiro_cps);
  for(auto b : v) {
    cout << "Got a bezier: " << b.in << " " << b.control_point << " " << b.out << endl;
  }
  return v;
}

void ConnectionSmoother::smooth() {
  VectorElementTree::Tree_ElementPtr * element_tree = &(tree->element_tree);
  // Find entry point
  auto it = ++element_tree->begin();
  auto it_end = element_tree->end();
  for(; it != it_end; ++it) {
    if((*it)->from == nullptr) {
      break;
    }
  }
  ElementPtr * elem = (*it);
  while(elem->to != nullptr) {
    Point_2 p1 = elem->getFromIndex(elem->exit_point_index);
    Point_2 p1_t = elem->getFromIndex(elem->exit_point_index - 1);
    Point_2 p2 = elem->to->getFromIndex(elem->to->entry_point_index);
    Point_2 p2_t = elem->to->getFromIndex(elem->to->exit_point_index - 1);
    elem->smooth_connection = getBezierCPS(p1, Direction_2(p1 - p1_t), p2, Direction_2(p2 - p2_t));
    elem = elem->to;
  }

}