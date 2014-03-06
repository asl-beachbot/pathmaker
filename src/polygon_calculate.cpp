// Copyright (C) 2014  Wolf Vollprecht

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

// polygon_calculate.cpp


#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string.h>
#include <cstdlib>

#include <string.h>
#include <cstdlib>
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

// #include "tree.h"

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

  int insets = 10;
  double inset_width = 2;
  int pen_width = 2;
  bool pen_cosmetic = false;
  if (argc > 2) {
    for(int i = 1; i < argc; i++) {
      cout << argv[i] << " das war argv" << endl;
      if(strcmp(argv[i], "-n") == 0) {
        insets = atoi(argv[i+1]);
        cout << "Insets: " << insets << endl;
        i = i + 1;
      }
      else if (strcmp(argv[i], "-w") == 0) {
        inset_width = atof(argv[i+1]);
        cout << "Inset width: " << inset_width << endl;
        i = i + 1;
      }
      else if (strcmp(argv[i], "--pen-width") == 0) {
        pen_width = atof(argv[i+1]);
        cout << "Pen width: " << pen_width << endl;
        i = i + 1;
      }
      else if (strcmp(argv[i], "--pen-cosmetic") == 0) {
        pen_cosmetic = true;
        cout << "Pen is cosmetic" << endl;
        i = i + 1;
      }
    }
  }

  double lOffset = inset_width;

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

  ext_poly = ExtendedPolygonPtr(polygon_wh);
  
  tree = Tree(ext_poly);
  tree::<ExtendedPolygonPtr>::iterator node = tree.begin();

  for(;i<=insets; ++i) {
    lOffset += inset_width;

    PolygonWithHolesPtrVector offset_poly_wh = 
      CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, polygon_wh);

    if(offset_poly_wh.size() == 0) {
      // Break condition (no more polygons!)
      break;
    }

    // push back in ordinary list
    offset_polys.push_back(offset_poly_wh);

    tree::<ExtendedPolygonPtr>::iterator curr_lvl = node;

    Point_2 p_check;
    tree::<ExtendedPolygonPtr>::iterator next_lvl;
    for(PolygonWithHolesPtrVector::iterator poly = offset_polys[i].begin(); poly != offset_polys[i].end(); ++poly) {
      node = curr_lvl;
      outer_poly_boundary = node.poly.outer_boundary();
      p_check = poly.outer_boundary()[0]
      while(outer_poly_boundary.bounded_side(p_check) != CGAL::ON_BOUNDED_SIDE) {
        try{
          node = node.next_at_same_depth();
          outer_poly_boundary = node.poly.outer_boundary();
        }
        catch(int e) {
          cout << "No next sibling " << e << endl;
        }
      } 
      cout << "Found next parent" << endl;
      next_lvl = tree.insert(node, ExtendedPolygonPtr(poly));

      // Gradient and Paint
      PolygonWithHolesGraphicsI *pgi = new PolygonWithHolesGraphicsI((*poly).get());
      QColor pen_color(10, 250 - i * 5, 250 - i * 3);
      QPen pen(pen_color, pen_width);
      pen.setCosmetic(pen_cosmetic);
      pgi->setEdgesPen(pen);
      pgi->setVerticesPen(QPen(QColor(0,0,0,0)));
      window->addItem(pgi);
    }
    // tree::<ExtendedPolygonPointer>::node = tree.fixed_depth_iterator(next_lvl);
    //simple_connect(offset_poly_wh, offset_polys[i]);
    //simple_connect_singular_polys(&offset_poly_wh);
  }
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
  cout << "connecting <po></po>lys" << endl;
  
  // check if more than one poly exists in vector
  if(poly->size() == 1) { return; }
  
  for(std::vector<PolygonWithHolesPtr>::const_iterator i = poly->begin(); i != poly->end() - 1; ++i) {
    Polygon_2 bound1 = (**i).outer_boundary();
    Polygon_2 bound2 = (**(i+1)).outer_boundary();
    //window->addLine(bound1[0].x(), bound1[0].y(), bound2[0].x(), bound2[0].y());
  }
}

void PolygonCalculate::connect(PolygonWithHolesPtrVector inner_poly, PolygonWithHolesPtrVector outer_poly) {
  // Steps:
  // 1. Find a "kernel" from where the BeachBot ideally should start
  // 2. Drive kernel, then move on one stage further out
  // 3. Check number of "undriven" polygons inside the one we're at
  // 4. If more than 0 find the closest point to enter (from where we are)
  //    Entering should happen while driving on the polygon where at
  // 5. Drive inside again and repeat step 1 - 5

  // Step 1: Find kernel

  cout << "Running the connection sub-routine\n" << endl;

  // var to save driven points

  std::list<Point_2> reached_points;

  while(outer_poly_boundary.bounded_side(p_check) != CGAL::ON_BOUNDED_SIDE) {

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
        //window->addLine(p_found, p_target);
        //connector_lines.push_back(segment);
      }
    }
  }
}
