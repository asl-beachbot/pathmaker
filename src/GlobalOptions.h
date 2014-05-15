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
  po::options_description desc = po::options_description("Allowed options");
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
    ;
  }
  int parseConfigFile(std::string cfg_filename = "config.cfg") {
    std::ifstream ifs = std::ifstream(cfg_filename);
    po::store(po::parse_config_file(ifs), vm);
  }
  int parseCommandLine(int argc, char ** argv) {
    this->parseConfigFile();
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
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
    if(vm.count("config_file")) {
      this->parseConfigFile((std::string) vm["config_file"]);
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

  std::string filename = "assets/2.svg";
  bool display = false;
  double field_width = 5;
  double field_height = 5;
  double beachbot_size = 0.25;
  double line_distance = 0.25;
  double rounding_radius = 0.5;
  bool segmentation_on = 0;
  double scale_for_disp = 100;
  bool translate_playfield = false;
  double translate_playfield_x = 0;
  double translate_playfield_y = 0;
  double angle_interpolation_stepsize = 0.2;
  double area_deletion_threshold = 0;
  int number_of_bezier_segs = 30;
  double max_squared_point_distance = 0.05;
  double threshold_round_angle = 3.14 / (double)2;
  int fill_method = 2; // spiral: 2, wiggle: 1
private:
  
// options

  GlobalOptions() {};

   
  // Don't implement below
  GlobalOptions(GlobalOptions const&);
  void operator=(GlobalOptions const&);
  // End don't implement
};