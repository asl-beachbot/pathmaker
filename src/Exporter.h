// SVG Output
#pragma once
#include "CGAL_Headers.h"
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
		cout << "Exporting to Files" << endl;
		if(!GlobalOptions::getInstance().SVG_export_filename.empty()) {
			std::string fn = GlobalOptions::getInstance().SVG_export_filename;
			ofstream of(fn);
			Transformation t(CGAL::SCALING, GlobalOptions::getInstance().scale_for_disp);
			std::string res = str(format("<svg width=\"%1%\" height=\"%2%\">") 
				% GlobalOptions::getInstance().field_width
				% GlobalOptions::getInstance().field_height);
			res += "<path d=\"M ";
			for(int i = 0; i < export_poly->size(); ++i) {
				Point_2 p = export_poly->at(i);
				p.transform(t);
				res += str(format("%1%,%2% L ") % p.x() % p.y());
			}
			res.resize(res.size() - 3);
			
			 // remove last 3 chars
			res += "\" style=\"fill: none; stroke: 'black'; stroke-width: 0.2\" rake_info=\"";
			for(int i = 0; i < export_rake->size(); ++i) {
				res += str(format("%1% ") % (int)export_rake->at(i));
			}
			res += "\" manually_modified=\"true\" /></svg>";
			of << res;
		}
		if(!GlobalOptions::getInstance().TXT_export_filename.empty()) {
			std::string res;
			std::string fn = GlobalOptions::getInstance().TXT_export_filename;
			cout << "Writing to File: " << fn << endl;
			ofstream of(fn);
			assert(export_poly->size() == export_rake->size());
			for(int i = 0; i < export_poly->size(); ++i) {
			  res += str(format("%1% %2% %3%\n") % export_poly->at(i).x() % export_poly->at(i).y() % (int)export_rake->at(i));
			}
			of << res;
		}
	}
};