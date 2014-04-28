#pragma once
#include "CGAL_Headers.h"

// forward declare

#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;

// class FillProcedure {
// public:
//   ElemList result;
// 	virtual ElemList fill(Polygon_with_holes_2 * poly);
// };
 // : public FillProcedure
class SpiralFillProcedure {
public:
  static SpiralFillProcedure& getInstance() {
      static SpiralFillProcedure instance; // Guaranteed to be destroyed.
                                           // Instantiated on first use.
      return instance;
  }
public:
  ElemList result;
	void run() {
    float lOffset = line_distance;
    int count = 0;
    PolygonWithHolesPtrVector offset_poly_wh =
        CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);

    while(offset_poly_wh.size() > 0) {
      cout << "adding poly" << count << " l o " << lOffset << *offset_poly_wh[0]<<  endl;
      count++;
      // no more polys
      for(std::vector<PolygonWithHolesPtr>::iterator i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
        Polygon_2 outer = (**i).outer_boundary();
        ElementPtr * poly_element = new PolygonElementPtr(outer);
        cout << poly_element << endl;
        result.push_back(poly_element);
      }
      lOffset = lOffset + line_distance;
      offset_poly_wh =
        CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);
    }
  }
	Polygon_with_holes_2 * poly;
  float line_distance;
	ElemList fill(Polygon_with_holes_2 * poly) {
    line_distance = 5;
    this->poly = poly;
    this->run();
    for(ElementPtr * e : result) {
      cout << "Element: " << e << endl;
    }
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

// class WiggleFillProcedure : FillProcedure {
// };