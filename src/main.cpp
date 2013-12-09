/*     Pathfinder
 *
 *     Released under GPLv3
 *     Developed by Wolf Vollprecht
 *
 *     Usage:
 *      ./bin/Pathfinder ./assets/sample_1.dat 
 */

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

// Own libraries

// Defining Types
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

using std::endl; using std::cout;

// using boost::property_tree::ptree;

// void pretty_print_property_tree(ptree const& pt) {
  
//   ptree::const_iterator end = pt.end();
//   for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
//     cout << it->first << ": " << it->second.get_value<std::string>() << endl;
//     pretty_print_property_tree(it->second);
//   }
// }

std::vector<Segment_2> connector_lines;

CGAL::Qt::SegmentsGraphicsItem<std::vector<Segment_2> > * sgi;


void iterate_polygon(Polygon_2 *p) {
  for(typename Polygon_2::Vertex_const_iterator i = p->vertices_begin(); i != p->vertices_end(); ++i) {
    cout << (*i).x() << " " << i->x() << endl;
  }
}

// Iterate over outer boundary (w/o holes)
void iterate_over_polygon_with_holes(PolygonWithHolesPtrVector *p) {  
  for(std::vector<PolygonWithHolesPtr>::iterator i = p->begin(); i != p->end(); ++i) {
    Polygon_2 outer = (**i).outer_boundary();
    iterate_polygon(&outer);
  }
}

void simple_connect(PolygonWithHolesPtrVector inner_poly, PolygonWithHolesPtrVector outer_poly) {
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
        connector_lines.push_back(segment);
      }
    }
  }
}

int main(int argc, char **argv) {
  cout << endl << "Welcome to the Pathfinder. Finding a path through the dark since 1999." << endl << "  (c) BeachBot Productions LLC. ";

  QApplication app(argc, argv);
  QGraphicsScene scene;
  QGraphicsView* view = new QGraphicsView(&scene);
  sgi = new CGAL::Qt::SegmentsGraphicsItem<std::vector<Segment_2> >(&connector_lines);

  // flip view
  view->scale(1, -1);
  CGAL::Qt::GraphicsViewNavigation navigation;
  view->installEventFilter(&navigation);
  view->viewport()->installEventFilter(&navigation);
  view->setRenderHint(QPainter::Antialiasing);
  view->show();

  Polygon_with_holes_2 polygon_wh;
  CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> *phgi;
  if( argc > 1 ) {
    std::string filename = argv[1];
    std::ifstream input_stream(filename.c_str());
    if(input_stream) {
      input_stream >> polygon_wh;
      phgi = new CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2>(&polygon_wh);
      scene.addItem(phgi);
    }
  }

  double lOffset = 2;
  // TODO
  // Investigate usage of straight skeleton calculation
  // SSPtr straight_skel = CGAL::create_interior_straight_skeleton_2(polygon_wh);
  // PolygonWithHolesPtrVector offset_poly_wh = 
  //   CGAL::create_offset_polygons_2<Polygon_with_holes_2>(lOffset, *straight_skel);
  
  PolygonWithHolesPtrVectorVector offset_polys;
  PolygonWithHolesPtrVector offset_poly_wh = 
    CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
  offset_polys.push_back(offset_poly_wh);
  
  PolygonWithHolesPtrVector outer_poly_wrapper;
  PolygonWithHolesPtr outer_poly_ptr(&polygon_wh);
  outer_poly_wrapper.push_back(outer_poly_ptr);
  simple_connect(offset_poly_wh, outer_poly_wrapper);

  // iterate_over_polygon_with_holes(&offset_poly_wh);

  int i = 0;
  QColor pen_color(10, 250, 250);
  QPen pen(pen_color);
  for(;i<=3; ++i) {
    lOffset += 2;
    PolygonWithHolesPtrVector offset_poly_wh = 
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
    offset_polys.push_back(offset_poly_wh);
    for(PolygonWithHolesPtrVector::iterator poly = offset_polys[i].begin(); poly != offset_polys[i].end(); ++poly) {
      CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> *pgi = 
        new CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2>((*poly).get());
      pgi->setEdgesPen(pen);
      scene.addItem(pgi);
    }

    simple_connect(offset_poly_wh, offset_polys[i]);

  }

  // display lines
  
  // QColor pen_color2(10, 0, 250);
  // QPen pen2(pen_color2);
  // for(std::vector<Line_2>::iterator it = connector_lines.begin(); it != connector_lines.end(); ++it) {
  //   cout << *it << endl;
  //   CGAL::Qt::LineGraphicsItem<K> *lgi = new CGAL::Qt::LineGraphicsItem<K>();
  //   lgi->setPen(pen2);
  //   lgi->setLine(*it);
  //   scene.addItem(lgi);
  //   lgi->show();
  // }

  scene.addItem(sgi);
  sgi->show();

  // just for fun and profit
  // iterate_polygon(p);

  view->show();
  return app.exec();

  // read_xml("./assets/bubbles.svg", pt);

  // SVGCalculate * test = new SVGCalculate(pt);

}
