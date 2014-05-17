#pragma once

#include <iostream>
#include <string>

#include "CGAL_Headers.h"

#include <boost/python.hpp>

using namespace std;
namespace bp = boost::python;


class VectorElement {
public:
  bool closed;
  bool filled;
  bool manually_modified;
  vector<char> rake_states;
  double stroke_width;
  list<Point_2> vertices;
  list< list<Point_2> >holes;
  void repr();
};

class ParsedSVG {
private:
  void extractPython(bp::dict result);
public:
  float width, height;
  list<VectorElement> elements;
  void repr();
  ParsedSVG() {

  };
  int parseSVGFile(string filename);
  int parseSVGString(string svg_xml_string);
};

