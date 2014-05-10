// PostProecssor

#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include <cmath>  // sqrt


typedef VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef VectorElementTree Tree;
typedef std::vector<Point_2> PointList;

struct ThreePointElem {
  Point_2 p1;
  Point_2 p2;
  Point_2 p3;
  Vector_2 v1;
  Vector_2 v2;
  Vector_2 v1_n;
  Vector_2 v2_n;
  float angle;
  int orientation;
};

class PostProcessor {
private:
  float radius;
  float angle_interpolation;

  VectorElementTree * tree;
  Transformation rotate_90;
  Transformation rotate_m90;

  float calc_angle(Vector_2 v1, Vector_2 v2) {
    cout << "Angle " << v1*v2 << " deg " << acos(v1 * v2) << endl;
    return acos(v1 * v2);
  }

  PointList round_corner(ThreePointElem * el) {
    // Algorithm to round corners of polygons
    // Two possible solution possibiliteis:
    // 1.  Circle with radius at corner (at the moment implemented)
    // 2.  Interprete corner as spline control point
    //     And use 2 other points on edge as origin points
    PointList result;

    float counter_angle = M_PI - el->angle;
    float l_down = radius / tan(el->angle / 2);

    // cont. if vectors are too short!
    if(el->v2.squared_length() < (l_down * l_down) ||
       el->v1.squared_length() < (l_down * l_down)) return result;

    float interpolate_angle = angle_interpolation * M_PI;
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    if(el->orientation == CGAL::RIGHT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) - (el->v1_n * l_down) - (el->v1_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) - el->v1_n * l_down) - v1_m;
    } else if(el->orientation == CGAL::LEFT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) - (el->v2_n * l_down) - (el->v2_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) - el->v2_n * l_down) - v1_m;
    } else {
      return result; // collinear!
    }

    int len_interpol = floor(counter_angle / angle_interpolation);
    cout << el->angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    // check NaN
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(i), cos(i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      cout << "Iterating over rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }
    return result;
  }

  PointList round_outer_corner(ThreePointElem * el) {

    PointList result;

    // possible change to direction instead of numerically unstable sqrt
    float counter_angle = M_PI + el->angle;

    float l_down = radius / tan(el->angle / 2);
    float interpolate_angle = angle_interpolation * M_PI;
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    if(el->orientation == CGAL::LEFT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) + (el->v1_n * l_down) + (el->v1_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) + el->v1_n * l_down) - v1_m;
    } else if(el->orientation == CGAL::RIGHT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) + (el->v2_n * l_down) + (el->v2_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) + el->v2_n * l_down) - v1_m;
    } else {
      return result; // collinear!
    }

    int len_interpol = floor(counter_angle / angle_interpolation);
    cout << el->angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    result.push_back(el->p2);
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(-i), cos(-i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      cout << "Iterating outside rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }
    result.push_back(el->p2);
    if(el->orientation == CGAL::RIGHT_TURN) {
      std::reverse(result.begin(), result.end());
    }
    return result;
  }

  PointList decide_action(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3, bool * outer, bool filling_element = false) {
    ThreePointElem elem_to_round;
    elem_to_round.p1 = p1;
    elem_to_round.p2 = p2;
    elem_to_round.p3 = p3;

    elem_to_round.v1 = Vector_2(p1, p2);
    elem_to_round.v1_n = elem_to_round.v1 / sqrt(elem_to_round.v1.squared_length());

    elem_to_round.v2 = Vector_2(p3, p2);
    elem_to_round.v2_n = elem_to_round.v2 / sqrt(elem_to_round.v2.squared_length());

    elem_to_round.angle = this->calc_angle(elem_to_round.v1_n, elem_to_round.v2_n);
    if(elem_to_round.angle != elem_to_round.angle) return PointList{p2};

    elem_to_round.orientation = CGAL::orientation(p1, p2, p3);
    if(filling_element) {
      // if(angle > )
    } else {
      // only sharp corners: go outside
      if(elem_to_round.angle < M_PI/2) {
        *outer = true;
        return round_outer_corner(&elem_to_round);
      }
      else {
        *outer = false;
        return PointList{p2};
        return round_corner(&elem_to_round);
      }
    }

  }
public:
	PostProcessor(VectorElementTree * tree, float r) : tree(tree) {
    angle_interpolation = 0.2;
    radius = 0.7;
    if(r != 0) {
      radius = r;
    }
    rotate_90 = Transformation(CGAL::ROTATION, sin(M_PI/2), cos(M_PI/2));
    rotate_m90 =  Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2));
  }
  void process() {
    // Something to consider: connections have been made, but 
    // the angle of the connection matters so we're finding the
    // right side of entry to the polygon
    auto element_tree = &(this->tree->element_tree);
    auto it = element_tree->begin();
    auto it_end = element_tree->end();

  	// skip playfield
  	for(++it; it != it_end; ++it) {
      switch((*it)->get_type()) {
        case EL_POLYLINE: {
          PolyLineElementPtr * polyline_el = static_cast<PolyLineElementPtr * >(*it);
          PolyLine_P * el = &(polyline_el->element);
          int len = el->size();
          if(len <= 2) continue;
          bool outer = false;
          for(int i = 1; i < len - 1; ++i) {
              PointList res = decide_action(el->at(i-1), el->at(i), el->at(i+1), &outer);
          }
          unsigned char rake_state;
          if(outer) {
            rake_state = 0;
          } else {
            rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
          }

        }
        break;
        case EL_POLYGON: {
          PolygonElementPtr * polygon_el = static_cast<PolygonElementPtr * >(*it);
          Polygon_2 * el = &(polygon_el->element);
          Polygon_2 * final_polygon = new Polygon_2();
          RakeVector * final_rake = new RakeVector();
          int len = el->size();
          for(int i = 0; i <= len; ++i ) {
            bool outer = false;
            // A farmers circulator
            Point_2 p1 = (*el)[i ? i-1 : len - 1];
            Point_2 p2 = (*el)[i];
            Point_2 p3 = (*el)[i+1 == len ? 0 : i + 1];
            PointList res = decide_action(p1, p2, p3, &outer);
            unsigned char rake_state;
            if(outer) {
              rake_state = 0;
            } else {
              rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
            }
            for(Point_2 p : res) {
              final_polygon->push_back(p);
              final_rake->push_back(rake_state);
              // cout << "Rounding: " << p << endl;
            }
          }
          polygon_el->element = *final_polygon;
          polygon_el->rake_states = *final_rake;
        }
        break;
      }
  	}
  }
};
