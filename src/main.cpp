/*     Pathfinder
 *
 *     Released under GPLv3
 *     Developed by Wolf Vollprecht
 *
 *     Usage:
 *      ./bin/Pathfinder ./assets/sample_1.dat 
 */

#include <iostream>
#include <vector>
#include <fstream>

#include <QApplication>

#include "view.h"
#include "polygon_calculate.h"

using std::endl; using std::cout;

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  PolygonWindow window;
  window.setupStatusBar();
  window.initWindow(argc, argv);
  window.show();
  PolygonCalculate polycalc;
  polycalc.run_program(argc, argv, &window);
  return app.exec();
}
