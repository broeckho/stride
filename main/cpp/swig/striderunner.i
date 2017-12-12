%module StrideRunner

// headers for generated file
%{
  #include "sim/StrideRunner.h"

  using namespace stride;
%}

// So we can pass python strings to arguments that require std::string.
%include std_string.i

%include "sim/StrideRunner.h"
