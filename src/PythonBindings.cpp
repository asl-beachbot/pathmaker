#include <boost/python.hpp>
#include <boost/python.hpp>

#include "SVGParserAdapter.h"
#include "VectorElementTree.h"
#include "SimpleConnector.h"
#include "FillProcedures.h"
#include "GlobalOptions.h"
#include "PreProcessor.h"
#include "SegmentationPreProcessor.h"

using namespace boost::python;

class Generator {
private:
    ParsedSVG * ps;
    VectorElementTree * vet;
    SimpleConnector * sc;
public:
    Generator() {
        GlobalOptions::getInstance().init();
        GlobalOptions::getInstance().parseConfigFile("pythoncfg.cfg");
        GlobalOptions::getInstance().printOptions();
        vet = nullptr;
        sc = nullptr;
    }
    void parseSVGString(std::string str) {
        ps = new ParsedSVG();
        ps->parseSVGString(str);
    }
    void parseSVGFile(std::string fn) {
        ps = new ParsedSVG();
        ps->parseSVGFile(fn);
    }
    std::string run_routine() {
      if(vet) {
        cout << "deleting the old VET" << endl;
        delete vet;
      }
      vet = new VectorElementTree();
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
      vet->fillPolys();
      // sc = new SimpleConnector(vet);
      // sc->connect();
      return vet->toJSON();
    }
    std::string update_fill_for_element(std::string arguments_as_json)  {
        Json::Reader jsonreader;
        Json::Value arguments;
        // all arguments stored in arguments
        cout << "Updating Fill (CPP)" << vet << endl;
        bool success = jsonreader.parse(arguments_as_json, arguments);
        if(!success) {
            std::cout << "Failed to parse arguments, bailing" << endl;
            return "{\"error\":\"Error Parsing JSON\"}";
        }
        int id = arguments.get("id", 0).asInt();
        if(id == 0) { return "{\"error\":\"ID zero\"}"; }
        ElementPtr * el = vet->getByID(id);
        cout << "Pointer to found Element: " << el << endl;
        if(el == NULL) { return "{\"error\":\"Element with ID does not exist\"}"; }
        double dx = arguments.get("dx", 0).asDouble();
        double dy = arguments.get("dy", 0).asDouble();
        auto casted_el = static_cast<FilledPolygonElementPtr *>(el);
        int segment_index = arguments.get("segment_index", 0).asInt();
        auto seg = &(casted_el->segments.at(segment_index));
        int fill_method = arguments.get("fill_method", 1).asInt();
        seg->fill_method = fill_method;
        seg->direction = Direction_2(dx, dy);
        // seg->direction = Direction_2(arguments["direction"][0].asInt())
        // int fill_method; // fill type: 1 = Skeleton, 2 = wiggle
        // Direction_2 direction; // only for wiggle fill
        vet->print_tree();
        vet->clearFill();
        vet->print_tree();
        vet->fillPolys();
        vet->print_tree();
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
        .def("parse_string", &Generator::parseSVGString)
        .def("parse_file", &Generator::parseSVGFile)
        .def("run_routine", &Generator::run_routine)
        .def("update_fill_for_element", &Generator::update_fill_for_element)
    ;
}