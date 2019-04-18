%module Population

// SegmentedVector.h:98: Warning 362: operator= ignored -- (JB) but we do not need it, so OK suppress warning
// SegmentedVector.h:109: Warning 362: operator= ignored -- (JB)but we do not need it, so OK, suppress warning
#pragma SWIG nowarn=362
//
// SegmentedVector.h:150: Warning 389: operator[] ignored (consider using %extend) -- (JB) extend below or just use at method
// SegmentedVector.h:156: Warning 389: operator[] ignored (consider using %extend) -- (JB) extend below or just use at method
#pragma SWIG nowarn=389
//
// SegmentedVector.h:83: Warning 509:  -- (JB) move constructor ignored because shadowed by copy constructor but that's OK
// SegmentedVector.h:253: Warning 509: -- (JB) move push_back ignored because it is shadowed by copy push_back but that's OK
#pragma SWIG nowarn=509

%{
#include "pop/Person.h"
#include "pop/Population.h"
#include <vector>
#include <string>
#include <memory>
#include "util/SegmentedVector.h"
    
using namespace stride;
using namespace stride::util;
%}

%include <std_vector.i>
%include <std_string.i>
%include <std_shared_ptr.i>

%include "pop/Person.h"
%include "util/SegmentedVector.h"
%extend stride::util::SegmentedVector<stride::Person, 2048> {
    stride::Person& __getitem__(std::size_t pos) {
        return (*($self))[pos];
    }
}
%shared_ptr(stride::util::SegmentedVector<stride::Person, 2048>);
%shared_ptr(stride::Population);
%template(PopulationBaseClass) stride::util::SegmentedVector<stride::Person, 2048>;
%include "pop/Population.h"
