// ConnectionSmoother.h


// This class is intended to smooth the found connections!

#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include "Spiro.h"

class ConnectionSmoother {
private:
  VectorElementTree * tree;
  Spiro s;
  std::vector<BezierCP> getBezierCPS(Point_2 p1, Vector_2 d1, Point_2 p2, Vector_2 d2);
  Point_2 pointBefore(ElementPtr * e);
  Point_2 pointAfter(ElementPtr * e);
public:
  ConnectionSmoother(VectorElementTree * tree) : tree(tree) {};
  void smooth();
};