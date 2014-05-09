// This class is supposed to segment non-convex polygons 
// as requested.

#pragma once
#include "VectorTreeElements.h"
#include "VectorElementTree.h"
#include "CGAL_Headers.h"
#include <iterator>     // std::back_inserter

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
  void segment(FilledPolygonElementPtr * poly_element_ptr) {
    std::list<Traits_Polygon_2> partition_polys;
    Polygon_2 outer = poly_element_ptr->element.outer_boundary();
    CGAL::optimal_convex_partition_2(outer.vertices_begin(),
                                     outer.vertices_end(),
                                     std::back_inserter(partition_polys),
                                     partition_traits);
    for(Traits_Polygon_2 tp : partition_polys) {
      FilledSegment f;
      f.poly = Polygon_2 (tp.vertices_begin(), tp.vertices_end());
      f.direction = Direction_2(0, 1);
      f.fill_type = 1;

      poly_element_ptr->segments.push_back(f);
    }
    // = partition_polys;
  }
public:
  SegmentationPreProcessor(VectorElementTree * vet) {
    this->tree = vet;
  }
  void process() {
    // Segment the shapes that have to be segmented!

    element_tree = &(this->tree->element_tree);
    TreeIterator it = element_tree->begin();
    TreeIterator it_end = element_tree->end();

    for(; it != it_end; ++it) {
      if((*it)->get_type() == EL_FILLED_POLYGON) {
        // This shape should be segmented, if not convex...
        FilledPolygonElementPtr * poly_element_ptr = static_cast<FilledPolygonElementPtr * >(*it);
        segment(poly_element_ptr);
      }
    }

  }
};