%module Simulator

// headers for generated file
%{
#include "sim/Simulator.h"
%}

%include <std_shared_ptr.i>
%shared_ptr(stride::Simulator)

%include "sim/Simulator.h";
