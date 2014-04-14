// This class defines possible elements for the tree and 
// a factory ?

class ElementPtr{
public:
  Polygon_2 poly;
  int visited_polys[];
  bool visited;

  Point_2 entry_point;
  int entry_point_index;
  ElementPtr(Polygon_2 poly) : poly(poly), visited(false) {};
  ElementPtr() :  visited(false) {};
  PolygonGraphicsI * graphx;
  ElementPtr * to;
  ElementPtr * from;
  void set_graphx() {
	  this->graphx = new PolygonGraphicsI(&poly);
	  return;
  }
  bool unvisited() {
	  return !this->visited;
  }
  void print_poly() {
    std::cout << "Polygon " << this << " " << poly << std::endl;
  }
};
