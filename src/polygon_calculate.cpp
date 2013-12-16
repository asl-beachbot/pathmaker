// polygon_calculate.cpp

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/Qt/PolygonGraphicsItem.h>
#include <CGAL/Qt/PolygonWithHolesGraphicsItem.h>
#include <CGAL/squared_distance_2.h>

#include <CGAL/Segment_2.h>
#include <CGAL/Qt/SegmentsGraphicsItem.h>

#include <QtGui>

#include <CGAL/Qt/GraphicsViewNavigation.h>

#include "polygon_calculate.h"
//#include "view.h"


// defining types

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                    Point_2;
typedef K::Segment_2                  Segment_2;
typedef K::Line_2                     Line_2;
typedef CGAL::Polygon_2<K>            Polygon_2;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes_2;

typedef boost::shared_ptr<Polygon_2>  PolygonPtr;
typedef std::vector<PolygonPtr>       PolygonPtrVector ;
typedef std::vector<PolygonPtrVector> PolygonPtrVectorVector;

typedef CGAL::Straight_skeleton_2<K>  SS;
typedef boost::shared_ptr<SS>         SSPtr;

typedef boost::shared_ptr<Polygon_with_holes_2>   PolygonWithHolesPtr;
typedef std::vector<PolygonWithHolesPtr>          PolygonWithHolesPtrVector;
typedef std::vector<PolygonWithHolesPtrVector>    PolygonWithHolesPtrVectorVector;

typedef CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> PolygonWithHolesGraphicsI;

using std::endl; using std::cout;

PolygonCalculate::PolygonCalculate() { }

void PolygonCalculate::run_program(int argc, char** argv, PolygonWindow* window) {

  this->window = window;

  cout << endl << "Welcome to the Pathfinder. Finding a path through the dark since 1999." << 
    endl << "  (c) BeachBot Productions LLC. ";

  sgi = new CGAL::Qt::SegmentsGraphicsItem<std::vector<Segment_2> >(&connector_lines);

  if( argc > 1 ) {
    std::string filename = argv[1];
    std::ifstream input_stream(filename.c_str());
    if(input_stream) {
      input_stream >> polygon_wh;
      phgi = new PolygonWithHolesGraphicsI(&polygon_wh);
      phgi->show();
      window->addItem(phgi);
    }
  }

  double lOffset = 2;
  // TODO
  // Investigate usage of straight skeleton calculation
  // SSPtr straight_skel = CGAL::create_interior_straight_skeleton_2(polygon_wh);
  // PolygonWithHolesPtrVector offset_poly_wh = 
  //   CGAL::create_offset_polygons_2<Polygon_with_holes_2>(lOffset, *straight_skel);
  
  PolygonWithHolesPtrVector offset_poly_wh = 
    CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
  offset_polys.push_back(offset_poly_wh);
  
  PolygonWithHolesPtr outer_poly_ptr(&polygon_wh);
  outer_poly_wrapper.push_back(outer_poly_ptr);
  simple_connect(offset_poly_wh, outer_poly_wrapper);

  int i = 0;
  QColor pen_color(10, 250, 250);
  QPen pen(pen_color);
  for(;i<=30; ++i) {
    lOffset += 2;
    PolygonWithHolesPtrVector offset_poly_wh = 
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
    offset_polys.push_back(offset_poly_wh);
    for(PolygonWithHolesPtrVector::iterator poly = offset_polys[i].begin(); poly != offset_polys[i].end(); ++poly) {
       PolygonWithHolesGraphicsI *pgi = new PolygonWithHolesGraphicsI((*poly).get());
      pgi->setEdgesPen(pen);
      window->addItem(pgi);
    }

    simple_connect(offset_poly_wh, offset_polys[i]);
    simple_connect_singular_polys(&offset_poly_wh);
  }

  // display lines

  QColor segments_color(40, 50, 10);
  QPen segments_pen(segments_color);
  sgi->setZValue(0);
  segments_pen.setWidth(20);
  sgi->setVerticesPen(segments_pen);

  window->addItem(sgi);
  sgi->show();
}

void PolygonCalculate::iterate_polygon(Polygon_2 *p) {
  for(typename Polygon_2::Vertex_const_iterator i = p->vertices_begin(); i != p->vertices_end(); ++i) {
    cout << (*i).x() << " " << i->x() << endl;
  }
}
  
  // Iterate over outer boundary (w/o holes)
void PolygonCalculate::iterate_over_polygon_with_holes(PolygonWithHolesPtrVector *p) {  
for(std::vector<PolygonWithHolesPtr>::iterator i = p->begin(); i != p->end(); ++i) {
    Polygon_2 outer = (**i).outer_boundary();
    iterate_polygon(&outer);
  }
}

void PolygonCalculate::simple_connect_singular_polys(const PolygonWithHolesPtrVector * poly) const {
  cout << "connecting polys" << endl;
  
  // check if more than one poly exists in vector
  if(poly->size() == 1) { return; }
  
  for(std::vector<PolygonWithHolesPtr>::const_iterator i = poly->begin(); i != poly->end() - 1; ++i) {
    Polygon_2 bound1 = (**i).outer_boundary();
    Polygon_2 bound2 = (**(i+1)).outer_boundary();
    window->addLine(bound1[0].x(), bound1[0].y(), bound2[0].x(), bound2[0].y());
  }
}

void PolygonCalculate::simple_connect(PolygonWithHolesPtrVector inner_poly, PolygonWithHolesPtrVector outer_poly) {
  cout << "connecting" << endl;
  for(std::vector<PolygonWithHolesPtr>::iterator i = inner_poly.begin(); i != inner_poly.end(); ++i) {
    Polygon_2 inner_poly_boundary = (**i).outer_boundary();
    for(std::vector<PolygonWithHolesPtr>::iterator j = outer_poly.begin(); j != outer_poly.end(); ++j) {
      Polygon_2 outer_poly_boundary = (**j).outer_boundary();

      Point_2 p_target = inner_poly_boundary[0];
      if(outer_poly_boundary.bounded_side(p_target) != CGAL::ON_BOUNDED_SIDE) {
        continue;
      }
      cout << "Target Point: " << p_target.x() << " " << p_target.y() << endl << endl;
      float dist = 0;
      Point_2 p_found;
      for(typename Polygon_2::Vertex_const_iterator i = outer_poly_boundary.vertices_begin(); i != outer_poly_boundary.vertices_end(); ++i) {
        // cout << i->x() << " " << i->y() << endl;
        float temp_dist = CGAL::squared_distance(*i, p_target);
        cout << temp_dist << endl;
        if(dist == 0 || temp_dist < dist) {
          dist = temp_dist;
          p_found = *i;
          cout << "Found something!" << endl;
          cout << "New Line: " << i->x() << "," << i->y() << "  " << p_target.x() << "," << p_target.y() << endl;
        }
      }
      if(dist != 0) {
        Segment_2 segment(p_found, p_target);
        window->addLine(p_found, p_target);
        //connector_lines.push_back(segment);
      }
    }
  }
}
