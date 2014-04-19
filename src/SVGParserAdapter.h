#pragma once

#include <iostream>
#include <string>

#include "CGAL_Headers.h"

using namespace std;

class VectorElement {
public:
  bool closed;
  bool filled;
  list<Point_2> vertices;
  list< list<Point_2> >holes;
  void repr();
};

class ParsedSVG {
public:
  float width, height;
  list<VectorElement> elements;
  void repr();
  void parseSVGFile(string filename);
};

