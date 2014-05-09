// PostProecssor

#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include <cmath>  // sqrt

typedef VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef VectorElementTree Tree;
typedef std::list<Point_2> PointList;
class PostProcessor {
private:
  float radius;
  float angle_interpolation;

  VectorElementTree * tree;
  Transformation rotate_90(CGAL::ROTATION, sin(M_PI/2), cos(M_PI/2));
  Transformation rotate_m90(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2));

  float calc_angle(Vector_2 v1, Vector_2 v2) {
    cout << "Angle " << v1*v2 << " deg " << acos(v1 * v2) << endl;
    return acos(v1 * v2);
  }

  PointList round_corner(float angle, Vector_2 v1_n, Vector_2 v2_n) {
    // Algorithm to round corners of polygons
    // Two possible solution possibiliteis:
    // 1.  Circle with radius at corner (at the moment implemented)
    // 2.  Interprete corner as spline control point
    //     And use 2 other points on edge as origin points
    PointList result;

    float counter_angle = M_PI - angle;
    float l_down = radius / tan(angle / 2);

    // cont. if vectors are too short!
    if(v2.squared_length() < (l_down * l_down) ||
       v1.squared_length() < (l_down * l_down)) return result;

    float interpolate_angle = angle_interpolation * M_PI;
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    int orientation = CGAL::orientation(p1, p2, p3);
    if(orientation == CGAL::LEFT_TURN) {
      v1_m = (p2 - CGAL::ORIGIN) - (v1_n * l_down) - (v1_n.transform(rotate_m90) * radius);
      rotate_rad = ((p2 - CGAL::ORIGIN) - v1_n * l_down) - v1_m;
    } else if(orientation == CGAL::RIGHT_TURN) {
      v1_m = (p2 - CGAL::ORIGIN) - (v2_n * l_down) - (v2_n.transform(rotate_m90) * radius);
      rotate_rad = ((p2 - CGAL::ORIGIN) - v2_n * l_down) - v1_m;
    } else {
      return result; // collinear!
    }

    int len_interpol = floor(counter_angle / interpolate_angle);
    cout << angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    // check NaN
    if(angle != angle) continue;
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(i), cos(i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      cout << "Iterating over rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }
    return list;
  }
  PointList round_outer_corner(float angle, Vector_2 v1_n, Vector_2 v2_n, Point_2 p2) {

    PointList result;

    // possible change to direction instead of numerically unstable sqrt
    if(angle != angle) continue;
    float counter_angle = M_PI + angle;

    float l_down = radius / tan(angle / 2);
    float interpolate_angle = 0.01 * M_PI;
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    if(orientation == CGAL::LEFT_TURN) {
      v1_m = (p2 - CGAL::ORIGIN) + (v1_n * l_down) + (v1_n.transform(rotate_m90) * radius);
      rotate_rad = ((p2 - CGAL::ORIGIN) + v1_n * l_down) - v1_m;
    } else if(orientation == CGAL::RIGHT_TURN) {
      v1_m = (p2 - CGAL::ORIGIN) + (v2_n * l_down) + (v2_n.transform(rotate_m90) * radius);
      rotate_rad = ((p2 - CGAL::ORIGIN) + v2_n * l_down) - v1_m;
    } else {
      return result; // collinear!
    }

    int len_interpol = floor(counter_angle / interpolate_angle);
    cout << angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    result.push_back(p2);
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(-i), cos(-i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      cout << "Iterating outside rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }
    result.push_back(p2);
  }

  PointList decide_action(Point_2 p1, Point_2 p2, Point_2 p3, bool filling_element = false) {
    Vector_2 v1 = Vector_2(p1, p2);
    Vector_2 v1_n = v1 / sqrt(v1.squared_length());

    Vector_2 v2 = Vector_2(p3, p2);
    Vector_2 v2_n = v2 / sqrt(v2.squared_length());

    float angle = this->calc_angle(v1_n, v2_n);

    if(filling_element) {
      // if(angle > )
    } else {
      // only sharp corners: go outside
      if(angle < M_PI/3) {
        return round_outer_corner(angle, v1_n, v2_n, p2);
      }
      else {
        return round_corner(angle, v1_n, v2_n);
      }
    }

  }
public:
	PostProcessor(VectorElementTree * tree) : tree(tree) {
    angle_interpolation = 0.2;
    radius = 5;
  }
  void run_procedure() {
    // Something to consider: connections have been made, but 
    // the angle of the connection matters so we're finding the
    // right side of entry to the polygon
  	auto it = tree->begin();
  	auto it_end = tree->end();
  	// skip playfield
  	for(++it; it != it_end; ++it) {
      switch((*it)->get_type()) {
        case EL_POLYLINE:
          z
      }
  	}
  }
};
