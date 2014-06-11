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
  std::vector<BezierCP> getBezierCPS(Point_2 p1, Direction_2 d1, Point_2 p2, Direction_2 d2);
public:
  ConnectionSmoother(VectorElementTree * tree) : tree(tree) {};
  void smooth();
};