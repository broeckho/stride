# Install script for directory: /Users/broeckho/usr/projects/c++/stride/main/python/examples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/broeckho/opt/stride-2")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/data" TYPE FILE FILES
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/contact_matrix_flanders_subpop.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/disease_influenza.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/disease_measles.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/holidays_flanders_2017.json"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/holidays_none.json"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/pop_antwerp.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/pop_flanders1600.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/data/pop_flanders600.csv"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/config" TYPE FILE FILES "/Users/broeckho/usr/projects/c++/stride/main/python/examples/config/run_default.xml")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ FILES
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runCallbackInterval.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runCallbackTimestep.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runExperiments.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runManualConfig.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runRepeat.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runSimple.py"
    "/Users/broeckho/usr/projects/c++/stride/main/python/examples/runStopOnCallback.py"
    )
endif()

