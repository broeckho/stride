%module(directors="1") SimulatorObserver

// headers for generated file
%{
#include "sim/SimulatorObserver.h"
%}

%include <std_shared_ptr.i>
%shared_ptr(stride::SimulatorObserver)

%feature("director") stride::SimulatorObserver;
%include "sim/SimulatorObserver.h";
