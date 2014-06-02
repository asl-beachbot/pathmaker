#pragma once
#include "CGAL_Headers.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <cmath>
// forward declare

#include "GlobalOptions.h"
#include "VectorTreeElements.h"

typedef std::list<ElementPtr * > ElemList;

class FillProcedure {
public:
  ElemList result;
	virtual ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {};
};

class SpiralFillProcedure : public FillProcedure {
public:
  // ElemList result;
	void run() {
    cout << "Area smaller than " << max_area_for_deletion << " will get deleted" << endl;
    float lOffset = line_distance;
    int count = 0;
    SSPtr ss = CGAL::create_interior_straight_skeleton_2(*poly);

    PolygonPtrVector offset_poly_wh = CGAL::create_offset_polygons_2(lOffset, *ss);

    // PolygonWithHolesPtrVector offset_poly_wh =
    //     CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);

    while(offset_poly_wh.size() > 0) {
      cout << "adding poly" << count << " l o " << lOffset << *offset_poly_wh[0]<<  endl;
      count++;
      // no more polys
      for(auto i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
        Polygon_2 outer = (**i);
        cout << "Area: " << outer.area() << endl;
        if(max_area_for_deletion > std::abs(outer.area())) { // area is signed ccw or cw
          cout << "Skipping / Removing inside poly" << endl;
          continue;
        }
        ElementPtr * poly_element = new PolygonElementPtr(outer, Rake::RAKE_FULL);
        poly_element->fill_element = true;
        cout << poly_element << endl;
        result.push_back(poly_element);
      }
      lOffset = lOffset + line_distance;
      offset_poly_wh = CGAL::create_offset_polygons_2(lOffset, *ss);

      // offset_poly_wh =
      //   CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);
    }
  }
	Polygon_with_holes_2 * poly;
  float line_distance;
	ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {
    result.clear();
    line_distance = GlobalOptions::getInstance().line_distance;
    this->poly = &(filled_poly_ptr->element);
    this->run();
    for(ElementPtr * e : result) {
      cout << "Element: " << e << endl;
    }
    return result;
	}
  ~SpiralFillProcedure() {
  }
  SpiralFillProcedure() : max_area_for_deletion(0) {
    this->max_area_for_deletion = GlobalOptions::getInstance().area_deletion_threshold;
  };
private:
  double max_area_for_deletion;
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
  Transformation rotate_90;
  float line_distance;
  void fill_polygon(Polygon_2 poly, Direction_2 direction) {
    // inset
    // PolygonWithHolesPtrVector offset_poly_wh =
    //   CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *poly);

    std::list<Segment_2> poly_lines;
    Point_2 v_start = findStartingPoint(poly, direction);

    Vector_2 repos_vector = direction.vector();
    repos_vector = rotate_90(repos_vector);
    // repos_vector = repos_vector.transform(Transformation(CGAL::SCALING, line_distance));
    cout << "REPOS VECTOR " << repos_vector << endl;

    v_start = v_start + repos_vector.transform(Transformation(CGAL::SCALING, line_distance));

    int len = poly.size();
    for(int i = 0; i <= len; ++i ) {
      Point_2 p1 = poly[i ? i-1 : len - 1];
      Point_2 p2 = poly[i];
      poly_lines.push_back(Segment_2(p1, p2));
    }
    Point_2 prev_line_endpoint; // used for calculating next connection point

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
        int new_index = 0;
        if(i > 0) {
          double temp_dist = (prev_line_endpoint - temp_intersect[0]).squared_length();
          for(int j = 1; j < temp_intersect.size(); ++j) {
            if((prev_line_endpoint - temp_intersect[j]).squared_length() < temp_dist) {
              new_index = j;
            }
          }
        }
        if(new_index == 0) {
          intersections.push_back(temp_intersect[1]);
          intersections.push_back(temp_intersect[0]);
          ElementPtr * poly_element = new PolyLineElementPtr(
            std::list<Point_2>{temp_intersect[1], temp_intersect[0]}
          );
          poly_element->fill_element = true;
          result.push_back(poly_element);

          prev_line_endpoint = temp_intersect[1];
        } else {
          intersections.push_back(temp_intersect[0]);
          intersections.push_back(temp_intersect[1]);
          ElementPtr * poly_element = new PolyLineElementPtr(
            std::list<Point_2>{temp_intersect[0], temp_intersect[1]}
          );
          poly_element->fill_element = true;
          result.push_back(poly_element);
          prev_line_endpoint = temp_intersect[0];
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

    // ElementPtr * poly_element = new PolyLineElementPtr(intersections);
    // poly_element->fill_element = true;

    //result.push_back(poly_element);
  }
  Point_2 findStartingPoint(Polygon_2 poly, Direction_2 direction) {
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
  WiggleFillProcedure() {
    rotate_90 = Transformation(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2));
  }

  Polygon_2 p;
  ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {
    result.clear();
    line_distance = GlobalOptions::getInstance().line_distance;
    this->p = filled_poly_ptr->element.outer_boundary();

    cout << "Filling element, Segments: " << filled_poly_ptr->segments.size() << endl;
    if(filled_poly_ptr->segments.size() > 0) {
      for (FilledSegment fs : filled_poly_ptr->segments) {
        cout << "Working on Segment" << endl;
        fill_polygon(fs.poly, fs.direction);
      }
    }
    else {
      fill_polygon(filled_poly_ptr->element.outer_boundary(), Direction_2(1, 0));
    }

    return result;
  }
};