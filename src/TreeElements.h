// This class defines possible elements for the tree and 
// a factory ?

enum ElementType {
  POLYGON,
  FILLED_POLYGON,
  POLYLINE
};

class ElementPtr{
public:
  bool visited;

  Point_2 entry_point;
  Point_2 exit_point;
  int entry_point_index;
  int exit_point_index;

  ElementPtr * to;
  ElementPtr * from;

  ElementPtr() :  visited(false) {};

  virtual set_graphx() {};
  virtual get_type() {};

  bool unvisited() {
	  return !this->visited;
  }
};


class PolygonElementPtr : public ElementPtr {
  
  Polygon_2 element;

  int visited_vertices[];
  PolygonElementPtr(Polygon_2 poly) : poly(poly), visited(false) {};
  PolygonGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&poly);
    return;
  }

  ElementType get_type() { return ElementType.Polygon; }
  void print_poly() {
    std::cout << "Polygon " << this << " " << poly << std::endl;
  }

};

class FilledPolygonElementPtr : public ElementPtr {
  
  Polygon_2 element;

  int visited_vertices[];

  PolygonElementPtr(Polygon_2 poly) : poly(poly), visited(false) {};

  PolygonGraphicsI * graphx;

  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&poly);
    return;
  }
  ElementType get_type() { return ElementType.FilledPolygon; }
  void print_poly() {
    std::cout << "Polygon " << this << " " << poly << std::endl;
  }

};

class PolyLineElementPtr : public ElementPtr {
  
  std::list<Point_2>  polyline;

  int visited_vertices[];
  PolyLineElementPtr(std::list<Point_2> polyline) : polyline(polyline), visited(false) {};
  PolygonGraphicsI * graphx;
  void set_graphx() {
    this->graphx = new PolygonGraphicsI(&poly);
    return;
  }
  ElementType get_type() { return ElementType.POLYLINE; }

  void print_poly() {
    std::cout << "Polygon " << this << " " << poly << std::endl;
  }

};