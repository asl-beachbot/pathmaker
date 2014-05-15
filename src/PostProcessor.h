// PostProecssor

#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include <cmath>  // sqrt
#include <boost/format.hpp>
#include "GlobalOptions.h"
#include "Exporter.h"
using boost::format; using boost::str;


typedef VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef VectorElementTree Tree;
typedef PolyLine_P PointList;

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
  float max_interpol_distance;
  double threshold_round_angle;
  int number_of_bezier_segs;
  VectorElementTree * tree;
  Transformation rotate_90;
  Transformation rotate_m90;
  PolyLineElementPtr * final_element;
  float calc_angle(Vector_2 v1, Vector_2 v2) {
    cout << "Angle " << v1*v2 << " deg " << acos(v1 * v2) << endl;
    return acos(v1 * v2);
  }
  PointList bezierHelper (
          Point_2 p1,
          Point_2 c1,
          Point_2 c2,
          Point_2 p2
        )
  {
    PointList res;
    unsigned int i;
    cout << "Creating bezier with " << p1 << " " << c1 << " " << c2 << " " << p2 << endl;
    for (i=0; i <= number_of_bezier_segs; ++i)
    {
      double t = (double)i / (double)number_of_bezier_segs;

      double a = pow((1.0 - t), 3.0);
      double b = 3.0 * t * pow((1.0 - t), 2.0);
      double c = 3.0 * pow(t, 2.0) * (1.0 - t);
      double d = pow(t, 3.0);

      double x = a * p1.x() + b * c1.x() + c * c2.x() + d * p2.x();
      double y = a * p1.y() + b * c1.y() + c * c2.y() + d * p2.y();

      Point_2 p = Point_2(x, y);

      res.push_back(p);
    }
    return res;
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
       el->v1.squared_length() < (l_down * l_down)) return PointList{el->p2};

    float interpolate_angle = angle_interpolation * M_PI;
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    if(el->orientation == CGAL::LEFT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) - (el->v1_n * l_down) - (el->v1_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) - el->v1_n * l_down) - v1_m;
    } else if(el->orientation == CGAL::RIGHT_TURN) {
      v1_m = (el->p2 - CGAL::ORIGIN) - (el->v2_n * l_down) - (el->v2_n.transform(rotate_m90) * radius);
      rotate_rad = ((el->p2 - CGAL::ORIGIN) - el->v2_n * l_down) - v1_m;
    } else {
      return PointList{el->p2}; // collinear!
    }

    int len_interpol = floor(counter_angle / angle_interpolation);
    cout << el->angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    // check NaN
    for(float i = 0; i <= counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(i), cos(i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      // cout << "Iterating over rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }
    if(el->orientation == CGAL::RIGHT_TURN) {
      std::reverse(result.begin(), result.end());
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
      return PointList{el->p2}  ; // collinear!
    }

    int len_interpol = floor(counter_angle / angle_interpolation);
    cout << el->angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    result.push_back(el->p2);
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(-i), cos(-i));
      Vector_2 v_temp = rotate_rad.transform(rot);
      result.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      // cout << "Iterating outside rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
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
    if(elem_to_round.angle != elem_to_round.angle) return PointList({p2});

    elem_to_round.orientation = CGAL::orientation(p1, p2, p3);
    if(filling_element) {
      if(elem_to_round.angle < threshold_round_angle) {
        *outer = false;
        // return PointList{p2};
        return round_corner(&elem_to_round);
      }
    } else {
      // only sharp corners: go outside
      if(elem_to_round.angle < threshold_round_angle) {
        *outer = true;
        return round_outer_corner(&elem_to_round);
      }
      else {
        *outer = false;
        // return PointList{p2};
        return round_corner(&elem_to_round);
      }
    }
    return PointList{p2};
  }
  PointList interpolateDistance(Point_2 p1, Point_2 p2) {
    int n_elems = (int)ceil(sqrt((p1-p2).squared_length()) / max_interpol_distance);
    cout << (p1-p2).squared_length() << " msqid " << max_interpol_distance << " nel " << n_elems << endl;
    PointList res;
    if(n_elems > 1) {
      Vector_2 v_n = (p2 - p1) / sqrt((p1 - p2).squared_length());
      double len_el = sqrt((p1 - p2).squared_length()) / n_elems;
      for(int i = 0; i <= n_elems - 1; i++) {
        res.push_back(p1 + (v_n * (len_el * i)));
      }
      return res;
    } else {
      res.push_back(p1);
      // res.push_back(p2);
    }
    return res;
  }
public:
  PolyLine_P * final_path;
  RakeVector * final_rake;
	PostProcessor(VectorElementTree * tree) : tree(tree) {
    radius = GlobalOptions::getInstance().rounding_radius;
    angle_interpolation = GlobalOptions::getInstance().angle_interpolation_stepsize;
    number_of_bezier_segs = GlobalOptions::getInstance().number_of_bezier_segs;
    max_interpol_distance = GlobalOptions::getInstance().max_interpol_distance;
    threshold_round_angle = GlobalOptions::getInstance().threshold_round_angle;
    rotate_90 = Transformation(CGAL::ROTATION, sin(M_PI/2), cos(M_PI/2));
    rotate_m90 =  Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2));
  }
  PointList round_connector(Point_2 p11, Point_2 p12, ElementPtr * to) {
    // Extrapolate direction
    // and round with two bezier curves
    Point_2 p21, p22;
    switch(to->get_type()) {
      case EL_POLYLINE: {
        if(to->entry_point_index == 0) {
          // forward
          p21 = to->getFromIndex(0);
          p22 = to->getFromIndex(1);
        }
        else {
          // backward
          p21 = to->getFromIndex(-1);
          p22 = to->getFromIndex(-2);
        }
      }
      break;
      case EL_POLYGON:
      case EL_FILLED_POLYGON: {
        // allways clockwise or whatever
        cout << "Polygon Entry: " << to->entry_point_index << " Size: " << static_cast<PolygonElementPtr * >(to)->element.size() << endl;
        p21 = to->getFromIndex(to->entry_point_index);
        p22 = to->getFromIndex(to->entry_point_index + 1);
      }
      break;
    }
    Segment_2 s = Segment_2(p12, p21);
    Vector_2 d1 = (p12 - p11) / sqrt((p12 -p11).squared_length());
    Vector_2 d2 = (p22 - p21) / sqrt((p22 - p21).squared_length());
    double length = sqrt((p12 - p21).squared_length());
    cout << "Length of connection " << length << endl;
    Transformation cp_scale = Transformation(CGAL::SCALING, length / 4);
    Point_2 cp1 = p12 + d1.transform(cp_scale);
    Point_2 cp2 = p21 - d2.transform(cp_scale);
    return bezierHelper(p12, cp1, cp2, p21);
  }
  void process() {
    // Something to consider: connections have been made, but 
    // the angle of the connection matters so we're finding the
    // right side of entry to the polygon

    auto p0 = Point_2(0, 0);

    auto element_tree = &(this->tree->element_tree);
    auto playfield = *(element_tree->begin());
    auto it = ++element_tree->begin(); // skip playfield
    auto it_end = element_tree->end();

    final_path = new PolyLine_P();
    final_rake = new RakeVector();

    for(; it != it_end; ++it) {
      if((*it)->from == NULL) {
        break; // it = start iterator
      }
    }
    ElementPtr * elem = (*it);
    while(elem->to != NULL) {
      switch(elem->get_type()) {
        case EL_POLYLINE: {
          PolyLineElementPtr * polyline_el = static_cast<PolyLineElementPtr * >(*it);
          PolyLine_P * el = &(polyline_el->element);
          if(elem->manually_modified) {
            cout << "Elem manually modified!" << endl;
            assert(polyline_el->rake_states.size() == el->size());
            for(int i = 0; i < el->size(); ++i) {
              final_path->push_back(el->at(i));
              final_rake->push_back(polyline_el->rake_states.at(i));
            }
          } 
          else {
            int len = el->size();
            // if(len <= 2) { // there is nothing to round here!
            //   elem = elem->to;
            //   continue;
            // }
            bool outer = false;
            PointList res;
            unsigned char rake_state;
            if(polyline_el->entry_point_index == 0) {
              // forward
              int i = 1;
              for(; i < len - 1; ++i) {
                res = decide_action(el->at(i - 1), el->at(i), el->at(i + 1), &outer);
                if(outer) {
                  rake_state = 0;
                } else {
                  rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
                }
                for(Point_2 p : res) {
                  final_path->push_back(p);
                  final_rake->push_back(rake_state);
                }
              }
              if(elem->to && elem->to != playfield) {
                ++i;
                PointList temp_res = round_connector(el->at(len - 2), el->at(len - 1), elem->to);
                for(Point_2 p: temp_res) {
                  final_path->push_back(p);
                  final_rake->push_back(0);
                }
              }
            }
            else {
              int i = len - 1;
              for(; i > 0; --i) {
                res = decide_action(el->at(i + 1), el->at(i), el->at(i - 1), &outer);
                if(outer) {
                  rake_state = 0;
                } else {
                  rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
                }
                for(Point_2 p : res) {
                  assert(p != p0);
                  final_path->push_back(p);
                  final_rake->push_back(rake_state);
                }
              }
              if(elem->to && elem->to != playfield) {
                --i;
                PointList temp_res = round_connector(el->at(0), el->at(1), elem->to);
                for(Point_2 p: temp_res) {
                  assert(p != p0);
                  final_path->push_back(p);
                  final_rake->push_back(0);
                }
              }
            }
          }
        }
        break;
        case EL_FILLED_POLYGON:
        case EL_POLYGON: {
          Polygon_2 * el;
          bool boundary = false;
          if(elem->get_type() == EL_FILLED_POLYGON) {
            el = &(static_cast<FilledPolygonElementPtr * >(elem)->element.outer_boundary());
          } else {
            PolygonElementPtr * polygon_el = static_cast<PolygonElementPtr * >(elem);
            el = &(polygon_el->element);
            boundary = true;
          }
          int len = el->size();
          int end_index = len;
          int entry_index = 0;
          if(elem->to) {
            end_index = elem->exit_point_index;
            entry_index = elem->entry_point_index;
          }

          bool finished = false;
          bool circling_started = false;
          for(int i = entry_index; i <= entry_index + len; ++i ) {
            unsigned char rake_state = 0;
            bool outer = false;
            // A farmers circulator
            int ind = i % len;

            int p1_ind = ind ? ind - 1 : len - 1;
            int p3_ind = ind + 1 == len ? 0 : ind + 1;

            Point_2 p1 = (*el)[ind ? ind - 1 : len - 1];
            Point_2 p2 = (*el)[ind];
            Point_2 p3 = (*el)[ind + 1 == len ? 0 : ind + 1];

            cout << "P1 index: " << p1_ind << " P2: " << p3_ind << endl;
            cout << "Ind: " << ind << " end_ind: " << end_index << " entry: " << entry_index << " i: " << i << endl ;
            PointList res;
            if(circling_started && ind == end_index && elem->to && elem->to != playfield) {
              cout << "rounding connector" << endl;
              // elem->to->entry_point_index = elem->to->entry_point_index + 1; // TODO Quick Fix (replace in Connector)
              res = round_connector(p1, p2, elem->to);
              rake_state = (elem->to->fill_element && elem->fill_element) ? Rake::RAKE_MEDIUM : 0;
              finished = true; // finished circling
            } else if (ind == end_index && boundary) {
              // this is the end! 
              finished = true;
            } else {
              res = decide_action(p1, p2, p3, &outer, elem->fill_element);
              if(outer) {
                rake_state = 0;
              } else {
                rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
              }
            }
            for(Point_2 p : res) {
              assert(p != p0);
              final_path->push_back(p);
              final_rake->push_back(rake_state);
              // cout << "Rounding: " << p << endl;
            }
            if(outer) {
              final_rake->pop_back();
              final_rake->push_back(Rake::RAKE_MEDIUM); // replace with Linewidth
            }
            circling_started = true;
            if (finished) break;
          }
        }
        break;
        // case EL_FILLED_POLYGON: {
          // handle it like a polygon for now.
          // PolygonElementPtr * polygon_el = static_cast<FilledPolygonElementPtr * >(*it);
          // PolygonWithHoles_2 * el = &(polygon_el->element);
          // PolygonWithHoles_2 * final_polygon = new Polygon_2();
          // RakeVector * final_rake = new RakeVector();
          // int len = el->size();
          // for(int i = 0; i <= len; ++i ) {
          //   bool outer = false;
          //   // A farmers circulator
          //   Point_2 p1 = (*el)[i ? i-1 : len - 1];
          //   Point_2 p2 = (*el)[i];
          //   Point_2 p3 = (*el)[i+1 == len ? 0 : i + 1];
          //   PointList res = decide_action(p1, p2, p3, &outer);
          //   unsigned char rake_state;
          //   if(outer) {
          //     rake_state = 0;
          //   } else {
          //     rake_state = Rake::RAKE_MEDIUM; // replace with Linewidth
          //   }
          //   for(Point_2 p : res) {
          //     final_polygon->push_back(p);
          //     final_rake->push_back(rake_state);
          //     // cout << "Rounding: " << p << endl;
          //   }
          // }
          // polygon_el->element = *final_polygon;
          // polygon_el->rake_states = *final_rake;
        // }
        // break;
      }
      elem = elem->to;
    }
    // Draw it somehow!
    if(final_path->size() > 1) {
      auto len = final_path->size();
      PointList interpolated_vec;
      RakeVector interpolated_rake_vec;
      int i = 0;
      for(int j = 1; j < len; j++) {
        PointList temp_list = interpolateDistance(final_path->at(j - 1), final_path->at(j));
        interpolated_vec.insert(
          interpolated_vec.end(),
          temp_list.begin(),
          temp_list.end()
        );
        interpolated_rake_vec.insert(
          interpolated_rake_vec.end(),
          temp_list.size(),
          final_rake->at(i)
        );
        ++i;
      }
      interpolated_vec.push_back(final_path->back());
      interpolated_rake_vec.push_back(final_rake->back());
      final_path->clear();
      final_rake->clear();
      *final_path = interpolated_vec;
      *final_rake = interpolated_rake_vec;
    }
    cout << "size : " << final_path->size();
    PolyLineElementPtr * poly_el_ptr = new PolyLineElementPtr(*final_path);
    poly_el_ptr->post_processed_result = true;
    poly_el_ptr->rake_states = *final_rake;
    this->final_element = poly_el_ptr;

    Exporter e = Exporter(final_path, final_rake);
    e.export_result();
    tree->element_tree.append_child(tree->element_tree.begin(), poly_el_ptr);
  }
};
