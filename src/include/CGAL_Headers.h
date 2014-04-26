#pragma once

#include <boost/shared_ptr.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2_algorithms.h>

#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>

#include <CGAL/squared_distance_2.h>
#include <CGAL/intersections.h>
#include <CGAL/partition_2.h>

#include <CGAL/Aff_transformation_2.h>

#include <CGAL/convex_hull_2.h>


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


typedef CGAL::Aff_transformation_2<K> Transformation;

typedef typename SS::Halfedge_const_iterator Halfedge_const_iterator ;
typedef typename SS::Halfedge_const_handle   Halfedge_const_handle ;

#ifdef WITH_GUI
#include <CGAL/Qt/PolygonGraphicsItem.h>
#include <CGAL/Qt/PolygonWithHolesGraphicsItem.h>
#include <CGAL/Qt/GraphicsViewNavigation.h>

#include "CustomPolylinesGraphicsItem.h"

typedef CGAL::Qt::PolygonWithHolesGraphicsItem<Polygon_with_holes_2> PolygonWithHolesGraphicsI;
typedef CGAL::Qt::PolygonGraphicsItem<Polygon_2> PolygonGraphicsI;
typedef CGAL::Qt::CustomPolylinesGraphicsItem<std::list<std::list<Point_2> > > PolylinesGraphicsI;

#endif
