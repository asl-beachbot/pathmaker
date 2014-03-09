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

// polygon_calculate.h

#pragma once
#include <vector>
#include <iostream>

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

#include "view.h"
#include "tree.h"

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

typedef CGAL::Qt::PolygonGraphicsItem<Polygon_2> PolygonGraphicsI;


class ExtendedPolygonPtr{
public:
  Polygon_2 poly;
  int visited_polys[];
  bool visited;
  ExtendedPolygonPtr(Polygon_2 poly) : poly(poly) {};
  ExtendedPolygonPtr() {};
  PolygonGraphicsI * graphx;
  void set_graphx() {
	this->graphx = new PolygonGraphicsI(&poly);
	return;
  }
  void print_poly() {
    std::cout << "Polygon " << this << " " << poly << std::endl;
  }
};

typedef tree<ExtendedPolygonPtr> PolyTree;

class PolygonCalculate{
public:
  PolygonCalculate();
  void run_program(int argc, char** argv, PolygonWindow* window);
private:
  PolygonWindow* window;
  std::vector<Segment_2> connector_lines;
  CGAL::Qt::SegmentsGraphicsItem<std::vector<Segment_2> > * sgi;
  CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> *phgi;
  Polygon_with_holes_2 polygon_wh;
  PolygonWithHolesPtrVectorVector offset_polys;
  PolygonWithHolesPtrVector outer_poly_wrapper;
  PolyTree p_tree;
  PolygonWithHolesPtrVector offset_poly_wh;
  PolygonGraphicsI * pgi;

  PolygonPtrVector render_polys;
  PolygonWithHolesPtr outer_poly_ptr;

  void iterate_polygon(Polygon_2 *p);
  void iterate_over_polygon_with_holes(PolygonWithHolesPtrVector *p);
  void connect();
  void simple_connect(PolygonWithHolesPtrVector inner_poly, PolygonWithHolesPtrVector outer_poly); 
  void simple_connect_singular_polys(const PolygonWithHolesPtrVector * poly) const;
  int find_and_add(PolyTree * tree, PolyTree::iterator curr_node, 
  PolygonWithHolesPtr p, int depth);
};
