// Parsed SVG

// #include "python_wrapper.h"

#include <iostream>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <string>

#include "VectorElementTree.h"
#include "SVGParserAdapter.h"
#include "SimpleConnector.h"
#include "FillProcedures.h"
#include "SegmentationPreProcessor.h"
#include "PostProcessor.h"
#include "PreProcessor.h"
#include "GlobalOptions.h"
#include "PythonHelpers.h"


#ifdef STANDALONE
#endif

using namespace std;
namespace bp = boost::python;

void VectorElement::repr() {
  cout << "Closed: " << closed << endl <<
          "Filled: " << filled << endl <<
          "Points: " << endl;
  for(Point_2 p : vertices) {
    cout << p << endl;
  }
  cout << "Holes: " << endl;
  int i = 0;
  for(std::list<Point_2> h: holes) {
    cout << " - " << i << " - "<< endl;
    for(Point_2 p: h) {
      cout << p << endl;
    }
    ++i;
  }
}

void ParsedSVG::repr() {
  int i = 0;
  cout << "Width: " << width << endl
       << "Height: " << height << endl
       << "Elements" << endl;
  for(VectorElement e : elements) {
    cout << endl << "::: #" << i << " :::" << endl;
    e.repr();
    ++i;
  }
};

void ParsedSVG::extractPython(bp::dict result) {
  this->width = bp::extract<float>(result["svg_base"]["width"]);
  this->height = bp::extract<float>(result["svg_base"]["height"]);
  bp::extract<std::string>stroke_sizes(result["svg_base"]["rake_sizes"]);
  if(stroke_sizes.check()) GlobalOptions::getInstance().parseSVGStrokeSizes(stroke_sizes);
  bp::extract<float>rescale(result["svg_base"]["scale_for_disp"]);
  if(rescale.check()) GlobalOptions::getInstance().scale_back = rescale;
  bp::list elem_list = bp::extract<bp::list>(result["elements"]);
  int l = bp::len(elem_list);
  for(int i = 0; i < l; ++i) {
    VectorElement ve;
    ve.filled = bp::extract<bool>(elem_list[i]["filled"]);
    ve.closed = bp::extract<bool>(elem_list[i]["closed"]);
    ve.startpoint = bp::extract<bool>(elem_list[i]["startpoint"]);
    ve.manually_modified = bp::extract<bool>(elem_list[i]["manually_modified"]);
    ve.stroke_width = bp::extract<double>(elem_list[i]["stroke_width"]);
    if(!(elem_list[i]["rake_states"]) == boost::python::api::object() ) {
      cout << "Reading Rake States" << endl;
      for(int j = 0; j < bp::len(elem_list[i]["rake_states"]); ++j) {
        ve.rake_states.push_back(bp::extract<char>(elem_list[i]["rake_states"][j]));
      }
    }
    bp::list coords_list = bp::extract<bp::list>(elem_list[i]["coords"]);
    int coords_len = bp::len(coords_list);
    for(int j = 0; j < coords_len; ++j) {
      ve.vertices.push_back(Point_2(
        bp::extract<float>(coords_list[j][0]),
        bp::extract<float>(coords_list[j][1])
        )
      );
    }
    bp::list holes_list = bp::extract<bp::list>(elem_list[i]["holes"]);
    int holes_len = bp::len(holes_list);
    for(int j = 0; j < holes_len; ++j) {
      int hole_len = bp::len(holes_list[j]);
      std::list<Point_2> hole_poly_list;
      for(int k = 0; k < hole_len; ++k) {
        hole_poly_list.push_back(Point_2(
          bp::extract<float>(holes_list[j][k][0]),
          bp::extract<float>(holes_list[j][k][1])
          )
        );
      }
      ve.holes.push_back(hole_poly_list);
    }
    this->elements.push_back(ve);
  }
  this->repr();
}

int ParsedSVG::parseSVGFile(std::string filename) {
  initializePython();
  try {
    bp::object svg_parser = bp::import("parse_svg");
    bp::object parse_fcn = svg_parser.attr("parse_file");
    bp::dict result = (bp::dict) parse_fcn(filename);
    extractPython(result);
  }
  catch (bp::error_already_set) {
    std::string msg;
    if (PyErr_Occurred()) {
        msg = handle_pyerror(); 
    }
    bp::handle_exception();
    PyErr_Clear();
    cout << "::: ERROR ::: "<< endl << msg << endl;
    cout << "\n\nMAYBE THE SVG FILE WAS EMPTY OR DOES NOT EXIST?" << endl;
    return 1;
  }
  return 0;
}

int ParsedSVG::parseSVGString(std::string svg_xml_string) {
  initializePython();
  try {
    bp::object svg_parser = bp::import("parse_svg");
    bp::object parse_fcn = svg_parser.attr("parse_string");
    bp::dict result = (bp::dict) parse_fcn(svg_xml_string);
    extractPython(result);
  }
  catch (bp::error_already_set) {
    std::string msg;
    if (PyErr_Occurred()) {
        msg = handle_pyerror(); 
    }
    bp::handle_exception();
    PyErr_Clear();
    cout << "::: ERROR ::: "<< endl << msg << endl;
    cout << "\n\nMAYBE THE SVG FILE WAS EMPTY OR DOES NOT EXIST?" << endl;
    return 1;
  }
  return 0;
}

#ifdef STANDALONE



int main(int argc, char** argv) {
   // GlobalOptions config = GlobalOptions::getInstance();

  GlobalOptions::getInstance().init();
#ifdef STANDALONE
  int quit = GlobalOptions::getInstance().parseCommandLine(argc, argv);
  if(quit) return quit;

  GlobalOptions::getInstance().printOptions();
#endif
  ParsedSVG * ps = new ParsedSVG();
  quit = ps->parseSVGFile(GlobalOptions::getInstance().filename);
  if(quit) return quit;

  VectorElementTree * vet = new VectorElementTree();
  vet->createAndSortTree(ps);
  PreProcessor * ppp = new PreProcessor(vet);
  ppp->process(
    GlobalOptions::getInstance().translate_playfield_x,
    GlobalOptions::getInstance().translate_playfield_y,
    GlobalOptions::getInstance().field_width,
    GlobalOptions::getInstance().field_height
  );
  if(GlobalOptions::getInstance().segmentation_on) {
    SegmentationPreProcessor * spp = new SegmentationPreProcessor(vet);
    spp->process();
  }
  // vet->print_tree();
  vet->fillPolys();
  SimpleConnector * sc = new SimpleConnector(vet);
  sc->connect();

  vet->clearFill();
  
  // PostProcessor *  psc = new PostProcessor(vet);
  // psc->process();


  // psc->export_result();
  
  std::string json = "var PolyJSON = '" + vet->toJSON();
  json += "'";
  std::ofstream of("PolyJSON.js");
  of << json;
  vet->writeToFile("out_tree.txt");
#ifdef WITH_GUI
  if(GlobalOptions::getInstance().display) {
    if(GlobalOptions::getInstance().scale_for_disp != 1) {
      ppp->process(GlobalOptions::getInstance().scale_for_disp);
    }
    QApplication app(argc, argv);
 
    View * window = new View();
    window->initWindow();
    window->show();
    vet->addWindow(window);
    vet->drawTreeOnCanvas();
    vet->drawConnections();
    return app.exec();
  }
#endif
  // delete spp;
  delete vet;
  delete sc;
  delete ps;
  return 0;
}
#endif