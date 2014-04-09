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
#include <CGAL/intersections.h>

#include <CGAL/Aff_transformation_2.h>

#include <CGAL/Segment_2.h>
#include <CGAL/Qt/SegmentsGraphicsItem.h>

#include <QtGui>

#include <CGAL/Qt/GraphicsViewNavigation.h>

#include <Eigen/Dense>
using namespace Eigen;

#include "polygon_calculate.h"

// #include "tree.h"

//#include "view.h"


// defining types

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                    Point_2;
typedef K::Segment_2                  Segment_2;
typedef K::Vector_2                   Vector_2;
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

typedef CGAL::Qt::PolygonGraphicsItem<Polygon_2> PolygonGraphicsI;

typedef CGAL::Qt::CustomPolylinesGraphicsItem<std::list<std::list<Point_2> > > PolylinesGraphicsI;

typedef CGAL::Aff_transformation_2<K> Transformation;


typedef tree<ExtendedPolygonPtr> PolyTree;

using std::endl; using std::cout;

// Helper functions

void print_tree(PolyTree * tree) {
  PolyTree::iterator sib2 = tree->begin();
  PolyTree::iterator end2 = tree->end();
  while(sib2!=end2) {
    for(int i=0; i < tree->depth(sib2)-2; ++i) 
      cout << " ";
    sib2->print_poly();
    ++sib2;
  }
}

// enum PolyOrientation { CONVEX, CONCAVE, FLAT };

// End Helper Functions


PolygonCalculate::PolygonCalculate() { }


int PolygonCalculate::find_and_add(PolyTree * tree, PolyTree::iterator curr_node, 
  PolygonWithHolesPtr p, int depth) {

  int limDepth = 25;
  int lOffset = 3;
  // depth reached
  if(depth > limDepth) return 0;

  cout << "Searching Depth: " << depth << endl;

  PolygonWithHolesPtrVector offset_poly_wh = 
    CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2(lOffset, *p);

  // no more polys
  if(offset_poly_wh.size() <= 0) return 0;
  int next_depth = ++depth;
  int leaf = 0;
  for(std::vector<PolygonWithHolesPtr>::iterator i = offset_poly_wh.begin(); i != offset_poly_wh.end(); ++i) {
    Polygon_2 outer = (**i).outer_boundary();
    ExtendedPolygonPtr poly_element = ExtendedPolygonPtr(outer);
    PolyTree::iterator next_node = tree->append_child(curr_node, poly_element);
    //recursive iteration to next node
    leaf++;
    cout << "Leaf: " << leaf << " Depth: " << depth << endl;
    this->find_and_add(tree, next_node, *i, next_depth);
  }
  // print_tree(tree);
  return 1;
}


void PolygonCalculate::run_program(int argc, char** argv, PolygonWindow* window) {

  this->window = window;

  cout << endl << "Welcome to the Pathfinder. Finding a path through the dark since 1999." << 
    endl << "  (c) BeachBot Productions LLC. ";

  
  this->plgi = new PolylinesGraphicsI(&this->poly_connector_lines);
  
  this->round_corners_gi = new PolylinesGraphicsI(&this->round_corners_lines);

  this->straight_skel_gi = new PolylinesGraphicsI(&this->straight_skel_lines);

  // QObject::connect(window, SIGNAL(changed()),
  //                  this->plgi, SLOT(modelChanged()));


  QColor pen_color(255, 0, 0);
  QPen pen(pen_color, 3);
  pen.setCosmetic(false);
  // sgi->setEdgesPen(pen);
  // sgi->setVerticesPen(pen);
  // sgi = new CGAL::Qt::SegmentsGraphicsItem<std::list<Segment_2> >(&connector_lines);
  this->plgi->setEdgesPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  this->round_corners_gi->setEdgesPen(QPen(Qt::red, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  this->straight_skel_gi->setEdgesPen(QPen(Qt::green, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  window->addItem(this->plgi);
  window->addItem(this->straight_skel_gi);
  window->addItem(this->round_corners_gi);
  this->straight_skel_gi->show();
  this->round_corners_gi->show();

  if( argc > 1 ) {
    std::string filename = argv[1];
    std::ifstream input_stream(filename.c_str());
    if(input_stream) {
      input_stream >> polygon_wh;
      this->phgi = new PolygonWithHolesGraphicsI(&polygon_wh);
      this->phgi->show();
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

  SSPtr straight_skel = CGAL::create_interior_straight_skeleton_2(polygon_wh);
  
  typedef typename SS::Halfedge_const_iterator Halfedge_const_iterator ;
  typedef typename SS::Halfedge_const_handle   Halfedge_const_handle ;
  
  for(Halfedge_const_iterator hit = straight_skel->halfedges_begin(); hit != straight_skel->halfedges_end(); ++hit)
  {
    Halfedge_const_handle h = hit ;

    if( h->is_bisector() && 
        ((h->id()%2)==0) && 
        !h->has_infinite_time() && 
        !h->opposite()->has_infinite_time() )
    {
      this->addLine(h->vertex()->point(), 
                    h->opposite()->vertex()->point(), 
                    &this->straight_skel_lines);
    }
  }
  this->straight_skel_gi->modelChanged();

  // iterate and draw straight skeleton

  this->outer_poly_ptr = PolygonWithHolesPtr(&(this->polygon_wh));

  // new code for recursive tree
  ExtendedPolygonPtr top_ptr = ExtendedPolygonPtr(polygon_wh.outer_boundary());
  this->p_tree = PolyTree(top_ptr);
  find_and_add(&this->p_tree, p_tree.begin(), outer_poly_ptr, 0);
  print_tree(&this->p_tree);
  PolyTree::post_order_iterator test_node = this->p_tree.begin_post();

  connect(test_node, NULL);

  // Call model changed to update the bounding box! IMPORTANT!
  this->plgi->modelChanged();

  // this->plgi->boundingRect()->top();

  PolyTree::iterator node = p_tree.begin();
  PolyTree::iterator end = p_tree.end();
  while(node != end) {
    node->set_graphx();
    this->window->addItem(node->graphx);
    ++node;
  }
}

int PolygonCalculate::addLine(Point_2 from, Point_2 to, std::list<std::list<Point_2> > * lines_list) {
  std::list<Point_2> list;// = std::list<Point_2>();
  list.push_back(from);
  list.push_back(to);
  lines_list->push_back(list);
}


Point_2 PolygonCalculate::find_closest_point_on_poly(Point_2 exit_point, Polygon_2 p2) {
  Polygon_2::Vertex_const_iterator p2_begin = p2.vertices_begin();
  Polygon_2::Vertex_const_iterator p2_end = p2.vertices_end();
  float distance, temp_dist;
  Point_2 point2 = *p2_begin;
  distance = CGAL::squared_distance(exit_point, *p2_begin);
  for(;p2_begin != p2_end; ++p2_begin) {
    temp_dist = CGAL::squared_distance(exit_point, *p2_begin);
    if(temp_dist < distance) {
      distance = temp_dist;
      point2 = *p2_begin;
    }
  }
  return point2;
}


int PolygonCalculate::connect(PolyTree::iterator node, PolyTree::iterator connect_from) {
  // Recursive algorithm to connect all Polygons
  // Steps:
  // 1. Find a "kernel" from where the BeachBot ideally should start
  // 2. Drive kernel, then move on one stage further out
  // 3. Check number of "undriven" polygons inside the one we're at
  // 4. If more than 0 find the closest point to enter (from where we are)
  //    Entering should happen while driving on the polygon where at
  // 5. Drive inside again and repeat step 1 - 5
 
  // There are no "half-driven" Polygons at the moment
  // No state keeping etcs

	PolyTree::sibling_iterator child_it = this->p_tree.child(node, 0);
	if(child_it == this->p_tree.end()) { // invalid pointer == no children
		connect_from->to = node;
    node->from = connect_from;
    cout << "Connecting " << &connect_from << " to " << &node << endl;
		node->visited = true;


    // this->addLine(node->poly[0], connect_from->poly[0]);
    this->connect(this->p_tree.parent(node), node);

    Point_2 next_entry = this->find_closest_point_on_poly(connect_from->entry_point, node->poly);
    node->entry_point = next_entry;
    cout << next_entry;
    this->addLine(next_entry, connect_from->entry_point, &this->poly_connector_lines);

		return 1;
	}
	for(;child_it != child_it.end();++child_it) {
    cout << "child visited?" << endl;
    if(child_it->unvisited()) {
      cout << "unvisited child !" << endl;
			this->connect(child_it, connect_from);
      return 1;
		}
	}
	// Always starting at a kernel
	// first one is NULL
	if(connect_from != NULL) {
		connect_from->to = node;
    node->from = connect_from;
    cout << "Connecting " << &connect_from << " to " << &node << endl;
    Point_2 next_entry = this->find_closest_point_on_poly(connect_from->entry_point, node->poly);
    node->entry_point = next_entry;

    Line_2 line = Line_2(connect_from->entry_point, next_entry);
    // this->checkPolyIntersection(line);

    this->addLine(next_entry, connect_from->entry_point, &this->poly_connector_lines);

    // this->addLine(node->poly[0], connect_from->poly[0]);
	} else {
    // first entry point randomly on polygon
    node->entry_point = node->poly[0];
  }
	node->visited = true;
	if(node == this->p_tree.begin()) {
		return 1;
	} else {
		return this->connect(this->p_tree.parent(node), node);
	}
}


float PolygonCalculate::calc_angle(Vector_2 v1, Vector_2 v2) {
  cout << "Angle " << v1*v2 << " deg " << acos(v1 * v2) << endl;
  return acos(v1 * v2);
}

int PolygonCalculate::find_orientation(Point_2 p1, Point_2 p2, Point_2 p3) {
  // https://en.wikipedia.org/wiki/Curve_orientation
  // Afaik polygons = all anti-clockwise
  Matrix3f m;
  m(0, 0) = 1;
  m(1, 0) = 1;
  m(2, 0) = 1;

  m(0, 1) = p1.x();
  m(0, 2) = p1.y();
  m(1, 1) = p2.x();
  m(1, 2) = p2.y();
  m(2, 1) = p3.x();
  m(2, 2) = p3.y();

  float det = m.determinant();
  cout << "Determinante = " << det << endl;
  if(det < 0) {
    // concave
    return -1;
  } else if(det > 0) {
    // convex
    return 1;
  }
  return 0;
}

void PolygonCalculate::checkPolyIntersection(Line_2 line) {
  // for(ExtendedPolygonPtr p: this->p_tree) {
  //   Polygon_2::Edge_const_iterator begin = p.poly.edges_begin();
  //   Polygon_2::Edge_const_iterator end = p.poly.edges_end();
  //   for(;begin != end; ++begin) {
  //     auto result = intersection(line, *begin);
  //     cout << "Intersection: " << endl;
  //   }
  // }
}

char * PolygonCalculate::exportToString() {
  std::string ret;

}

void PolygonCalculate::round_corners(float r) {
  // Algorithm to round corners of polygons
  // Two possible solution possibiliteis:
  // 1.  Circle with radius at corner (at the moment implemented)
  // 2.  Interprete corner as spline control point
  //     And use 2 other points on edge as origin points

  PolyTree::iterator it_node = this->p_tree.begin();
  Polygon_2::Vertex_const_iterator p_begin = it_node->poly.vertices_begin();
  Polygon_2::Vertex_const_iterator p_end = it_node->poly.vertices_end();
  

  Transformation rotate_90(CGAL::ROTATION, sin(M_PI/2), cos(M_PI/2)); 
  Transformation rotate_m90(CGAL::ROTATION, sin(-M_PI/2), cos(-M_PI/2)); 
  
  int len = it_node->poly.size();
  for(int i = 0; i <= len; ++i ) {

    Point_2 p1 = it_node->poly[i ? i-1 : len - 1];
    Point_2 p2 = it_node->poly[i];
    Point_2 p3 = it_node->poly[i+1 == len ? 0 : i + 1];

    Vector_2 v1 = Vector_2(p1, p2);
    Vector_2 v1_n = v1 / sqrt(v1.squared_length());

    Vector_2 v2 = Vector_2(p3, p2);
    Vector_2 v2_n = v2 / sqrt(v2.squared_length());

    float angle = this->calc_angle(v1_n, v2_n);
    float counter_angle = M_PI - angle;

    float l_down = r / tan(angle / 2);
    float interpolate_angle = 0.01 * M_PI; 
    Vector_2 v1_m;
    // code for inside
    Vector_2 rotate_rad;
    if(this->find_orientation(p1, p2, p3) > 0) {
      v1_m = (p2 - CGAL::ORIGIN) - (v1_n * l_down) - (v1_n.transform(rotate_m90) * r);
      rotate_rad = ((p2 - CGAL::ORIGIN) - v1_n * l_down) - v1_m;
    } else {
      v1_m = (p2 - CGAL::ORIGIN) - (v2_n * l_down) - (v2_n.transform(rotate_m90) * r);
      rotate_rad = ((p2 - CGAL::ORIGIN) - v2_n * l_down) - v1_m;
    }

    std::list<Point_2> list;// = std::list<Point_2>();
    
    // if(this->find_orientation(p1, p2, p3) > 0) {
    //   midpoint = (v1_s - v1_s.transform(rotate_90));
    //   pm = it_node->poly[i] + midpoint;
    // } else {
    //   midpoint = (v1_s - v1_s.transform(rotate_m90));      
    //   pm = it_node->poly[i] - midpoint;
    // }
    int len_interpol = floor(counter_angle / interpolate_angle);
    cout << angle << " " << counter_angle << " " <<  interpolate_angle << endl;
    for(float i = 0; i < counter_angle; i += counter_angle / len_interpol) {
      Transformation rot(CGAL::ROTATION, sin(i), cos(i)); 
      Vector_2 v_temp = rotate_rad.transform(rot);
      list.push_back(CGAL::ORIGIN + (v1_m + v_temp));
      cout << "Iterating over rounded edge " << v_temp.x() << " " << v_temp.y() << endl;
    }

    cout << "pushing back lines" << endl;
    this->round_corners_lines.push_back(list);
  }
  this->round_corners_gi->modelChanged();

  // this->calc_angle(it_node[len], it_node[i], it_node[i + 1]);

}

void PolygonCalculate::toggle_sgi(int value) {
  cout << "Toggling SGI: " << value << endl;
  if(value)
    this->straight_skel_gi->show();
  else
    this->straight_skel_gi->hide();
}