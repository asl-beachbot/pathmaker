 #include "PreProcessor.h"
#include "GlobalOptions.h"
#include "CGAL_Headers.h"
#include <CGAL/Bbox_2.h>
#include <CGAL/bounding_box.h>

typedef CGAL::Bbox_2 Bbox_2;


// This class is mainly used for scaling all the polys to 
// a certain bounding box!
void PreProcessor::process(double xmin_target, double ymin_target, double width_target, double height_target) {
	auto it = ++tree->element_tree.begin();
	auto it_end = tree->element_tree.end();
	Bbox_2 bb_final;
	Bbox_2 bb;
	int i = 0;
	for(; it != it_end; ++it) {
		switch((*it)->get_type()) {
			case EL_POLYGON: {
				Polygon_2 * p = &(static_cast<PolygonElementPtr *>(*it)->element);
				bb = CGAL::bounding_box(p->vertices_begin(), p->vertices_end()).bbox();
			}
			case EL_FILLED_POLYGON:{
				Polygon_2 * p = &(static_cast<FilledPolygonElementPtr *>(*it)->element.outer_boundary());
				bb = CGAL::bounding_box(p->vertices_begin(), p->vertices_end()).bbox();
			}
			case EL_POLYLINE: {
				PolyLine_P * p = &(static_cast<PolyLineElementPtr *>(*it)->element);
				bb = CGAL::bounding_box(p->begin(), p->end()).bbox();
			}
		}
		if(i == 0) {
			bb_final = bb;
		} else {
			bb_final = bb_final + bb;
		}
		i++;
	}
	double w = bb_final.xmax() - bb_final.xmin();
	double h = bb_final.ymax() - bb_final.ymin();
	cout << "BB: " << bb_final.xmax()<< " " << bb_final.xmin() << " " << bb_final.ymax() << " " << bb_final.ymin() << endl;

	if(GlobalOptions::getInstance().field_offset) {
		width_target -= 2 * GlobalOptions::getInstance().field_offset;
		height_target -= 2 * GlobalOptions::getInstance().field_offset;
	}
	double ws = (double)width_target / (double)w;
	double hs = (double)height_target / (double)h;
	double s = (ws < hs) ? ws : hs;
	cout << "Scale: " << s << endl;
	this->process(s, 
		bb_final.xmin() - GlobalOptions::getInstance().field_offset * 1 / s, 
		bb_final.ymin() - GlobalOptions::getInstance().field_offset * 1 / s); // -bb_final.xmin() -bb_final.ymin();
}

void PreProcessor::process(double scale, double trans_x, double trans_y) {
	Transformation trafo_scale(CGAL::SCALING, scale);
	Transformation trafo_trans(CGAL::TRANSLATION, Vector_2(-trans_x, -trans_y));
	Transformation trafo = trafo_scale * trafo_trans;
	// GlobalOptions::getInstance().tree_trafo = trafo;
	cout << "Transforming: " << trafo << endl;
	auto it = tree->element_tree.begin();
	auto it_end = tree->element_tree.end();

	for(; it != it_end; ++it) {
		switch((*it)->get_type()) {
			case EL_POLYGON: {
				Polygon_2 * p = &(static_cast<PolygonElementPtr *>(*it)->element);
				for(auto poly_it = p->vertices_begin(); poly_it != p->vertices_end(); ++poly_it) {
					*poly_it = poly_it->transform(trafo);
				}			
			}
			break;
			case EL_FILLED_POLYGON: { // Todo transform holes!
				FilledPolygonElementPtr * fp = static_cast<FilledPolygonElementPtr *>(*it);
				Polygon_with_holes_2 scaled_pwh(transform(trafo, fp->element.outer_boundary()));
				for(auto hit = fp->element.holes_begin(); hit != fp->element.holes_end(); ++hit) {
					scaled_pwh.add_hole(transform(trafo, (*hit)));
				}
				cout << "Scaled PWH = " << scaled_pwh << endl;
				fp->element = scaled_pwh;
			}
			break;
			case EL_POLYLINE: {
				PolyLine_P * p = &(static_cast<PolyLineElementPtr *>(*it)->element);
				auto poly_it = p->begin();
				auto poly_end = p->end();
				for(; poly_it != poly_end; ++poly_it) {
					*poly_it = poly_it->transform(trafo);
				}
			}
			break;
		}
	}
	if(tree->startpoint_elem) {
		PolyLine_P * p = &(static_cast<PolyLineElementPtr *>(tree->startpoint_elem)->element);
		auto poly_it = p->begin();
		auto poly_end = p->end();
		for(; poly_it != poly_end; ++poly_it) {
			*poly_it = poly_it->transform(trafo);
		}
	}
}