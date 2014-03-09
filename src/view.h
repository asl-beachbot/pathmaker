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

// view.h
#pragma once
#include <QObject>
#include <QMainWindow>
#include <QtGui>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <ui_pathfinder.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2 Point_2;

using std::vector;


class PolygonWindow : public QMainWindow {
  Q_OBJECT
public:
  int initWindow(int argc, char** argv);
  PolygonWindow(QWidget* parent = 0);
  void setupStatusBar();
  void addItem(QGraphicsItem* item);
  void addLine(Point_2 p1, Point_2 p2);
  void addLine(float x1, float y1, float x2, float y2);
private:
  QGraphicsScene scene;
  CGAL::Qt::GraphicsViewNavigation* navigation;
  Ui::MainWindow ui;
  QLabel* mouse_xycoord;
public slots:
  void acceptValueFromCheckbox(int value);
};