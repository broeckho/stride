# Install script for directory: /Users/broeckho/usr/projects/c++/stride/main/resources

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/data" TYPE FILE FILES
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/belgium_commuting.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/belgium_commuting_major.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/belgium_population.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/belgium_population_major.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/contact_matrix_flanders_15touch.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/contact_matrix_flanders_subpop.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/disease_influenza.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/disease_influenza_15touch.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/disease_measles.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/disease_measles_15touch.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/holidays_flanders_2016.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/holidays_flanders_2017.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/holidays_miami_2017.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/holidays_none.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/households_flanders.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/immunity_measels.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_antwerp.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_flanders1600.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_flanders600.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_miami.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_miami_geo.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_nassau.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_nassau_geo.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_oklahoma.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/pop_oklahoma_geo.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_2011_BE_demo_pjan.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_2011_BE_hh_size.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_regular_weekday_community_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_regular_weekday_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_regular_weekday_household_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_regular_weekday_school_student_age24_gam_mij_median.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_regular_weekday_workplace_employed_age_gam_mij_median.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_weekend_community_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_weekend_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_15touch_weekend_household_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_regular_weekday_community_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_regular_weekday_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_regular_weekday_household_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_regular_weekday_school_student_age24_gam_mij_median.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_regular_weekday_workplace_employed_age_gam_mij_median.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_weekend_community_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_fl2010_weekend_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/ref_miami_household_gam_mij_rec.csv"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/data/susceptibility_measles_belgium_2013.csv"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/config" TYPE FILE FILES
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/run_default.xml"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_15touch_flanders_contact_week.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_15touch_flanders_contact_weekend.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_default.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_flanders_contact_week.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_flanders_contact_weekend.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_flanders_r0.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_flanders_transmissions.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_gtester_default.json"
    "/Users/broeckho/usr/projects/c++/stride/main/resources/config/wrapper_gtester_measles.json"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/broeckho/usr/projects/c++/stride/cmake-build-release/main/resources/lib/cmake_install.cmake")

endif()

