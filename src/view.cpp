// Copyright (C) 2014  Wolf Vollprecht

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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
  //Ui::MainWindow ui;
  ui.setupUi(this);
  this->show();

  navigation = new CGAL::Qt::GraphicsViewNavigation();
  ui.view->installEventFilter(navigation);
  ui.view->viewport()->installEventFilter(navigation);

  QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
                   mouse_xycoord, SLOT(setText(QString)));

  QObject::connect(
      ui.checkBox, SIGNAL(stateChanged(int)),
      this, SLOT(acceptValueFromCheckbox(int))
      );
  ui.view->setRenderHint(QPainter::Antialiasing);
  ui.view->setScene(&scene);
  ui.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //this->setCentralWidget(this->view);
  ui.view->show();
}

void PolygonWindow::setupStatusBar() {
  this->statusBar()->addWidget(new QLabel(this), 1);
  this->statusBar()->addWidget(mouse_xycoord, 0);
}

void PolygonWindow::addLine(float x1, float y1, float x2, float y2) {
  scene.addLine(QLineF(x1, y1, x2, y2), QPen(Qt::green));
}
void PolygonWindow::addLine(Point_2 p1, Point_2 p2) {
  scene.addLine(QLineF(p1.x(), p1.y(), p2.x(), p2.y()), QPen(Qt::green));
}

void PolygonWindow::addItem(QGraphicsItem* item) {
  scene.addItem(item);
  ui.view->show();
}
void PolygonWindow::acceptValueFromCheckbox(int value) {
  cout << "the fucking valuellll" << value << endl;
}
// constructor
PolygonWindow::PolygonWindow(QWidget* parent) : QMainWindow(parent){
  setWindowTitle("This is a skeletal test.");
  mouse_xycoord = new QLabel("0.00, 0.00", this);
}
