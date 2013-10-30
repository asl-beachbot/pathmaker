/*     Pathfinder
 *
 *     Version: @Pathfinder_VERSION_MAJOR@.@Pathfinder_VERSION_MINOR@
 */

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using boost::property_tree::ptree;
void pretty_print_property_tree(ptree const& pt) {
    
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        cout << it->first << ": " << it->second.get_value<std::string>() << endl;
        pretty_print_property_tree(it->second);
    }
}

int main(int argc, char **argv) {
    cout << "Welcome to the Pathfinder. Finding a path through the dark since 1999." << endl << "  (c) BeachBot Productions LLC. ";
    ptree pt;

    read_xml("/home/wolf/projects/pathmaker/files/bubbles.svg", pt);
    pretty_print_property_tree(pt);
    string test = pt.get<string>("svg");
    cout << test; 
}
