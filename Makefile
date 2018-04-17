#############################################################################
#  This file is part of the Stride software.
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or any
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
#	Meta makefile calls cmake to do the heavy lifting.
#
#############################################################################

#============================================================================
#   Configuring Make invocations.
#============================================================================
NCORES=`getconf _NPROCESSORS_ONLN`
ifeq ($(PARALLEL_MAKE),)
	PARALLEL_MAKE = -j$(NCORES)
endif

#============================================================================
#   Test related: had to duplicate CMAKE_INSTALL_PREFIX here for gtester
#============================================================================
LABEL=$(shell git rev-list HEAD --count)
CMAKE_INSTALL_PREFIX  = $(HOME)/opt/stride-$(LABEL)

#============================================================================
# 	CMake command
#============================================================================
ifeq ($(CMAKE),)
	CMAKE = cmake
endif
ifeq ($(CMAKE_GENERATOR),)
	CMAKE_GENERATOR = "Unix Makefiles"
endif

#============================================================================
#   MACRO definitions to pass on to cmake
#============================================================================
ifeq ($(BUILD_DIR),)
	BUILD_DIR = ./cmake-build-release
endif

ifneq ($(CMAKE_GENERATOR),)
	CMAKE_ARGS += -DCMAKE_GENERATOR=$(CMAKE_GENERATOR)
endif
ifneq ($(CMAKE_C_COMPILER),)
	CMAKE_ARGS += -DCMAKE_C_COMPILER:FILEPATH=$(CMAKE_C_COMPILER)
endif
ifneq ($(CMAKE_CXX_COMPILER),)
	CMAKE_ARGS += -DCMAKE_CXX_COMPILER:FILEPATH=$(CMAKE_CXX_COMPILER)
endif
ifneq ($(CMAKE_CXX_FLAGS),)
	CMAKE_ARGS += -DCMAKE_CXX_FLAGS:STRING=$(CMAKE_CXX_FLAGS)
endif
ifneq ($(CMAKE_BUILD_TYPE),)
	CMAKE_ARGS += -DCMAKE_BUILD_TYPE:STRING=$(CMAKE_BUILD_TYPE)
endif
ifneq ($(CMAKE_INSTALL_PREFIX),)
	CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX:PATH=$(CMAKE_INSTALL_PREFIX)
endif
ifneq ($(STRIDE_INCLUDE_DOC),)
	CMAKE_ARGS += -DSTRIDE_INCLUDE_DOC:BOOL=$(STRIDE_INCLUDE_DOC)
endif
ifneq ($(STRIDE_VERBOSE_TESTING),)
	CMAKE_ARGS += -DSTRIDE_VERBOSE_TESTING:BOOL=$(STRIDE_VERBOSE_TESTING)
endif
ifneq ($(STRIDE_BOOST_ROOT),)
	CMAKE_ARGS += -DSTRIDE_BOOST_ROOT:STRING=$(STRIDE_BOOST_ROOT)
endif
ifneq ($(STRIDE_BOOST_NO_SYSTEM_PATHS),)
	CMAKE_ARGS += -DSTRIDE_BOOST_NO_SYSTEM_PATHS:STRING=$(STRIDE_BOOST_NO_SYSTEM_PATHS)
endif
ifneq ($(STRIDE_FORCE_NO_OPENMP),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_OPENMP:BOOL=$(STRIDE_FORCE_NO_OPENMP)
endif
ifneq ($(STRIDE_FORCE_NO_PYHTON),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_PYTHON:BOOL=$(STRIDE_FORCE_NO_PYTHON)
endif
ifneq ($(STRIDE_FORCE_NO_HDF5),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_HDF5:BOOL=$(STRIDE_FORCE_NO_HDF5)
endif

#============================================================================
#   Targets
#============================================================================
.PHONY: help configure bootstrap all build_all build_main build_test
.PHONY: install install_all install_main install_test package
.PHONY: test installcheck distclean remove_build

help:

	@ $(CMAKE) -E echo " Read INSTALL.txt in this directory for a brief overview."
	@ $(CMAKE) -E echo " Current macro values are:"

	@ $(CMAKE) -E echo "   BUILD_DIR                     : " $(BUILD_DIR)
	@ $(CMAKE) -E echo " "
	@ $(CMAKE) -E echo "   CMAKE_GENERATOR               : " $(CMAKE_GENERATOR)
	@ $(CMAKE) -E echo "   CMAKE_CXX_COMPILER            : " $(CMAKE_CXX_COMPILER)
	@ $(CMAKE) -E echo "   CMAKE_CXX_FLAGS               : " $(CMAKE_CXX_FLAGS)
	@ $(CMAKE) -E echo "   CMAKE_BUILD_TYPE              : " $(CMAKE_BUILD_TYPE)
	@ $(CMAKE) -E echo "   CMAKE_INSTALL_PREFIX          : " $(CMAKE_INSTALL_PREFIX)
	@ $(CMAKE) -E echo " "
	@ $(CMAKE) -E echo "   STRIDE_INCLUDE_DOC            : " $(STRIDE_INCLUDE_DOC)
	@ $(CMAKE) -E echo "   STRIDE_VERBOSE_TESTING        : " $(STRIDE_VERBOSE_TESTING)
	@ $(CMAKE) -E echo "   STRIDE_BOOST_ROOT             : " $(STRIDE_BOOST_ROOT)
	@ $(CMAKE) -E echo "   STRIDE_BOOST_NO_SYSTEM_PATHS  : " $(STRIDE_BOOST_NO_SYSTEM_PATHS)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_OPENMP        : " $(STRIDE_FORCE_NO_OPENMP)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_HDF5          : " $(STRIDE_FORCE_NO_HDF5)

	@ $(CMAKE) -E echo " "

cores:
	@ echo "\nMake invocation using -j"$(NCORES) "\n"

configure: cores
	$(CMAKE) -E make_directory $(BUILD_DIR)
	$(CMAKE) -E chdir $(BUILD_DIR) $(CMAKE) $(CMAKE_ARGS) ..

all: configure
	$(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) --no-print-directory all

install: cores
	$(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) --no-print-directory install

clean: cores
	$(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) clean

distclean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

test: install
	cd $(BUILD_DIR)/test; ctest $(TESTARGS) -V

gtest: install
	cd $(CMAKE_INSTALL_PREFIX); bin/gtester $(GTESTARGS)

format:
	resources/bash/clang-format-all .
	resources/bash/remove_trailing_space

#############################################################################
REGERX=influen