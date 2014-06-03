// Connector Implementation
#pragma once
#include "VectorElementTree.h"
#include "CGAL_Headers.h"

typedef  VectorElementTree::Tree_ElementPtr::iterator TreeIterator;
typedef  VectorElementTree::Tree_ElementPtr Tree_ElementPtr;
typedef  VectorElementTree Tree;

class Connector {
protected:
  Tree * tree;
  Tree_ElementPtr * element_tree;
public:
  Connector(VectorElementTree * vet) : tree(vet) {
    element_tree = &(vet->element_tree);
  };
  virtual void connect() = 0;
  void print_connections();
};

class SimpleConnector : public Connector {
private:
  Point_2 find_closest_point_on_element(Point_2 exit_point, ElementPtr * p2, bool connect);
  int connect_recursive(TreeIterator node, TreeIterator connect_from);
public:
  virtual void connect();
  SimpleConnector(VectorElementTree * vet) : Connector(vet) {
  }
};