// view.h
#include <QMainWindow>
#include <QtGui>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2 Point_2;

using std::vector;


class PolygonWindow : public QMainWindow {
public:
  int initWindow(int argc, char** argv);
  PolygonWindow(QWidget* parent = 0);
  void setupStatusBar();
  void addItem(QGraphicsItem* item);
  void addLine(Point_2 p1, Point_2 p2);
private:
  QGraphicsScene scene;
  CGAL::Qt::GraphicsViewNavigation* navigation;

  QGraphicsView* view;
  QLabel* mouse_xycoord;
//signals:
};
