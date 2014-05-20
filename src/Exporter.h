// SVG Output
#pragma once
#include "CGAL_Headers.h"
#include "VectorTreeElements.h"
#include "GlobalOptions.h"
#include <boost/format.hpp>
#include <algorithm>
#include <CGAL/aff_transformation_tags.h>
using boost::format; using boost::str;

class Exporter {
private:


	PolyLine_P * export_poly;
	RakeVector * export_rake;
	std::vector<Point_2> * turn_points;
	// VectorElementTree * tree;
public:
	Exporter(PolyLine_P * export_poly, RakeVector * export_rake, std::vector<Point_2> * turn_points) : 
		export_poly(export_poly),
		export_rake(export_rake),
		turn_points(turn_points) {};
	void export_result() {
		cout << "Exporting to Files" << endl;
		if(!GlobalOptions::getInstance().SVG_export_filename.empty()) {
			double scale_for_disp = GlobalOptions::getInstance().scale_for_disp;
			std::string fn = GlobalOptions::getInstance().SVG_export_filename;
			cout << "Writing to SVG: " << fn << endl;
			ofstream of(fn);
			std::string res = "";
			res += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> ";
			res += "<!-- Created by Wolf Vollprechts super awesome path parser --> ";
			res += "<svg ";
			res += "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" ";
			res += "xmlns:cc=\"http://creativecommons.org/ns#\" ";
			res += "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" ";
			res += "xmlns:svg=\"http://www.w3.org/2000/svg\" ";
			res += "xmlns=\"http://www.w3.org/2000/svg\" ";
			res += "xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\" ";
			res += "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" ";
			res += str(format("width=\"%1%\" height=\"%2%\" ") 
				% (GlobalOptions::getInstance().field_width * scale_for_disp)
				% (GlobalOptions::getInstance().field_height * scale_for_disp));
			res += "id=\"svg_output\" ";
			res += str(format("scale_for_disp=\"%1%\" ") % scale_for_disp);
			res += "version=\"1.1\"> ";
			Transformation t(CGAL::SCALING, scale_for_disp);
			res += "<path d=\"M ";
			for(int i = 0; i < export_poly->size(); ++i) {
				Point_2 p = export_poly->at(i);
				Point_2 pn = p.transform(t);
				res += str(format("%1%,%2% L ") % pn.x() % pn.y());
			}
			res.resize(res.size() - 3); // delete last three chars (space and last L)
			
			res += "\" style=\"fill: none; stroke: black; stroke-width: 2\" rake_info=\"";
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

			double field =(double)GlobalOptions::getInstance().field_height;// / (double)2;

			assert(export_poly->size() == export_rake->size());
			if(turn_points->size() == 0) {
				for(int i = 0; i < export_poly->size(); ++i) {
				  res += str(format("%1% %2% %3%\n") % (double) export_poly->at(i).x() % (double) (export_poly->at(i).y() * -1 +field) % (int)export_rake->at(i));
				}
			} else {
				cout << "Exporting with Turn Points" << endl;
				for(int i = 0; i < export_poly->size(); ++i) {
				  res += str(format("%1% %2% %3%\n") % (double) export_poly->at(i).x() % (double) (export_poly->at(i).y() * -1 + field) % (int)export_rake->at(i));
				  if(std::find(turn_points->begin(), turn_points->end(), (*export_poly)[i]) != turn_points->end()) {
				  	res += "\n";
				  }
				}
			}
			of << res;
		}
	}
};