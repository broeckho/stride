%module Sim

// headers for generated file
%{
#include "sim/python/Subject.h"
#include "sim/Sim.h"

%}

namespace stride { namespace python {} }

%include "sim/python/Subject.h"

%ignore stride::python::Subject<unsigned int, SimulatorObserver>;
%template(SubjectBaseClass) stride::python::Subject<unsigned int, SimulatorObserver>;

%include <std_shared_ptr.i>
%shared_ptr(stride::python::Subject<unsigned int, SimulatorObserver>)
%shared_ptr(stride::Sim)

%include "sim/Sim.h";
