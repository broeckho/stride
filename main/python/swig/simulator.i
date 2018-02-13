%module Simulator

// headers for generated file
%{
#include "sim/python/Subject.h"
#include "sim/Simulator.h"
%}

namespace stride { namespace python {} }

%include "sim/python/Subject.h"

%ignore stride::python::Subject<unsigned int, SimulatorObserver>;
%template(SubjectBaseClass) stride::python::Subject<unsigned int, SimulatorObserver>;

%include <std_shared_ptr.i>
%shared_ptr(stride::python::Subject<unsigned int, SimulatorObserver>)
%shared_ptr(stride::Simulator)

%include "sim/Simulator.h";
