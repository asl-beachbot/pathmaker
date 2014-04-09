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

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <QtGui>

#include <CGAL/Qt/GraphicsViewNavigation.h>

#include "polygon_calculate.h"
#include "view.h"

// defining types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2  Point_2;


using std::endl; using std::cout;

int PolygonWindow::initWindow(int argc, char** argv) {

  setWindowTitle("This is a skeletal test.");
  // mouse_xycoord = new QLabel("0.00, 0.00", this);

  QApplication::setGraphicsSystem("opengl");
  //view->show();
  //Ui::MainWindow ui;
  ui.setupUi(this);
  this->show();

  navigation = new CGAL::Qt::GraphicsViewNavigation();
  ui.view->installEventFilter(navigation);
  ui.view->viewport()->installEventFilter(navigation);

  QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
                   ui.mouse_xy, SLOT(setText(QString)));

  // QObject::connect(
  //     ui.checkBox, SIGNAL(stateChanged(int)),
  //     this, SLOT(acceptValueFromCheckbox(int))
  // );
  QObject::connect(
      ui.ssCheckBox, SIGNAL(stateChanged(int)),
      this, SLOT(toggleStraightSkeleton(int))
  );
  QObject::connect(
      ui.roundCornersButton, SIGNAL(released()),
      this, SLOT(buttonRoundCorners())
  );
  QObject::connect(
      ui.exportButton, SIGNAL(released()),
      this, SLOT(exportButton())
  );
  QObject::connect(
      ui.doubleSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(spinBoxValueChanged(double))
  );
  ui.view->setRenderHint(QPainter::Antialiasing);
  ui.view->setAcceptDrops(false);
 
  scene.setItemIndexMethod(QGraphicsScene::NoIndex);

  ui.view->setScene(&scene);
  ui.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //this->setCentralWidget(this->view);
  ui.view->show();
  // this->polycalc = PolygonCalculate();
  // this->polycalc.run_program(argc, argv, this);

}

void PolygonWindow::addItem(QGraphicsItem* item) {
  scene.addItem(item);
  ui.view->show();
}

void PolygonWindow::spinBoxValueChanged(double i) {
  cout << " asasd " << i << endl; 
}

void exportButton() {
  cout << "Displaying Export Text" << endl;
  this->polycalc->exportToString();
}

void PolygonWindow::toggleStraightSkeleton(int value) {
  cout << " Hide the skeleton " << value << endl;
  this->polycalc->toggle_sgi(value);
}

// void PolygonWindow::acceptValueFromCheckbox(int value) {
//   cout << " test2 " << endl;
// }

void PolygonWindow::buttonRoundCorners() {
  cout << "Calculating corners" << endl;
  this->polycalc->round_corners((float)this->ui.doubleSpinBox->value());
}
// constructor
PolygonWindow::PolygonWindow(QWidget* parent) : QMainWindow(parent){
}
