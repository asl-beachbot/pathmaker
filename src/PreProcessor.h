#pragma once
#include "CGAL_Headers.h"
#include <CGAL/bounding_box.h>

// This class is mainly used for scaling all the polys to 
// a certain bounding box!
class PreProcessor {
private:
	VectorElementTree * tree;
public:
	void process(double xmin_target, double ymin_target, double width_target, double height_target);
	void process(double scale, double trans_x = 0, double trans_y = 0);
	PreProcessor(VectorElementTree * tree) : tree(tree) {
	}
};