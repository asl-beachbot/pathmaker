// view.cpp
#include "view.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <QtGui>

#include <CGAL/Qt/GraphicsViewNavigation.h>

// defining types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                    Point_2;


using std::endl; using std::cout;

int PolygonWindow::initWindow(int argc, char** argv) {
  QApplication::setGraphicsSystem("opengl");
  //view->show();
  scene.setSceneRect(0,0, 100, 100);
  scene.addRect(QRectF(0,0, 100, 100));
  scene.addLine(QLineF(0,0, 100, 100));
  scene.addLine(QLineF(0,100, 100, 0));
  this->view = new QGraphicsView(&scene);
  this->navigation = new CGAL::Qt::GraphicsViewNavigation();
  view->installEventFilter(navigation);
  view->viewport()->installEventFilter(navigation);

  QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
		   mouse_xycoord, SLOT(setText(QString)));


  view->setRenderHint(QPainter::Antialiasing);
  //this->view->setScene(&scene);
  this->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->setCentralWidget(this->view);
  this->view->show();
}

void PolygonWindow::setupStatusBar() {
  this->statusBar()->addWidget(new QLabel(this), 1);
  this->statusBar()->addWidget(mouse_xycoord, 0);
}

void PolygonWindow::addLine(Point_2 p1, Point_2 p2) {
  scene.addLine(QLineF(p1.x(), p1.y(), p2.x(), p2.y()), QPen(Qt::green));
}

void PolygonWindow::addItem(QGraphicsItem* item) {
  scene.addItem(item);
  view->show();
}

// constructor
PolygonWindow::PolygonWindow(QWidget* parent) : QMainWindow(parent){
  setWindowTitle("This is a skeletal test.");
  mouse_xycoord = new QLabel("0.00, 0.00", this);
}
