#pragma once

#include <iostream>
#include <string>

#include "CGAL_Headers.h"

// #include <QObject>
// #include <QMainWindow>
// #include <QtGui>
// #include <CGAL/Qt/GraphicsViewNavigation.h>
// #include <ui/ui_pathfinder.h>
#include <boost/python.hpp>

using namespace std;
namespace bp = boost::python;

class VectorElement {
public:
  bool closed;
  bool filled;
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
  void test() {
    cout << "asasd" << endl;
  }
  ParsedSVG() {

  };
  void parseSVGFile(string filename);
  void parseSVGString(string svg_xml_string);
};

