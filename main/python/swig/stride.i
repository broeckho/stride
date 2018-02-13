%module stride

// for handling errors.
%include <exception.i>

%{
#include <iostream>
%}

%exception {
  try {
    $action
  } catch (const std::exception &exc) {
    std::cerr << "Error: " << exc.what() << std::endl;
    SWIG_exception(SWIG_RuntimeError,"Exception in C++ code.");
  } catch (...) {
    SWIG_exception(SWIG_RuntimeError,"Exception in C++ code.");
  }
}

%include "./contactpooltype.i";
%include "./health.i";
%include "./population.i";
%include "./simulatorobserver.i";
%include "./simulator.i";
%include "./striderunner.i";
