#include <boost/python.hpp>

#include "SVGParserAdapter.h"

using namespace boost::python;

class Test {
public:
	Test(int a) {}
	int f(int i) {
		return i + 2;
	}
};
 char const* yay()
{
  return "Yay!";
}

BOOST_PYTHON_MODULE(beachbot_pathgen)
{
    // Create the Python type object for our extension class and define __init__ function.
    class_<ParsedSVG>("ParsedSVG", init<>())
        .def("parse_string", &ParsedSVG::parseSVGString)  // Add a regular member function.
        .def("parse_file", &ParsedSVG::parseSVGFile)  // Add invite() as a regular function to the module.
    ;
    // class_<Test>("Test", init<int>())
    //     .def("f", &Test::f)  // Add a regular member function.
    //     .def("parse_file", &Test::parseSVGFile)  // Add invite() as a regular function to the module.
    // ;
// def("yay", yay);


    // def("invite", invite); // Even better, invite() can also be made a member of module!!!
}