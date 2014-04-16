// Parsed SVG 
#include "python_wrapper.h"


class VectorElement {
public:
  bool closed;
  bool filled;
  std::list<Point_2> vertices;
  VectorElement(std::list<Point_2> vertices,
    closed = false, filled = false) {
    this->vertices = vertices;
    this->closed = closed;
    this->filled = filled;
  }
  Polygon_2 getElement () {
    if(closed) {
      return Polygon_2(vertices.begin(), vertices.end());
    }
  }
};

class ParsedSVG {
public:
  parseSVGFile(std::string filename) {

  }
}