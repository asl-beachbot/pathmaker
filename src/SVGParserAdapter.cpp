// Parsed SVG 
#include "python_wrapper.h"

#include <iostream>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <string>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2 Point_2;

using namespace std;
namespace bp = boost::python;


class VectorElement {
public:
  bool closed;
  bool filled;
  std::list<Point_2> vertices;
  void repr() {
    cout << "Closed: " << closed << endl <<
            "Filled: " << filled << endl <<
            "Points: " << endl;
    for(Point_2 p : vertices) {
      cout << p << endl;
    }
  }
//   VectorElement(std::list<Point_2> vertices,
//     closed = false, filled = false) {
//     this->vertices = vertices;
//     this->closed = closed;
//     this->filled = filled;
//   }
//   Polygon_2 getElement () {
//     if(closed) {
//       return Polygon_2(vertices.begin(), vertices.end());
//     }
//   }
};

class ParsedSVG {
public:
  float width, height;
  std::list<VectorElement> elements;
  void repr() {
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
  void parseSVGFile(std::string filename) {
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
        this->elements.push_back(ve);
      }
      this->repr();
      // int l = bp::len(result.keys());
      // for(int i = 0; i < l; i++) {
      //   bp::object o = result.keys()[i];
      //   std::string k = bp::extract<std::string>(o);
      //   cout << k << endl;
      // }
      // cout << "::: Imported DAT ::: "<< endl << result << endl;
      // return dat;
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
};

int main() {
  ParsedSVG * ps = new ParsedSVG();
  ps->parseSVGFile("assets/2.svg");
  delete ps;
}