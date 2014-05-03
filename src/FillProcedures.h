#pragma once
#include "CGAL_Headers.h"
#include <CGAL/Boolean_set_operations_2.h>
// forward declare

#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;

class FillProcedure {
public:
  ElemList result;
	virtual ElemList fill(Polygon_with_holes_2 * poly) {};
};

class SpiralFillProcedure : public FillProcedure {
public:
  static SpiralFillProcedure& getInstance() {
      static SpiralFillProcedure instance; // Guaranteed to be destroyed.
                                           // Instantiated on first use.
      return instance;
  }
public:
  // ElemList result;
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
typedef std::list<Polygon_with_holes_2> Pwh_list_2;
class WiggleFillProcedure : FillProcedure {
private:
  Direction_2 direction;
public:
  static WiggleFillProcedure& getInstance() {
    static WiggleFillProcedure instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
    return instance;
  }

  Polygon_2 p;
  std::list<Segment_2> poly_lines;
  ElemList fill(Polygon_with_holes_2 * poly) {
    float line_distance = 5;
    this->p = poly->outer_boundary();
    Point_2 v_start = *(p.left_vertex());
    v_start = v_start + Vector_2(line_distance, 0);
    // auto sect = CGAL::intersection(p, Line_2(Point_2(0,0), Point_2(600, 600)));
    // Line_2 l_test = Line_2(Point_2(-10, -10), Point_2(500, 500));
    Pwh_list_2 intR;

    int len = p.size();
    for(int i = 0; i <= len; ++i ) {
      Point_2 p1 = p[i ? i-1 : len - 1];
      Point_2 p2 = p[i];
      poly_lines.push_back(Segment_2(p1, p2));
    }

    std::list<Point_2> intersections;
    bool done = false;
    direction = Direction_2(Vector_2(Point_2(0, 0), Point_2(0, 1)));
    Line_2 l_test = Line_2(v_start, direction);
    int i = 0;
    int count = 0;
    float act_x = v_start.x();
    while(!done) {
      for(Segment_2 segment : poly_lines) {
        CGAL::Object intersect = CGAL::intersection (l_test, segment);
        Point_2 intersect_point;
        if(assign(intersect_point, intersect)) {
          intersections.push_back(intersect_point);
          ++count;
        }
      }
      cout << "C: " << count;
      if(count == 0) {
        done = true;
      }
      count = 0;

      cout << " act_x: " << act_x << " " << endl;

      act_x += line_distance;
      l_test = Line_2(Point_2(act_x, 0), direction);

    }

    // if(intersections.size() > 2) {
    //   cout << "This polygon is not convex!" << endl;
    // }
    // else {
    // }
    ElementPtr * poly_element = new PolyLineElementPtr(intersections);
    result.push_back(poly_element);
    return result;
  }
};