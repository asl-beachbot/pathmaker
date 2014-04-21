// Python Interface
// https://wiki.python.org/moin/boost.python/EmbeddingPython
#pragma once

#include <iostream>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <string>

using namespace std;
namespace bp = boost::python;

std::string handle_pyerror()
{
    PyObject *exc,*val,*tb;
    bp::object formatted_list, formatted;
    PyErr_Fetch(&exc,&val,&tb);
    bp::handle<> hexc(exc),hval(bp::allow_null(val)),htb(bp::allow_null(tb));
    bp::object traceback(bp::import("traceback"));
    if (!tb) {
        bp::object format_exception_only(traceback.attr("format_exception_only"));
        formatted_list = format_exception_only(hexc,hval);
    } else {
        bp::object format_exception(traceback.attr("format_exception"));
        formatted_list = format_exception(hexc,hval,htb);
    }
    formatted = bp::str("\n").join(formatted_list);
    return bp::extract<std::string>(formatted);
}

int initializePython() {
  cout << "Init Python" << endl;
  Py_Initialize();
  boost::filesystem::path workingDir = boost::filesystem::absolute("./python/").normalize();
  PyObject* sysPath = PySys_GetObject("path");
  PyList_Insert( sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
}

std::string get_dat_from_svg(std::string filename) {
	bool py_exception = false;
	try {
		boost::filesystem::path workingDir = boost::filesystem::absolute("./python/").normalize();
		PyObject* sysPath = PySys_GetObject("path");
		std::cout << workingDir << std::endl;
		PyList_Insert( sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
	  	bp::object sys = bp::import("parse_dat");
	  	bp::object pyfcn = sys.attr("call_from_cpp");
	  	// Extract the python version.
	  	std::string dat = bp::call<std::string>(pyfcn.ptr(), filename);
	  	cout << "::: Imported DAT ::: "<< endl << dat << endl;
	  	return dat;
	}
	catch (bp::error_already_set) {
		std::string msg;
    if (PyErr_Occurred()) {
        msg = handle_pyerror(); 
    }
    py_exception = true;
    bp::handle_exception();
    PyErr_Clear();
    cout << "::: ERROR ::: "<< endl << msg << endl;
    return "ERROR";
	}
}

// int main(void) {
// 	Py_Initialize();
// 	bool py_exception = false;
// 	try {
// 		boost::filesystem::path workingDir = boost::filesystem::absolute("./python/").normalize();
// 		PyObject* sysPath = PySys_GetObject("path");
// 		std::cout << workingDir << std::endl;
// 		PyList_Insert( sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
// 	  	object sys = import("parse_dat");
// 	  	bp::object pyfcn = sys.attr("call_from_cpp");
// 	  	// Extract the python version.
// 	  	std::string version = call<std::string>(pyfcn.ptr(), "assets/test3.svg");
// 	  	std::cout << version << std::endl;
// 	}
// 	catch (bp::error_already_set) {
// 		std::string msg;
// 	        if (PyErr_Occurred()) {
// 	            msg = handle_pyerror(); 
// 	        }
// 	        py_exception = true;
// 	        bp::handle_exception();
// 	        PyErr_Clear();
// 	        cout << msg << endl;
// 	}

// 	// object importer = import("parse_dat");
// 	// std::string ret = extract<std::string>(importer.attr("test"));
// 	// //boost::python::object test = importer.attr("test")();
// 	// // char * test = boost::python::call<char * > (importer);
// 	// std::cout << "Test: " << ret << std::endl;
// }

