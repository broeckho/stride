%module Sim

// headers for generated file
%{
#include "sim/Sim.h"
extern std::shared_ptr<stride::Sim> CreateSim(const std::string& config);
%}

%include <std_string.i>

%include <std_shared_ptr.i>
%shared_ptr(stride::Sim)

%include "sim/Sim.h";
%include "cpp/create_sim.h"
