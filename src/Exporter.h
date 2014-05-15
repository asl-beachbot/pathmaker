// SVG Output
#pragma once
#include "VectorTreeElements.h"
#include "GlobalOptions.h"
#include <boost/format.hpp>
using boost::format; using boost::str;

class Exporter {
private:
	PolyLine_P * export_poly;
	RakeVector * export_rake;
	// VectorElementTree * tree;
public:
	Exporter(PolyLine_P * export_poly, RakeVector * export_rake) : 
		export_poly(export_poly),
		export_rake(export_rake) {};
	void export_result() {
		if(!GlobalOptions::getInstance().SVG_export_filename.empty()) {
			std::string fn = GlobalOptions::getInstance().SVG_export_filename;
			ofstream of(fn);
			std::string res = "<svg width=\"10m\" height=\"10m\">";
			res += "<path d=\"M ";
			for(int i = 0; i < export_poly->size(); ++i) {
				res += str(format("%1%,%2% L ") % export_poly->at(i).x() % export_poly->at(i).y());
			}
			res.resize(res.size() - 3);
			
			 // remove last 3 chars
			res += "\" style=\"fill: none; stroke: black; stroke-width: 0.2\" />";
			res += "</svg>";
			of << res;
		}
		if(!GlobalOptions::getInstance().TXT_export_filename.empty()) {
			std::string res;
			std::string fn = GlobalOptions::getInstance().TXT_export_filename;
			ofstream of(fn);
			assert(export_poly->size() == export_rake->size());
			for(int i = 0; i < export_poly->size(); ++i) {
			  res += str(format("%1% %2% %3%\n") % export_poly->at(i).x() % export_poly->at(i).y() % (int)export_rake->at(i));
			}
			of << res;
		}
	}
};