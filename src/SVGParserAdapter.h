#pragma once

#include <iostream>
#include <string>

#include "CGAL_Headers.h"

#include <QObject>
#include <QMainWindow>
#include <QtGui>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <ui/ui_pathfinder.h>

using namespace std;

class View : public QMainWindow {
public:
  View(QWidget* parent = 0);
  void addItem(QGraphicsItem* item);
  int initWindow();
  void addText(char * txt, int x, int y) {
    // text = scene.addText("test", QFont("Arial", 10));
    printf("text: %s, %d, %d", txt, x, y);
    QGraphicsTextItem * text = scene.addText(QString(txt), QFont("Arial", 10));
    text->setPos(x, y);
    text->show();
    this->textItems.push_back(text);
  }
private:
  QGraphicsTextItem * text;
  std::list< QGraphicsTextItem * > textItems;
  Ui::MainWindow ui;
  QGraphicsScene scene;
  CGAL::Qt::GraphicsViewNavigation* navigation;
};

int View::initWindow() {
  setWindowTitle("Quick View Window");
  QApplication::setGraphicsSystem("opengl");
  ui.setupUi(this);
  this->show();

  navigation = new CGAL::Qt::GraphicsViewNavigation();
  ui.view->installEventFilter(navigation);
  ui.view->viewport()->installEventFilter(navigation);
  ui.view->setRenderHint(QPainter::Antialiasing);
  ui.view->setAcceptDrops(false);

  scene.setItemIndexMethod(QGraphicsScene::NoIndex);

  ui.view->setScene(&scene);
  ui.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  ui.view->show();
}
void View::addItem(QGraphicsItem* item) {
  scene.addItem(item);
  ui.view->show();
}
View::View(QWidget* parent) : QMainWindow(parent){
}

class VectorElement {
public:
  bool closed;
  bool filled;
  list<Point_2> vertices;
  list< list<Point_2> >holes;
  void repr();
};

class ParsedSVG {
public:
  float width, height;
  list<VectorElement> elements;
  void repr();
  void parseSVGFile(string filename);
};

