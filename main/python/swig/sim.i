%module Sim

// headers for generated file
%{
#include "sim/Sim.h"
%}

%include <std_string.i>

%include <std_shared_ptr.i>
%shared_ptr(stride::Sim)

%include "sim/Sim.h";
