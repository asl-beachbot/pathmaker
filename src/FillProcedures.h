#include "CGAL_headers.h"
#include "VectorElementTree.h"

typedef ElemList std::list<ElementPtr>;

class FillProcedure {
public:
  ElemList result;
	virtual ElemList fill() = 0;
};

class SpiralFillProcedure : FillProcedure {
	run() {
	  PolygonWithHolesPtrVector offset_poly_wh =
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, poly);
    // no more polys
    if(offset_poly_wh.size() <= 0) return 0;
    int next_depth = ++depth;
    for(std::vector<PolygonWithHolesPtr>::iterator i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
      Polygon_2 outer = (**i).outer_boundary();
      ElementPtr poly_element = PolygonElementPtr(outer);
      result.push_back(poly_element);
  }
	Polygon_2 * poly;
	ElemList fill(Polygon_2 * poly) {
    this->poly = poly;
    return result; // return copy of result 
	}
  ~SpiralFillProcedure() {
    // delete poly;
  }
};

class WiggleFillProcedure : FillProcedure {
};