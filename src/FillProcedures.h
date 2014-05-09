#pragma once
#include "CGAL_Headers.h"
#include <CGAL/Boolean_set_operations_2.h>
// forward declare

#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;
Transformation rotate90(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2));

class FillProcedure {
public:
  ElemList result;
	virtual ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {};
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
	ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {
    line_distance = 5;
    this->poly = &(filled_poly_ptr->element);
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
/*
  Steps of the "wiggle" fill:

    1. Offset the shape (it has to be convex!)
        - Make sure the offset does not destroy the shape! 
    2. Fill the shape with lines in the direction
        - Start point is that point, where all points lie on right 
          side of directional line!
    3. Return Polyline (or Polylines?)
*/


private:
  Direction_2 direction;
  float line_distance;
  void fill_polygon(Polygon_2 poly) {
    // inset
    // PolygonWithHolesPtrVector offset_poly_wh =
    //   CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);


    direction = Direction_2(1, 1);
    std::list<Segment_2> poly_lines;
    Point_2 v_start = findStartingPoint(poly);

    Vector_2 repos_vector = direction.vector();
    repos_vector = rotate90(repos_vector);
    // repos_vector = repos_vector.transform(Transformation(CGAL::SCALING, line_distance));
    cout << "REPOS VECTOR " << repos_vector << endl;

    v_start = v_start + repos_vector.transform(Transformation(CGAL::SCALING, line_distance));

    int len = poly.size();
    for(int i = 0; i <= len; ++i ) {
      Point_2 p1 = poly[i ? i-1 : len - 1];
      Point_2 p2 = poly[i];
      poly_lines.push_back(Segment_2(p1, p2));
    }

    std::list<Point_2> intersections;
    std::vector<Point_2> temp_intersect;
    bool done = false;
    Line_2 l_test = Line_2(v_start, direction);
    int i = 0;
    int count = 0;
    float act_x = 0;
    while(!done) {
      for(Segment_2 segment : poly_lines) {
        CGAL::Object intersect = CGAL::intersection (l_test, segment);
        Point_2 intersect_point;
        if(assign(intersect_point, intersect)) {
          temp_intersect.push_back(intersect_point);
        }
      }
      if(temp_intersect.size() >= 2) {
        if(i % 2) {
          intersections.push_back(temp_intersect[1]);
          intersections.push_back(temp_intersect[0]);
        } else {
          intersections.push_back(temp_intersect[0]);
          intersections.push_back(temp_intersect[1]);
        }
        temp_intersect.clear();
      }  else  {
        done = true;
      }

      if(i > 10000) {
        break; /// just in case something goes wrong! 
      }
      act_x += line_distance;
      ++i;
      l_test = Line_2(v_start + repos_vector.transform(Transformation(CGAL::SCALING, act_x)), direction);
    }

    ElementPtr * poly_element = new PolyLineElementPtr(intersections);
    result.push_back(poly_element);
  }
  Point_2 findStartingPoint(Polygon_2 poly) {
    auto it = poly.vertices_begin();
    auto it_end = poly.vertices_end();
    for(; it != it_end; ++it ) {
      Line_2 l_test(*it, direction);
      bool all_right = true;
      auto inner_it = poly.vertices_begin();
      auto inner_it_end = poly.vertices_end();
      for(; inner_it != inner_it_end; ++inner_it ) {
        if(inner_it == it) {
          continue;
        }
        if(l_test.has_on_positive_side(*inner_it)) {
          all_right = false;
          break;
        }
      }
      if(all_right == true) {
        cout << "Starting point found " << *it << endl;
        return *it;
      }
    }
    cout << "No Starting Point found! Aborting?" << endl;
    return *it;
  }
public:
  static WiggleFillProcedure& getInstance() {
    static WiggleFillProcedure instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
    return instance;
  }
  Polygon_2 p;
  ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {
    line_distance = 5;
    this->p = filled_poly_ptr->element.outer_boundary();

    cout << "Filling element, Segments: " << filled_poly_ptr->segments.size() << endl;
    if(filled_poly_ptr->segments.size() > 0) {
      for (FilledSegment fs : filled_poly_ptr->segments) {
        cout << "Working on Segment" << endl;
        fill_polygon(fs.poly);
      }
    }
    else {
      fill_polygon(filled_poly_ptr->element.outer_boundary());
    }

    return result;
  }
};