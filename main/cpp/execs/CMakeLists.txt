#============================================================================
# Build the Stride binary.
#============================================================================
add_executable(stride stride.cpp)
target_link_libraries(stride ${LIBS} libstride libgengeopop)

install(TARGETS stride DESTINATION ${BIN_INSTALL_LOCATION})

#============================================================================
# Build the Calibration binary.
#============================================================================
# add_executable(calibration calibration.cpp)
#
# if(Qt5_FOUND)
#     target_link_libraries(calibration ${LIBS} ${QT_LIBRARIES} libstride libgengeopop libcalibration)
# else()
#     target_link_libraries(calibration ${LIBS} libstride libgengeopop libcalibration)
# endif()
#
# install(TARGETS calibration DESTINATION ${BIN_INSTALL_LOCATION})


#============================================================================
# Build the GenGeoPop binary.
#============================================================================
add_executable(gengeopop gengeopop.cpp)
target_link_libraries(gengeopop ${LIBS} libprotobuf_insrc libproto libstride libgengeopop)
install(TARGETS gengeopop DESTINATION ${BIN_INSTALL_LOCATION})
