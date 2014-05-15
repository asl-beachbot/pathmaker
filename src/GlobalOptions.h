//GlobalOptions.h
#pragma once

#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

#ifdef STANDALONE
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#endif

class GlobalOptions
{
public:
  static GlobalOptions& getInstance()
  {
    static GlobalOptions instance;
    return instance;
  }
#ifdef STANDALONE
  po::options_description desc;
  po::variables_map vm;

  void init() {
    desc.add_options()
        ("help,h", "produce help message")
        ("filename,f", po::value<std::string>(), "SVG File for parsing")
        ("round_radius,rr", po::value<double>(), "set radius for corner rounding")
        ("fill_method,m", po::value<int>(), "set fill method (1: wiggle or 2: spiral)")
        ("scale_for_disp,scale_fd", po::value<double>(), "scale for display")
        ("angle_step", po::value<double>(), "Interpolation stepsize for rounding (e.g. 0.2 * PI)")
        ("max_squared_point_distance,ms", po::value<double>(), "Max distance for points (handmade for Timon)")
        ("display,d", "Open up the QT Window for inspection and modification")
        ("threshold_round_angle,t", po::value<double>(), "Defines from which angle on it should be rounded (or outer rounded)")
        ("line_distance,ld", po::value<double>(), "Line distance inside filled elements")
        ("area_deletion_threshold", po::value<double>(), "Maximum area of filling elements that will get deleted ")
        ("config_file,c", po::value<std::string>(), "Use a different config file")
        ("segmentation_on", po::value<bool>(), "Turn on or off segmentation")
        ("text_export_filename", po::value<std::string>(), "Filename for export to textfile")
        ("svg_export_filename", po::value<std::string>(), "Filename for export to SVG File")
    ;
  }

  int parseConfigFile(std::string cfg_filename = "config.cfg") {
    std::ifstream ifs(cfg_filename);
    po::store(po::parse_config_file(ifs, desc), vm);
  }

  int parseCommandLine(int argc, char ** argv) {
    this->parseConfigFile();
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    if(vm.count("config_file")) {
      this->parseConfigFile((std::string) vm["config_file"].as<std::string>());
    }
    if (vm.count("display")) {
      this->display = true;
    }
    if (vm.count("filename")) {
      this->filename = (std::string) vm["filename"].as<std::string>();
    }
    if (vm.count("round_radius")) {
      this->rounding_radius = (double) vm["round_radius"].as<double>();
    }
    if(vm.count("fill_method")) {
      this->fill_method = (int) vm["fill_method"].as<int>();
    }
    if(vm.count("field_width")) {
      this->field_width = (double) vm["field_width"].as<double>();
    }
    if(vm.count("field_height")) {
      this->field_height = (double) vm["field_height"].as<double>();
    }
    if(vm.count("beachbot_size")) {
      this->beachbot_size = (double) vm["beachbot_size"].as<double>();
    }
    if(vm.count("scale_for_disp")) {
      this->scale_for_disp = (double) vm["scale_for_disp"].as<double>();
    }
    if(vm.count("line_distance")) {
      this->line_distance = (double) vm["line_distance"].as<double>();
    }
    if(vm.count("angle_step")) {
      this->angle_interpolation_stepsize = (double) vm["angle_step"].as<double>();
    }
    if(vm.count("max_squared_point_distance")) {
      this->max_squared_point_distance = (double) vm["max_squared_point_distance"].as<double>();
    }
    if(vm.count("threshold_round_angle")) {
      this->threshold_round_angle = (double) vm["threshold_round_angle"].as<double>();
    }
    if(vm.count("area_deletion_threshold")) {
      this->area_deletion_threshold = (double) vm["area_deletion_threshold"].as<double>();
    }
    if(vm.count("segmentation_on")) {
      this->segmentation_on = (bool) vm["segmentation_on"].as<bool>();
    }
    if(vm.count("text_export_filename")) {
      this->TXT_export_filename = (std::string) vm["text_export_filename"].as<std::string>();
    }
    if(vm.count("svg_export_filename")) {
      this->SVG_export_filename = (std::string) vm["svg_export_filename"].as<std::string>();
    }
    return 0;
  }
#endif
  void printOptions() {
    cout << 
"                     `,:                                           \n" <<
"                  ::::::      :,                                   \n" <<
"                  ::::::      ::::`                                \n" <<
"                  `:::::      ::::::                               \n" <<
"           ::      :::::     `::::::                               \n" <<
"         .:::      :::::     ::::::                                \n" <<
"        ::::::     :::::     ::::::                                \n" <<
"       :::::::.     ::::     :::::       `                         \n" <<
"        :::::::     ::::    .::::,      ::                         \n" <<
"         ::::::,    ::::    :::::      ::::                        \n" <<
"          ::::::    ,:::    ::::`     ::::::                       \n" <<
"           ::::::    :::    ::::     ::::::::                      \n" <<
"            :::::    :::   ,:::     :::::::::                      \n" <<
"   ::        :::::   :::   ::::    ::::::::`                       \n" <<
"  .::::       ::::.  .::   :::    :::::::,                         \n" <<
"  ::::::`      ::::   `    ::.   :::::::                 +@@@`     \n" <<
"  ::::::::      :::,            ::::::                  @@ @@@     \n" <<
" :::::::::::     :             :::::.                  #@: #@@     \n" <<
" ::::::::::::,          #@#    ::::                    +@@ #@@     \n" <<
"    ,::::::::::       @@@@@@    :                        . @@@     \n" <<
"        ,:::::::     @+  '@@         #@`   +@@      :@@@   @@:     \n" <<
"           `::::    @`    @@       @@@@@  #@@@#    @@#'@@  @@` @@@ \n" <<
"                   @+  + `@@.     #@@ @@  @@@   ; ,@@ :@@ ,@@@@@@@@\n" <<
"                  @@ `@  @@@@@@   @@.#@@ @@@  #@@ @@@ @@# @@@   @@@\n" <<
"                 `@# @@     @@@@ #@@ @@  @@@  @@. @@, '`  @@#  ,@@@\n" <<
":::::::::::::::  @@ @@#      @@@ @@@@:  .@@+ ,@@ '@@     @@@   @@@;\n" <<
",::::::::::::::  `  @@.      @@@ @@'    @@@. @@@ #@@   `@'@@   @@@ \n" <<
"`:::::::::::::,    `@@      `@@@@@@   .@:@@:'@@.`@@@. @@ @@@  '@@' \n" <<
" ::::::::::`       #@@      @@@ .@@  @@  @@@:@@@# @@@@,  @@   @@@  \n" <<
" ::::::.        .  @@@    #@@@   @@@@        `:    ,    ,@    @@'  \n" <<
" ::,          `::  @@; ;@@@#                                  @@   \n" <<
"             :::::`@@                              +@#         @@  \n" <<
"           :::::: +@@          @@                @@`@@@         +@ \n" <<
"         .::::::  @@@:        @@@@@@            @@  +@@            \n" <<
"        :::::::   @@::      .@@@`@@@,           @@  @@@            \n" <<
"      ::::::::   #@;::     @@@@#  @@+           `@+ @@#            \n" <<
"    ,::::::::   `@:::.    @'#@@` `@@                @@ @           \n" <<
"    ::::::::   .@::::    @@ @@@  @@@#     +` +@@   @@@@;           \n" <<
"     ::::::      ::::   @@  @@# `@@@@@#  @@@@#@@   @@@    .@       \n" <<
"     .::::      :::::   @@ ;@@      @@@ ;@@#@#@@`'@@@    @@@       \n" <<
"      :::      .::::.  ,   @@@      @@@ @@@   @@: @@@   `@@,       \n" <<
"       :       :::::       @@#      @@@.@@#  '@`  @@'   @@@        \n" <<
"              ,:::::      ,@@      @@@ #@@   @@  :@@    @@`        \n" <<
"              ::::::      @@@     #@@' @@@  @@   #@@   @@.         \n" <<
"             ::::::`   @  @@    .@@@:  +@@ @@    ;@@@@@@           \n" <<
"             ::::::   @@ #@@  @@@@#     #@@+       ..              \n" <<
"              :::::   @; @@                   .@@@                 \n" <<
"                `::   @;@@                 ;@@@@@@;                \n" <<
"                      :@;           @   +@@@@@@@@@                 \n" <<
"                                    @@@@@@@@@;   @                 \n\n\n\n" <<
      "#############################################\n\n" <<
      "#  Options for PathFinder\n\n" << 
      "#  Filename: " << filename << endl <<
      "#  Field Size: " << field_width << " x " << field_height << endl <<
      "#  Fill Method: " << fill_method << endl <<
      "#  Lind Distance: " << line_distance << endl <<
      "#  Rounding Radius: " << rounding_radius << endl <<
      "#  BeachBot Size: " << rounding_radius << endl <<
      "#  Rounding Radius: " << rounding_radius << endl <<
      "#  Segmentation: " << segmentation_on << endl <<
      "#  Translate Playfield: " << translate_playfield << endl <<
      "#############################################" << 
      endl;

  }

  std::string filename;
  bool display;
  double field_width;
  double field_height;
  double beachbot_size;
  double line_distance;
  double rounding_radius;
  bool segmentation_on;
  double scale_for_disp;
  bool translate_playfield;
  double translate_playfield_x;
  double translate_playfield_y;
  double angle_interpolation_stepsize;
  double area_deletion_threshold;
  int number_of_bezier_segs;
  double max_squared_point_distance;
  double threshold_round_angle;
  int fill_method;// spiral: 2, wiggle: 1
  std::string TXT_export_filename;
  std::string SVG_export_filename;


private:
  
// options

  GlobalOptions() : 
    filename("assets/test.svg"),
    desc(po::options_description("Allowed options")),
    display(false),
    field_width(5),
    field_height(5),
    beachbot_size(0.25),
    line_distance(0.25),
    rounding_radius(0.5),
    segmentation_on(0),
    scale_for_disp(100),
    translate_playfield(false),
    translate_playfield_x(0),
    translate_playfield_y(0),
    angle_interpolation_stepsize(0.2),
    area_deletion_threshold(0),
    number_of_bezier_segs(30),
    max_squared_point_distance(0.05),
    threshold_round_angle(3.14 / (double)2),
    fill_method(2) // spiral: 2, wiggle: 1
  {
  };

   
  // Don't implement below
  GlobalOptions(GlobalOptions const&);
  void operator=(GlobalOptions const&);
  // End don't implement
};