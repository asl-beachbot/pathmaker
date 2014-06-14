#pragma once
#include "CGAL_Headers.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <cmath>
// forward declare

#include "GlobalOptions.h"
#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;
typedef std::list<Polygon_with_holes_2> Pwh_list_2;

class FillProcedure {
public:
  ElemList result;
  virtual ElemList fill(Polygon_with_holes_2 poly, 
    bool is_segment = false) = 0;
};

class SpiralFillProcedure : public FillProcedure {
private:
  int findClosestIndex(Point_2 p, Polygon_2 poly);
  double max_area_for_deletion;
public:
  float line_distance;
  void run(const Polygon_with_holes_2 * poly, Point_2 * start_point = nullptr);
  ElemList fill(Polygon_with_holes_2 poly, bool is_segment = false);
  ~SpiralFillProcedure();
  SpiralFillProcedure();
};

class WiggleFillProcedure : public FillProcedure {
private:
  Transformation rotate_90;
  float line_distance;
  void fill_polygon(Polygon_2 poly, Direction_2 direction);
  Point_2 findStartingPoint(Polygon_2 poly, Direction_2 direction);
  Direction_2 d;
public:
  WiggleFillProcedure();
  void setDirection(Direction_2 d);
  Polygon_2 p;
  ElemList fill(Polygon_with_holes_2 poly, bool is_segment = false);
};

class FillFactory {
private:
  SpiralFillProcedure s;
  WiggleFillProcedure w;
public:
  ElemList fill(FilledPolygonElementPtr * el);
};
