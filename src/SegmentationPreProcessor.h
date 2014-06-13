// This class is supposed to segment non-convex polygons 
// as requested.

#pragma once
#include "VectorTreeElements.h"
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <iterator>     // std::back_inserter
#include <algorithm>     // std::back_inserter

typedef VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef VectorElementTree Tree;
typedef CGAL::Partition_traits_2<K> Traits;
typedef Traits::Polygon_2 Traits_Polygon_2;

class SegmentationPreProcessor {
private:
  Tree * tree;
  Tree_ElementPtr * element_tree;
  Traits partition_traits;
  std::vector<Segment_2> cut_lines;
  double segment_offset;

  void segment(FilledPolygonElementPtr * poly_element_ptr) {
    std::list<Traits_Polygon_2> partition_polys;
    Polygon_2 outer = poly_element_ptr->element.outer_boundary();
    CGAL::optimal_convex_partition_2(outer.vertices_begin(),
                                     outer.vertices_end(),
                                     std::back_inserter(partition_polys),
                                     partition_traits);
    // CGAL::approx_convex_partition_2(outer.vertices_begin(),
    //                                  outer.vertices_end(),
    //                                  std::back_inserter(partition_polys),
    //                                  partition_traits);
    for(Traits_Polygon_2 tp : partition_polys) {
      if(segment_offset > 0) {
        cout << "Segment Offset: " << segment_offset << endl;
        PolygonPtrVector offset_polygons = 
          CGAL::create_interior_skeleton_and_offset_polygons_2
          (segment_offset,  Polygon_2 (tp.vertices_begin(), tp.vertices_end()));

        if(offset_polygons.size() == 0) {
          continue; // maybe some more failsafe handling?
        }
        for(auto i = offset_polygons.begin(); i != offset_polygons.end(); ++i) {
          FilledSegment f(**i);
          f.direction = Direction_2(0, 1);
          f.fill_method = 1;
          poly_element_ptr->segments.push_back(f);
        }
      }
      else {
        FilledSegment f(Polygon_2 (tp.vertices_begin(), tp.vertices_end()));
        f.direction = Direction_2(0, 1);
        f.fill_method = 1;
        poly_element_ptr->segments.push_back(f);
      }
    }
    for(auto it = poly_element_ptr->segments.begin(); it != poly_element_ptr->segments.end(); ++it) {
      cout << "Got a Segment" << endl;
    }
    // = partition_polys;
  }
  struct IntersectS {
    Point_2 p;
    int index_on_poly;
    double t;
  };

  static bool sort_intersects(const IntersectS & a, const IntersectS & b) {
    return a.t > b.t;
  }
  std::vector<IntersectS> find_intersections(Segment_2 s, Polygon_2 p) {
    std::vector<IntersectS> intersects;
    for(int i = 1; i <= p.size(); i++) {
      Segment_2 test_s = Segment_2(p[i-1], p[i % p.size()]);
      CGAL::Object intersect = CGAL::intersection (s, test_s);
      Point_2 intersect_point;
      if(assign(intersect_point, intersect)) {
        double t = (intersect_point.x() - s.source().x()) / (s.target().x() - s.source().x());
        cout << "Found intersection " << intersect_point << " On: " << t << endl;
        intersects.push_back({
          intersect_point,
          i,
          t
        });
      }
    }
    return intersects;
  }
  std::vector<Polygon_2> find_polygons_after_intersection(Polygon_2 p, Segment_2 s) {
    std::vector<IntersectS> intersects = find_intersections(s, p);
    for(IntersectS i : intersects) {
      cout << "Intersect: " << i.p << " " << i.index_on_poly << " " << i.t << endl;
    }
    std::sort(intersects.begin(), intersects.end(), &SegmentationPreProcessor::sort_intersects);
    cout << "Done Sorting! " << endl;
    for(IntersectS i : intersects) {
      cout << "Intersect: " << i.p << " " << i.index_on_poly << " " << i.t << endl;
    }
    int i = 1;
    std::vector<Polygon_2> result_polys;
    result_polys.push_back(p);
    if(intersects.size() < 2) {
      return result_polys;
    }
    for(; i < intersects.size(); i++) {
      cout << "Iterating intersects!" << endl;
      cout << intersects[0].p << " " << i << " " << intersects[i-1].p << endl;
      Segment_2 s(intersects[i-1].p, intersects[i].p);
      Point_2 m = s.source() + ((s.target() - s.source()) * 0.5);
      cout << "M : " << m << endl;
      std::vector<Polygon_2> new_res_polys;
      for(Polygon_2 i_p : result_polys) {
        if(i_p.has_on_bounded_side(m)) {
          // cut along this path!
          // the path is dividing the polygon into a left and a right side
          std::vector<Point_2> poly1, poly2;
          Point_2 test_point = s.source();
          // traverse poly in clockwise fashion
          bool switched = false;
          int n = 0;
          int j = 1;
          while(n < 3 && j < 2.2 * i_p.size()) {
            Segment_2 test_s(i_p[(j - 1) % i_p.size()], i_p[j  % i_p.size()]);
            Point_2 curr_point = i_p[(j - 1) % i_p.size()];
            if(CGAL::squared_distance(test_s, test_point) < 0.2) {
              // start filling poly1
              cout << "switching " << endl;
              n++;
              switched = true;
            }
            if(n == 3) {
              poly2.push_back(curr_point);
            }
            cout << "Switched: " << switched << " CurrPoint: " << curr_point << " N: " << n << endl;
            if(n == 1 && switched == false) {
              poly1.push_back(curr_point);
            }
            if(n == 1 && switched) {
              poly1.push_back(s.source());
              poly2.push_back(s.source());
              test_point = s.target();
              switched = false;
            }
            if(n == 2 && switched == false) {
              poly2.push_back(curr_point);
            }
            if(n == 2 && switched) {
              poly1.push_back(curr_point);
              poly1.push_back(s.target());
              poly2.push_back(s.target()); 
              test_point = s.source();
              switched = false;
            }
            j++;
          }
          new_res_polys.push_back(Polygon_2(poly1.begin(), poly1.end()));
          new_res_polys.push_back(Polygon_2(poly2.begin(), poly2.end()));
        } else {
          new_res_polys.push_back(i_p);
        }
      }
      result_polys = new_res_polys;
    }
    // segment current element from vector element tree
    return result_polys;
  }
public:
  SegmentationPreProcessor(VectorElementTree * vet) {
    this->segment_offset = GlobalOptions::getInstance().segment_offset;
    this->tree = vet;
  }

  void addCutLine(Segment_2 s) {
    // Transformation t = GlobalOptions::getInstance().global_trafo;
    Transformation t = Transformation(CGAL::SCALING, 1/GlobalOptions::getInstance().scale_for_disp);
    s = s.transform(t);
    cout << "Scaled Cut Line: " << s << endl;
    cut_lines.push_back(s);
  }

  void process() {
    // Segment the shapes that have to be segmented!

    element_tree = &(this->tree->element_tree);
    TreeIterator it = element_tree->begin();
    TreeIterator it_end = element_tree->end();

    // for(; it != it_end; ++it) {
    //   if((*it)->attrs.count("cut_line")) {

    //   }
    // }

    std::vector<ElementPtr *> new_elems;
    for(Segment_2 cl : cut_lines) {
      cout << "Segmenting with line: " << cl << endl;
      for(;; it != it_end) {
        if((*it)->get_type() == EL_FILLED_POLYGON) {
          // This shape should be segmented, if not convex...
          FilledPolygonElementPtr * poly_element_ptr = static_cast<FilledPolygonElementPtr * >(*it);
          std::vector<Polygon_2> new_polys = find_polygons_after_intersection(poly_element_ptr->element.outer_boundary(), cl);
          cout << "Found new polys: " << new_polys.size() << endl;
          std::list<Polygon_2> holes;
          for(Polygon_2 p : new_polys) {
            cout << "New Polygon: " << p << endl;
            FilledPolygonElementPtr * new_elem = new FilledPolygonElementPtr(Polygon_with_holes_2(p, holes.end(), holes.end()));
            // Todo take the fill stpuff from parent! 
            new_elems.push_back(new_elem);
            cout << new_elems[0] << endl;
          }
          it = element_tree->erase(it);
          if(it == it_end) break;
        } else {
          ++it;
        }
      }
      cut_lines.clear();
      cout << "Inserting new Elements " << new_elems.size() << endl;
      for(ElementPtr * e : new_elems) {
        e->print();
        tree->insertIntoTree(e);
      }
    }

    it = element_tree->begin();
    it_end = element_tree->end();

    for(; it != it_end; ++it) {
      if((*it)->get_type() == EL_FILLED_POLYGON) {
        // This shape should be segmented, if not convex...
        FilledPolygonElementPtr * poly_element_ptr = static_cast<FilledPolygonElementPtr * >(*it);
        segment(poly_element_ptr);
      }
    }
  }
};