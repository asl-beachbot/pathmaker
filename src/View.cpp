// View.cpp

#ifdef WITH_GUI
#include "View.h"
#include <QObject>
#include <QMainWindow>
#include <QtGui>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <ui/ui_pathfinder.h>

void View::addText(char * txt, int x, int y) {
  // text = scene.addText("test", QFont("Arial", 10));
  // printf("text: %s, %d, %d", txt, x, y);
  QGraphicsTextItem * text = scene.addText(QString(txt), QFont("Arial", 10));
  text->setPos(x, y);
  text->show();
  this->textItems.push_back(text);
}

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
  QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
                   ui.mouse_xy, SLOT(setText(QString)));
  ui.view->show();
}
void View::addItem(QGraphicsItem* item) {
  scene.addItem(item);
  ui.view->show();
}
View::View(QWidget* parent) : QMainWindow(parent){
}
#endif
