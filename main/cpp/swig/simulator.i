%module Simulator

// headers for generated file
%{
#include "util/Subject.h"
#include "sim/Simulator.h"
%}

namespace stride { namespace util {} }

%include "util/Subject.h"

%ignore stride::util::Subject<unsigned int, SimulatorObserver>;
%template(SubjectBaseClass) stride::util::Subject<unsigned int, SimulatorObserver>;

%include <std_shared_ptr.i>
%shared_ptr(stride::util::Subject<unsigned int, SimulatorObserver>)
%shared_ptr(stride::Simulator)

%include "sim/Simulator.h";
