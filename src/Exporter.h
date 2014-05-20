// SVG Output
#pragma once
#include "CGAL_Headers.h"
#include "VectorTreeElements.h"
#include "GlobalOptions.h"
#include <boost/format.hpp>
#include <algorithm>
using boost::format; using boost::str;

// enum Rake {
//   RAKE_ZERO = 0,
//   RAKE_SMALL = 0 | 1 << 3,
//   RAKE_MEDIUM = 0x1c,
//   RAKE_LARGE = 0x3e, 
//   RAKE_FULL = 0x7f
// };


class Exporter {
private:
	float rake_to_disp(unsigned char r) { // uint8 = rakeinformation
 		switch(r) {
			case Rake::RAKE_ZERO: return 0.5;
			case Rake::RAKE_SMALL: return 0.08 * scale_for_disp;
			case Rake::RAKE_MEDIUM: return 0.20 * scale_for_disp;
			case Rake::RAKE_LARGE: return 0.30 * scale_for_disp;
			case Rake::RAKE_FULL: return 0.43 * scale_for_disp;

		}
	}

	PolyLine_P * export_poly;
	RakeVector * export_rake;
	std::vector<Point_2> * turn_points;
	double scale_for_disp;
	// VectorElementTree * tree;
public:
	Exporter(PolyLine_P * export_poly, RakeVector * export_rake, std::vector<Point_2> * turn_points) : 
		export_poly(export_poly),
		export_rake(export_rake),
		turn_points(turn_points) {
			 scale_for_disp = GlobalOptions::getInstance().scale_for_disp;
		};
	void export_result() {
		cout << "Exporting to Files" << endl;
		if(!GlobalOptions::getInstance().SVG_export_filename.empty()) {
			std::string fn = GlobalOptions::getInstance().SVG_export_filename;
			cout << "Writing to SVG: " << fn << endl;
			ofstream of(fn);
			std::string res = "";
			res += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> \n";
			res += "<!-- Created by Wolf Vollprechts super awesome path parser --> \n";
			res += "<svg \n";
			res += "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" ";
			res += "xmlns:cc=\"http://creativecommons.org/ns#\" ";
			res += "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" ";
			res += "xmlns:svg=\"http://www.w3.org/2000/svg\" ";
			res += "xmlns=\"http://www.w3.org/2000/svg\" ";
			res += "xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\" ";
			res += "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" \n";
			res += str(format("width=\"%1%\" height=\"%2%\" \n") 
				% (GlobalOptions::getInstance().field_width * scale_for_disp)
				% (GlobalOptions::getInstance().field_height * scale_for_disp));
			res += "id=\"svg_output\" \n";
			res += str(format("scale_for_disp=\"%1%\" \n") % scale_for_disp);
			res += "rake_info=\"";
			for(int i = 0; i < export_rake->size(); ++i) {
				res += str(format("%1% ") % (int)export_rake->at(i));
			}
			res += "\"\nversion=\"1.1\"> ";
			Transformation t(CGAL::SCALING, scale_for_disp);
			int prev_rake = export_rake->at(0);
			std::string stroke_color = "#000000";
			if(prev_rake == Rake::RAKE_ZERO) stroke_color = "#FF0000";
			res += str(format("<path manually_modified=\"true\" style=\"fill:none;stroke:%1%;stroke-width:%2%;\" d=\"M ") 
				% stroke_color
				% rake_to_disp(export_rake->at(0)));
			for(int i = 0; i < export_poly->size(); ++i) {
				Point_2 p = export_poly->at(i);
				Point_2 pn = p.transform(t);
				res += str(format("%1%,%2% L ") % pn.x() % pn.y());
				if(prev_rake != export_rake->at(i)) {
					res.resize(res.size() - 3);
					prev_rake = export_rake->at(i);
					if(prev_rake == Rake::RAKE_ZERO) stroke_color = "#FF0000";
					else stroke_color = "#000000";
					res += str(format("\" />\n<path style=\"fill:none;stroke:%1%;stroke-width:%2%;\" d=\"M %3%,%4% L ")
						% stroke_color
						% rake_to_disp(export_rake->at(i))
						% pn.x() % pn.y()
						);
				}
			}
			res.resize(res.size() - 3); // delete last three chars (space and last L)
			res += "\" /></svg>";
			of << res;
		}
		if(!GlobalOptions::getInstance().TXT_export_filename.empty()) {
			std::string res;
			std::string fn = GlobalOptions::getInstance().TXT_export_filename;

			cout << "Writing to File: " << fn << endl;
			ofstream of(fn);
			assert(export_poly->size() == export_rake->size());
			if(turn_points->size() == 0) {
				for(int i = 0; i < export_poly->size(); ++i) {
				  res += str(format("%1% %2% %3%\n") % export_poly->at(i).x() % export_poly->at(i).y() % (int)export_rake->at(i));
				}
			} else {
				for(int i = 0; i < export_poly->size(); ++i) {
				  res += str(format("%1% %2% %3%\n") % (*export_poly)[i].x() % (*export_poly)[i].y() % (int)export_rake->at(i));
				  if(std::find(turn_points->begin(), turn_points->end(), (*export_poly)[i]) != turn_points->end()) {
				  	res += "\n";
				  }
				}
			}
			of << res;

		}
	}
};