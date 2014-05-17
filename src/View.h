// View.h

#ifdef WITH_GUI
#include <QObject>
#include <QMainWindow>
#include <QtGui>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <ui/ui_pathfinder.h>

class View : public QMainWindow {
public:
  View(QWidget* parent = 0);
  void addItem(QGraphicsItem* item);
  int initWindow();
  void addText(char * txt, int x, int y);
private:
  QGraphicsTextItem * text;
  std::list< QGraphicsTextItem * > textItems;
  Ui::MainWindow ui;
  QGraphicsScene scene;
  CGAL::Qt::GraphicsViewNavigation* navigation;
};

#endif
