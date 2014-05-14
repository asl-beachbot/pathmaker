#include <boost/python.hpp>

#include "SVGParserAdapter.h"
#include "VectorElementTree.h"
#include "SimpleConnector.h"
#include "FillProcedures.h"

using namespace boost::python;

class Generator {
private:
    ParsedSVG * ps;
    VectorElementTree * vet;
    SimpleConnector * sc;
public:
    Generator() {
        ps = new ParsedSVG();
    }
    void parseSVGString(std::string str) {
        ps->parseSVGString(str);
    }
    void parseSVGFile(std::string fn) {
        ps->parseSVGFile(fn);
    }
    std::string run_routine() {
      vet = new VectorElementTree();
      vet->createAndSortTree(ps);
      vet->fillPolys();
      sc = new SimpleConnector(vet);
      sc->connect();
      return vet->toJSON();
    }
    ~Generator() {
        delete ps;
        delete vet;
        delete sc;
    }
};

BOOST_PYTHON_MODULE(beachbot_pathgen)
{
    // Create the Python type object for our extension class and define __init__ function.
    class_<Generator>("Generator", init<>())
        .def("parse_string", &Generator::parseSVGString)  // Add a regular member function.
        .def("parse_file", &Generator::parseSVGFile)  // Add invite() as a regular function to the module.
        .def("run_routine", &Generator::run_routine)  // Add invite() as a regular function to the module.
    ;
}