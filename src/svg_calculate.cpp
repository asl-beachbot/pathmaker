#include "svg_calculate.h"

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;

SVGCalculate::SVGCalculate(ptree& image) {
  m_pt_image = image;
  this->calculate_path_from_image();

}

SVGCalculate::SVGCalculate() {}

void SVGCalculate::calculate_path_from_image() {
  this->print_pt(this->m_pt_image);
}

void SVGCalculate::print_pt(ptree const& pt) {
  ptree::const_iterator end = pt.end();
  for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
    cout << it->first << ": " << it->second.get_value<std::string>() << endl;
    print_pt(it->second);
  }
}