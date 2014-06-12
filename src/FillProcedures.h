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
	virtual ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) = 0;
};

class SpiralFillProcedure : public FillProcedure {
public:
  float line_distance;
  void run(const Polygon_with_holes_2 * poly, Point_2 * start_point = nullptr) {
    cout << "Area smaller than " << max_area_for_deletion << " will get deleted" << endl;
    float lOffset = line_distance / poly->outer_boundary().size();
    int count = 0;
    int idx = 0;
    Point_2 prev_point;
    Polygon_2 outer;

    SSPtr ss = CGAL::create_interior_straight_skeleton_2(*poly);
    PolygonPtrVector offset_poly_wh = CGAL::create_offset_polygons_2(lOffset, *ss);

    if(start_point) {
      idx = findClosestIndex((*start_point), *offset_poly_wh[0]);
    }

    cout << "Closest index: " << idx << endl;
    PolyLine_P superline;
    outer = *offset_poly_wh[0];
    // superline.push_back(outer[idx - 1 % offset_poly_wh[0]->size()]); (check if diagonal would be better...)
    superline.push_back(outer[idx % offset_poly_wh[0]->size()]);
    while(offset_poly_wh.size() > 0) {
      cout << "adding poly" << count << " l o " << lOffset << *offset_poly_wh[0]<<  endl;
      count++;
      // no more polys
      if(offset_poly_wh.size() > 1) {
        // decide which poly should continue this spiral
        // and with the rest, start a new recursion!
        int index_of_spiral_cont = 0;
        double curr_min_dist = 99999999;
        int count_ind = 0;
        for(int count_ind = 0; count_ind < offset_poly_wh.size(); ++count_ind) {
          int cl_index = findClosestIndex(prev_point, *offset_poly_wh[count_ind]);
          double dist = CGAL::squared_distance((*offset_poly_wh[count_ind])[cl_index], prev_point);
          if(dist < curr_min_dist) {
            curr_min_dist = dist;
            index_of_spiral_cont = count_ind;
          }
        }
        for(int count_ind = 0; count_ind < offset_poly_wh.size(); ++count_ind) {
          if(count_ind == index_of_spiral_cont) continue;
          else {
            this->run(new Polygon_with_holes_2((*offset_poly_wh[count_ind]), poly->holes_begin(), poly->holes_end()), &prev_point);
          }
        }
        Polygon_with_holes_2 temp_poly((*offset_poly_wh[index_of_spiral_cont]), poly->holes_begin(), poly->holes_end());
        // get a new idx! 
        idx = findClosestIndex(prev_point, (*offset_poly_wh[index_of_spiral_cont])) + 1;
        cout << "Temp Poly: " << temp_poly << endl;
        ss = CGAL::create_interior_straight_skeleton_2(temp_poly);
        lOffset = 0; // restart lOffset
        offset_poly_wh = CGAL::create_offset_polygons_2(lOffset, *ss);

      }
      for(auto i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
        outer = (**i);
        cout << "Area: " << outer.area() << endl; 

        if(max_area_for_deletion != 0 && max_area_for_deletion > std::abs(outer.area())) { // area is signed ccw or cw
          cout << "Skipping / Removing inside poly" << endl;
          continue;
        }
        ElementPtr * poly_element = new PolygonElementPtr(outer, Rake::RAKE_FULL);
        poly_element->fill_element = true;
        cout << poly_element << endl;
        // result.push_back(poly_element); TODO handle this stuff!
      }
      idx = findClosestIndex(prev_point, outer) + 1;
      superline.push_back(outer[idx % outer.size()]);
      prev_point = outer[idx % outer.size()];
      idx += 1;
      lOffset = lOffset + (line_distance / outer.size());
      offset_poly_wh = CGAL::create_offset_polygons_2(lOffset, *ss);
    }
    ElementPtr * superline_element = new PolyLineElementPtr(superline, Rake::RAKE_FULL);
    result.push_back(superline_element);
  }

	ElemList fill(FilledPolygonElementPtr * filled_poly_ptr) {
    result.clear();
    line_distance = GlobalOptions::getInstance().line_distance;
    if(filled_poly_ptr->segments.size()) {
      // handle all segments seperately!
      for(FilledSegment s : filled_poly_ptr->segments) {
        std::list<Polygon_2> empty_list;
        this->run(new Polygon_with_holes_2(s.element, empty_list.begin(), empty_list.end()), &s.entry_point);
      }
    } else {
      this->run( &(filled_poly_ptr->element));
    }
    for(ElementPtr * e : result) {
      cout << "Element: " << e << endl;
    }
    return result;
	}
  ~SpiralFillProcedure() {}
  SpiralFillProcedure() : max_area_for_deletion(0) {
    this->max_area_for_deletion = GlobalOptions::getInstance().area_deletion_threshold;
  };
  int findClosestIndex(Point_2 p, Polygon_2 poly) {
    double curr_dist = 99999999;
    int idx = 0;
    cout << "searching closest indx" << endl;
    for(int i = 0; i < poly.size(); ++i) {
      Point_2 q = poly[i];
      if(CGAL::squared_distance(p, q) < curr_dist) {
        curr_dist = CGAL::squared_distance(p, q);
        idx = i;
      }
    }
    return idx;
  }
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
        fill_polygon(fs.element, fs.direction);
      }
    }
    else {
      fill_polygon(filled_poly_ptr->element.outer_boundary(), Direction_2(1, 0));
    }

    return result;
  }
};