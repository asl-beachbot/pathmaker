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
