// ConnectionSmoother.cpp

#include "ConnectionSmoother.h"
#include "GlobalOptions.h"

std::vector<BezierCP> ConnectionSmoother::getBezierCPS(
  Point_2 p1,
  Vector_2 d1,
  Point_2 p2,
  Vector_2 d2) {
  std::vector<spiro_cp> spiro_cps;
  double rr = GlobalOptions::getInstance().rounding_radius;
  cout << "Deriving Control Points for Connection from: " << endl
    << p1 << " D1: " << d1 << endl
    << p2 << " D2: " << d2 << endl;

  spiro_cps.push_back({p1.x(), p1.y(), '{'});
  auto pm1 = p1 + d1 * rr * 0.01;
  spiro_cps.push_back({pm1.x(), pm1.y(), ']'});


  auto dist_points = sqrt((p1 - p2).squared_length());

  auto conn_line = p1 - p2;
  auto conn_line_n = conn_line / sqrt(conn_line.squared_length());
  auto conn_angle_1 = acos(-d1 * conn_line_n);
  auto conn_angle_2 = acos(d2 * conn_line_n);
  auto vec_angle = acos(-d1 * d2);
  auto no_curve_points = false;

  if (conn_angle_1 < M_PI/5 && conn_angle_2 < M_PI/5 && vec_angle < M_PI/5)
    no_curve_points = true;

  auto pm11 = p1 + d1 * rr;
  auto pm22 = p2 + d2 * rr;
  
  auto d_cp = sqrt((pm11 - pm22).squared_length());

  cout << "conn_angle_1 " << conn_angle_1 << " conn_angle_2 " << conn_angle_2 << " vec_angle " << vec_angle << endl;

  if (conn_angle_1 < M_PI/5 && conn_angle_2 < M_PI/5 && vec_angle < M_PI/5) {
    no_curve_points = true;
  }
  else {
    if(d_cp < rr) {
      // edge case: control points to close
      // push outside
      cout << "Special Case 1: Control points too close!" << endl;

      auto conn_line = pm11 - pm22;
      Vector_2 conn_line_n;
      
      if(conn_line.squared_length() == 0)
        conn_line_n = d1.transform(Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2)));
      else 
        conn_line_n = conn_line / sqrt(conn_line.squared_length());

      auto mid_point = pm11 + ((pm22 - pm11) / 2);

      pm11 = mid_point + (conn_line_n * (rr));
      pm22 = mid_point - (conn_line_n * (rr));

      spiro_cps.push_back({pm11.x(), pm11.y(), 'c'});
      spiro_cps.push_back({pm22.x(), pm22.y(), 'c'});

      no_curve_points = true;
    }
    cout << vec_angle << " " << d_cp << " " << rr << " "  << conn_angle_1 << endl;
    if(vec_angle > 3 * M_PI / 5 && d_cp > rr && conn_angle_1 > 3 * M_PI / 5 && conn_angle_2 > 3 * M_PI / 5) {
      // Points to far away!
      // test ray to back for distance

      cout << "Special Case 2: Points too far away" << endl;
      auto conn_line = pm11 - pm22;
      Vector_2 conn_line_n;

      if(conn_line.squared_length() == 0)
        conn_line_n = d1.transform(Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2)));
      else 
        conn_line_n = conn_line / sqrt(conn_line.squared_length());

      auto mid_point = pm11 + (pm22 - pm11) / 2;
      pm11 = mid_point + (conn_line_n * (rr / 2));
      pm22 = mid_point - (conn_line_n * (rr / 2));

      if((pm11 - p1).squared_length() > rr*rr) {
        conn_line = pm11 - p1;
        if(conn_line.squared_length() == 0)
          conn_line_n = d1.transform(Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2)));
        else 
          conn_line_n = conn_line / sqrt(conn_line.squared_length());
        pm11 = pm11 + conn_line_n * (rr / sqrt((pm11 - p1).squared_length()));
      }

      if((pm22 - p2).squared_length() > rr*rr) {
        conn_line = pm22 - p2;
        if(conn_line.squared_length() == 0)
          conn_line_n = d1.transform(Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2)));
        else 
          conn_line_n = conn_line / sqrt(conn_line.squared_length());
        pm22 = pm22 + conn_line_n * (rr / sqrt((pm22 - p2).squared_length()));
      }

      spiro_cps.push_back({pm11.x(), pm11.y(), 'c'});
      spiro_cps.push_back({pm22.x(), pm22.y(), 'c'});

      no_curve_points = true;
    }
    if (!no_curve_points) {
      spiro_cps.push_back({pm11.x(), pm11.y(), 'c'});
      spiro_cps.push_back({pm22.x(), pm22.y(), 'c'});
    }
  }

  auto pm2 = p2 + d2 * rr * 0.01;
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