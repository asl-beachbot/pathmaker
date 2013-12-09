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
// #include <list>
#include <fstream>

// #include <boost/foreach.hpp>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/xml_parser.hpp>
// #include <boost/format.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include<CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/Qt/PolygonGraphicsItem.h>
#include <CGAL/Qt/PolygonWithHolesGraphicsItem.h>

#include <QtGui>

#include <CGAL/Qt/GraphicsViewNavigation.h>
// #include <GL/gl.h>
// #include <GLU/glu.h>
// #include <GLUT/glut.h>

// Own libraries
// #include "svg_calculate.cpp"

// Defining Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                    Point;
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

typedef CGAL::Straight_skeleton_2<K> Ss ;
typedef boost::shared_ptr<Ss> SsPtr ;


using std::endl; using std::cout;

// using boost::property_tree::ptree;

// void pretty_print_property_tree(ptree const& pt) {
  
//   ptree::const_iterator end = pt.end();
//   for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
//     cout << it->first << ": " << it->second.get_value<std::string>() << endl;
//     pretty_print_property_tree(it->second);
//   }
// }

void iterate_polygon(Polygon_2 *p) {
  for(typename Polygon_2::Vertex_const_iterator i = p->vertices_begin(); i != p->vertices_end(); ++i) {
    cout << (*i).x() << " " << i->x() << endl;
  }
}

void iterate_over_polygon_with_holes(PolygonWithHolesPtrVector *p) {
  /*
   *for(std::vector<PolygonWithHolesPtr>::iterator i = p->begin(); i != p->end(); ++i) {
   *  Polygon_2 outer = (**i).outer_boundary();
   *  iterate_polygon(&outer);
   *}
   */
  
}

int main(int argc, char **argv) {
  cout << "Welcome to the Pathfinder. Finding a path through the dark since 1999." << endl << "  (c) BeachBot Productions LLC. ";

  QApplication app(argc, argv);
  QGraphicsScene scene;
  QGraphicsView* view = new QGraphicsView(&scene);
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

  double lOffset = 1;
  // TODO
  // Investigate usage of straight skeleton calculation
  SSPtr straight_skel = CGAL::create_interior_straight_skeleton_2(polygon_wh);
  
  PolygonWithHolesPtrVectorVector offset_polys;
  PolygonWithHolesPtrVector offset_poly_wh = 
    CGAL::create_offset_polygons_2<Polygon_with_holes_2>(lOffset, *straight_skel);
  //PolygonWithHolesPtrVector offset_poly_wh = 
    //CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
  offset_polys.push_back(offset_poly_wh);
  
  iterate_over_polygon_with_holes(&offset_poly_wh);

  int i = 0;
  QColor pen_color(10, 250, 250);
  QPen pen(pen_color);
  for(;i<=3; ++i) {
    lOffset += i;
    PolygonWithHolesPtrVector offset_poly_wh = 
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);
    offset_polys.push_back(offset_poly_wh);
    for(PolygonWithHolesPtrVector::iterator poly = offset_polys[i].begin(); poly != offset_polys[i].end(); ++poly) {
      CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> *pgi = 
        new CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2>((**poly));
      pgi->setEdgesPen(pen);
      scene.addItem(pgi);
    }
  }

  // just for fun and profit
  // iterate_polygon(p);

  view->show();
  return app.exec();

  // read_xml("./assets/bubbles.svg", pt);

  // SVGCalculate * test = new SVGCalculate(pt);

}
