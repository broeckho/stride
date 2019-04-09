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

CMAKE_BUILD_TYPE = "Release"

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
#LABEL=$(shell git rev-list HEAD --count)
#CMAKE_INSTALL_PREFIX  = $(HOME)/opt/stride-$(LABEL)
CMAKE_INSTALL_PREFIX  = $(HOME)/opt/stride

#============================================================================
# 	CMake command
#============================================================================
ifeq ($(CMAKE),)
	CMAKE = cmake
endif

#============================================================================
#   MACRO definitions to pass on to cmake
#============================================================================
CMAKE_ARGS += -DCMAKE_GENERATOR="Unix Makefiles"
ifneq ($(CMAKE_C_COMPILER),)
	CMAKE_ARGS += -DCMAKE_C_COMPILER:FILEPATH=$(CMAKE_C_COMPILER)
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
ifneq ($(STRIDE_FORCE_NO_OPENMP),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_OPENMP:BOOL=$(STRIDE_FORCE_NO_OPENMP)
endif
ifneq ($(STRIDE_FORCE_NO_PYHTON),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_PYTHON:BOOL=$(STRIDE_FORCE_NO_PYTHON)
endif
ifneq ($(STRIDE_FORCE_NO_BOOST),)
	CMAKE_ARGS += -DSTRIDE_FORCE_NO_BOOST:BOOL=$(STRIDE_FORCE_NO_BOOST)
endif
ifneq ($(BOOST_NO_SYSTEM_PATHS),)
	CMAKE_ARGS += -DBOOST_NO_SYSTEM_PATHS:BOOL=$(BOOST_NO_SYSTEM_PATHS)
endif
#============================================================================
#   Build directory.
#============================================================================
ifeq ($(BUILD_DIR),)
ifeq ($(CMAKE_BUILD_TYPE),Debug)
	BUILD_DIR = ./cmake-build-debug
else
	BUILD_DIR = ./cmake-build-release
endif
endif

#============================================================================
#   Targets
#============================================================================
.PHONY: help cores configure all install clean distclean test gtest format

help:
	@ $(CMAKE) -E echo " Read INSTALL.txt in this directory for a brief overview."
	@ $(CMAKE) -E echo " Current macro values are:"
	@ $(CMAKE) -E echo "   BUILD_DIR                     : " $(BUILD_DIR)
	@ $(CMAKE) -E echo " "
	@ $(CMAKE) -E echo "   CMAKE_GENERATOR               : " $(CMAKE_GENERATOR)
	@ $(CMAKE) -E echo "   CMAKE_BUILD_TYPE              : " $(CMAKE_BUILD_TYPE)
	@ $(CMAKE) -E echo "   CMAKE_INSTALL_PREFIX          : " $(CMAKE_INSTALL_PREFIX)
	@ $(CMAKE) -E echo " "
	@ $(CMAKE) -E echo "   STRIDE_INCLUDE_DOC            : " $(STRIDE_INCLUDE_DOC)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_OPENMP        : " $(STRIDE_FORCE_NO_OPENMP)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_PYTHON        : " $(STRIDE_FORCE_NO_PYTHON)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_HDF5          : " $(STRIDE_FORCE_NO_HDF5)
	@ $(CMAKE) -E echo "   STRIDE_FORCE_NO_BOOST         : " $(STRIDE_FORCE_NO_BOOST)
	@ $(CMAKE) -E echo "   BOOST_NO_SYSTEM_PATHS         : " $(BOOST_NO_SYSTEM_PATHS)
	@ $(CMAKE) -E echo " "

cores:
	@ echo "\nMake invocation using -j"$(NCORES)

configure:
	$(CMAKE) -E make_directory $(BUILD_DIR)
	$(CMAKE) -E chdir $(BUILD_DIR) $(CMAKE) $(CMAKE_ARGS) ..

all: cores configure
	$(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) --no-print-directory all

install:
	$(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) --no-print-directory install

clean: cores
	 if [ -d $(BUILD_DIR) ]; then $(MAKE) $(PARALLEL_MAKE) -C $(BUILD_DIR) clean; fi

distclean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

test: install
	cd $(BUILD_DIR)/test; ctest $(TESTARGS) -V

gtest: install
	cd $(CMAKE_INSTALL_PREFIX); bin/gtester $(TESTARGS)

format:
	resources/bash/clang-format-all .
	resources/bash/remove_trailing_space

#############################################################################
