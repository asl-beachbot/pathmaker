// ConnectionSmoother.cpp

#include "ConnectionSmoother.h"

std::vector<BezierCP> ConnectionSmoother::getBezierCPS(
  Point_2 p1,
  Vector_2 d1,
  Point_2 p2,
  Vector_2 d2) {
  std::vector<spiro_cp> spiro_cps;

  cout << "Deriving Control Points for Connection from: " << endl
    << p1 << " D1: " << d1 << endl
    << p2 << " D2: " << d2 << endl;
   spiro_cp points[4];

   // points[0].x = -100; points[0].y =    0; points[0].ty = '{';
   // points[1].x =    0; points[1].y =  100; points[1].ty = 'o';
   // points[2].x =  100; points[2].y =    0; points[2].ty = 'o';
   // points[3].x =    0; points[3].y = -100; points[3].ty = '}';
   // for(auto p : points) {
   //  spiro_cps.push_back({p.x, p.y, p.ty});
   // }
  spiro_cps.push_back({p1.x(), p1.y(), '{'});
  auto pm1 = p1 + d1 * 0.2;
  spiro_cps.push_back({pm1.x(), pm1.y(), ']'});
  auto pm11 = p1 + d1 * 5;
  spiro_cps.push_back({pm11.x(), pm11.y(), 'c'});

  auto pm22 = p2 + d2 * 5;
  spiro_cps.push_back({pm22.x(), pm22.y(), 'c'});
  auto pm2 = p2 + d2 * 0.2;
  spiro_cps.push_back({pm2.x(), pm2.y(), '['});

  spiro_cps.push_back({p2.x(), p2.y(), '}'});
  cout << "Spiro CP: ";
  cout << "(plate " << endl;
  for(auto cp : spiro_cps) {
    cout << "   (" << cp.ty << " " << cp.x * 100 << " " << cp.y * 100<< ")" << endl;
  }
  cout << ")" << endl;


  std::vector<BezierCP> v;
  v = s.TaggedSpiroCPsToBezier(spiro_cps);
  for(auto b : v) {
    cout << "Got a bezier: " << b.in << " " << b.control_point << " " << b.out << endl;
  }
  return v;
}
Point_2 ConnectionSmoother::pointBefore(ElementPtr * e) {
  Point_2 p;
  switch(e->get_type()) {
    case EL_POLYLINE:
      if(e->entry_point_index == 0) p = e->getFromIndex(1);
      else p = e->getFromIndex(-2);
    break;
    case EL_POLYGON:
    case EL_FILLED_POLYGON:
      p = e->getFromIndex(e->entry_point_index + 1);
  }
  return p;
}

Point_2 ConnectionSmoother::pointAfter(ElementPtr * e) {
  Point_2 p;
  switch(e->get_type()) {
    case EL_POLYLINE:
      if(e->exit_point_index == 0) p = e->getFromIndex(1);
      else p = e->getFromIndex(-2);
    break;
    case EL_POLYGON:
    case EL_FILLED_POLYGON:
      p = e->getFromIndex(e->exit_point_index - 1);
  }
  return p;
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
  while(elem->to != nullptr && elem->to != *element_tree->begin()) {
    cout << elem << " " << elem->to << endl;

      Point_2 p1 = elem->exit_point;
      Point_2 p2 = elem->to->entry_point;

      Point_2 p1_after = pointAfter(elem);
      Point_2 p2_before = pointBefore(elem->to);

      Vector_2 d1 = (p1 - p1_after) / sqrt((p1_after - p1).squared_length());
    Vector_2 d2 = (p2 - p2_before) / sqrt((p2_before - p2).squared_length());

    elem->smooth_connection = getBezierCPS(p1, d1, p2, d2);
    elem = elem->to;
  }

}