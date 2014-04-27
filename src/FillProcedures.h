#pragma once
#include "CGAL_Headers.h"

// forward declare

#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;


class FillProcedure {
public:
  ElemList result;
	virtual ElemList fill(Polygon_with_holes_2 * poly);
};

class SpiralFillProcedure : public FillProcedure {
public:
  static SpiralFillProcedure& getInstance() {
      static SpiralFillProcedure instance; // Guaranteed to be destroyed.
                                           // Instantiated on first use.
      return instance;
  }
public:
	void run() {
    float lOffset = line_distance;
    PolygonWithHolesPtrVector offset_poly_wh =
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);

    while(offset_poly_wh.size() > 0) {
      // no more polys
      for(std::vector<PolygonWithHolesPtr>::iterator i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
        Polygon_2 outer = (**i).outer_boundary();
        ElementPtr * poly_element = new PolygonElementPtr(outer);
        result.push_back(poly_element);
      }
      lOffset += line_distance;
      PolygonWithHolesPtrVector offset_poly_wh =
        CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);
    }
  }
	Polygon_with_holes_2 * poly;
  float line_distance;
	ElemList fill(Polygon_with_holes_2 * poly) {
    line_distance = 3;
    this->poly = poly;
    this->run();
    return result; // return copy of result 
	} 
  ~SpiralFillProcedure() {
    // delete poly;
  }
private:
  SpiralFillProcedure() {};                   // Constructor? (the {} brackets) are needed here.
  // Dont forget to declare these two. You want to make sure they
  // are unaccessable otherwise you may accidently get copies of
  // your singleton appearing.
  SpiralFillProcedure(SpiralFillProcedure const&);// Don't Implement
  void operator=(SpiralFillProcedure const&); // Don't implement
};

class WiggleFillProcedure : FillProcedure {
};