// PostProecssor

#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"

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
  std::vector<Point_2> turn_points;
  VectorElementTree * tree;
  Transformation rotate_90;
  Transformation rotate_m90;
  PolyLineElementPtr * final_element;
  float calc_angle(Vector_2 v1, Vector_2 v2);
  PointList bezierHelper (Point_2 p1, Point_2 c1, Point_2 c2, Point_2 p2);
  PointList round_corner(ThreePointElem * el);
  PointList round_outer_corner(ThreePointElem * el);
  PointList decide_action(const Point_2 & p1, const Point_2 & p2, const Point_2 & p3, bool * outer, bool filling_element = false); 
  PointList interpolateDistance(Point_2 p1, Point_2 p2);
public:
  PolyLine_P * final_path;
  RakeVector * final_rake;
	PostProcessor(VectorElementTree * tree);
  PointList round_connector(Point_2 p11, Point_2 p12, ElementPtr * to);
  void process();
};
