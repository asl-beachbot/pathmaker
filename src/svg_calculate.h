#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class SVGCalculate {
private:
  ptree m_pt_image;
  ptree m_pt_path;
  void calculate_path_from_image();
  void print_pt(ptree const& image);
public:
  SVGCalculate(ptree & image);
  SVGCalculate();
  ~SVGCalculate() {}
};