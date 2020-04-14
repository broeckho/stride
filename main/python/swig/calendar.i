%module Calendar

// headers for generated file
%{
#include "calendar/Calendar.h"
%}

%include <std_shared_ptr.i>

%shared_ptr(stride::Calendar);
%include "calendar/Calendar.h"
