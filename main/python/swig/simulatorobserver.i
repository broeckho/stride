%module(directors="1") SimulatorObserver

// headers for generated file
%{
#include "sim/python/SimulatorObserver.h"
%}

%include <std_shared_ptr.i>
%shared_ptr(stride::pyhton::SimulatorObserver)

%feature("director") stride::python::SimulatorObserver;
%include "sim/python/SimulatorObserver.h";
