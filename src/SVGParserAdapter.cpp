// Parsed SVG

#include "python_wrapper.h"

#include <iostream>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <string>

#include "VectorElementTree.h"
#include "SVGParserAdapter.h"
#include "SimpleConnector.h"

using namespace std;
namespace bp = boost::python;


void VectorElement::repr() {
  cout << "Closed: " << closed << endl <<
          "Filled: " << filled << endl <<
          "Points: " << endl;
  for(Point_2 p : vertices) {
    cout << p << endl;
  }
  cout << "Holes: " << endl;
  int i = 0;
  for(std::list<Point_2> h: holes) {
    cout << " - " << i << " - "<< endl;
    for(Point_2 p: h) {
      cout << p << endl;
    }
    ++i;
  }
}

void ParsedSVG::repr() {
  int i = 0;
  cout << "Width: " << width << endl
       << "Height: " << height << endl
       << "Elements" << endl;
  for(VectorElement e : elements) {
    cout << endl << "::: #" << i << " :::" << endl;
    e.repr();
    ++i;
  }
};
void ParsedSVG::parseSVGFile(std::string filename) {
  initializePython();
  try {
    bp::object svg_parser = bp::import("parse_svg");
    bp::object parse_fcn = svg_parser.attr("parse");
    bp::dict result = (bp::dict) parse_fcn(filename);
    this->width = bp::extract<float>(result["svg_base"]["width"]);
    this->height = bp::extract<float>(result["svg_base"]["height"]);
    bp::list elem_list = bp::extract<bp::list>(result["elements"]);
    int l = bp::len(elem_list);
    for(int i = 0; i < l; ++i) {
      VectorElement ve;
      ve.filled = bp::extract<bool>(elem_list[i]["filled"]);
      ve.closed = bp::extract<bool>(elem_list[i]["closed"]);
      bp::list coords_list = bp::extract<bp::list>(elem_list[i]["coords"]);
      int coords_len = bp::len(coords_list);
      for(int j = 0; j < coords_len; ++j) {
        ve.vertices.push_back(Point_2(
          bp::extract<float>(coords_list[j][0]),
          bp::extract<float>(coords_list[j][1])
          )
        );
      }
      bp::list holes_list = bp::extract<bp::list>(elem_list[i]["holes"]);
      int holes_len = bp::len(holes_list);
      for(int j = 0; j < holes_len; ++j) {
        int hole_len = bp::len(holes_list[j]);
        std::list<Point_2> hole_poly_list;
        for(int k = 0; k < hole_len; ++k) {
          hole_poly_list.push_back(Point_2(
            bp::extract<float>(holes_list[j][k][0]),
            bp::extract<float>(holes_list[j][k][1])
            )
          );
        }
        ve.holes.push_back(hole_poly_list);
      }
      this->elements.push_back(ve);
    }
    this->repr();
  }
  catch (bp::error_already_set) {
    std::string msg;
    if (PyErr_Occurred()) {
        msg = handle_pyerror(); 
    }
    bp::handle_exception();
    PyErr_Clear();
    cout << "::: ERROR ::: "<< endl << msg << endl;
    // return "ERROR";
  }
}


int main(int argc, char** argv) {
  ParsedSVG * ps = new ParsedSVG();
  ps->parseSVGFile("assets/2.svg");
  VectorElementTree * vet = new VectorElementTree;
  vet->createAndSortTree(ps);
  SimpleConnector * sc = new SimpleConnector(vet);
  sc->connect();
  QApplication app(argc, argv);
  if(argc > 1) {
    View * window = new View();
    window->initWindow();
    window->show();
    vet->addWindow(window);
    vet->drawTreeOnCanvas();
    vet->drawConnections();
    return app.exec();
  }

  delete ps;
  return 0;
}