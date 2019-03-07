set(libprotobuf_files
    google/protobuf/any.cc
    google/protobuf/any.pb.cc
    google/protobuf/api.pb.cc
    google/protobuf/compiler/importer.cc
    google/protobuf/compiler/parser.cc
    google/protobuf/descriptor.cc
    google/protobuf/descriptor.pb.cc
    google/protobuf/descriptor_database.cc
    google/protobuf/duration.pb.cc
    google/protobuf/dynamic_message.cc
    google/protobuf/empty.pb.cc
    google/protobuf/extension_set_heavy.cc
    google/protobuf/field_mask.pb.cc
    google/protobuf/generated_message_reflection.cc
    google/protobuf/generated_message_table_driven.cc
    google/protobuf/io/gzip_stream.cc
    google/protobuf/io/printer.cc
    google/protobuf/io/tokenizer.cc
    google/protobuf/io/zero_copy_stream_impl.cc
    google/protobuf/map_field.cc
    google/protobuf/message.cc
    google/protobuf/reflection_ops.cc
    google/protobuf/service.cc
    google/protobuf/source_context.pb.cc
    google/protobuf/struct.pb.cc
    google/protobuf/stubs/mathlimits.cc
    google/protobuf/stubs/substitute.cc
    google/protobuf/text_format.cc
    google/protobuf/timestamp.pb.cc
    google/protobuf/type.pb.cc
    google/protobuf/unknown_field_set.cc
    google/protobuf/util/delimited_message_util.cc
    google/protobuf/util/field_comparator.cc
    google/protobuf/util/field_mask_util.cc
    google/protobuf/util/internal/datapiece.cc
    google/protobuf/util/internal/default_value_objectwriter.cc
    google/protobuf/util/internal/error_listener.cc
    google/protobuf/util/internal/field_mask_utility.cc
    google/protobuf/util/internal/json_escaping.cc
    google/protobuf/util/internal/json_objectwriter.cc
    google/protobuf/util/internal/json_stream_parser.cc
    google/protobuf/util/internal/object_writer.cc
    google/protobuf/util/internal/proto_writer.cc
    google/protobuf/util/internal/protostream_objectsource.cc
    google/protobuf/util/internal/protostream_objectwriter.cc
    google/protobuf/util/internal/type_info.cc
    google/protobuf/util/internal/type_info_test_helper.cc
    google/protobuf/util/internal/utility.cc
    google/protobuf/util/json_util.cc
    google/protobuf/util/message_differencer.cc
    google/protobuf/util/time_util.cc
    google/protobuf/util/type_resolver_util.cc
    google/protobuf/wire_format.cc
    google/protobuf/wrappers.pb.cc
)

set(libprotobuf_includes
    google/protobuf/any.h
    google/protobuf/any.pb.h
    google/protobuf/api.pb.h
    google/protobuf/compiler/importer.h
    google/protobuf/compiler/parser.h
    google/protobuf/descriptor.h
    google/protobuf/descriptor.pb.h
    google/protobuf/descriptor_database.h
    google/protobuf/duration.pb.h
    google/protobuf/dynamic_message.h
    google/protobuf/empty.pb.h
    google/protobuf/field_mask.pb.h
    google/protobuf/generated_message_reflection.h
    google/protobuf/io/gzip_stream.h
    google/protobuf/io/printer.h
    google/protobuf/io/tokenizer.h
    google/protobuf/io/zero_copy_stream_impl.h
    google/protobuf/map_field.h
    google/protobuf/message.h
    google/protobuf/reflection_ops.h
    google/protobuf/service.h
    google/protobuf/source_context.pb.h
    google/protobuf/struct.pb.h
    google/protobuf/stubs/mathlimits.h
    google/protobuf/stubs/substitute.h
    google/protobuf/text_format.h
    google/protobuf/timestamp.pb.h
    google/protobuf/type.pb.h
    google/protobuf/unknown_field_set.h
    google/protobuf/util/delimited_message_util.h
    google/protobuf/util/field_comparator.h
    google/protobuf/util/field_mask_util.h
    google/protobuf/util/internal/datapiece.h
    google/protobuf/util/internal/default_value_objectwriter.h
    google/protobuf/util/internal/error_listener.h
    google/protobuf/util/internal/field_mask_utility.h
    google/protobuf/util/internal/json_escaping.h
    google/protobuf/util/internal/json_objectwriter.h
    google/protobuf/util/internal/json_stream_parser.h
    google/protobuf/util/internal/object_writer.h
    google/protobuf/util/internal/proto_writer.h
    google/protobuf/util/internal/protostream_objectsource.h
    google/protobuf/util/internal/protostream_objectwriter.h
    google/protobuf/util/internal/type_info.h
    google/protobuf/util/internal/type_info_test_helper.h
    google/protobuf/util/internal/utility.h
    google/protobuf/util/json_util.h
    google/protobuf/util/message_differencer.h
    google/protobuf/util/time_util.h
    google/protobuf/util/type_resolver_util.h
    google/protobuf/wire_format.h
    google/protobuf/wrappers.pb.h
)

set(libprotobuf_lite_files
    google/protobuf/arena.cc
    google/protobuf/extension_set.cc
    google/protobuf/generated_message_table_driven_lite.cc
    google/protobuf/generated_message_util.cc
    google/protobuf/implicit_weak_message.cc
    google/protobuf/parse_context.cc
    google/protobuf/io/coded_stream.cc
    google/protobuf/io/strtod.cc
    google/protobuf/io/zero_copy_stream.cc
    google/protobuf/io/zero_copy_stream_impl_lite.cc
    google/protobuf/message_lite.cc
    google/protobuf/repeated_field.cc
    google/protobuf/stubs/bytestream.cc
    google/protobuf/stubs/common.cc
    google/protobuf/stubs/int128.cc
    google/protobuf/stubs/io_win32.cc
    google/protobuf/stubs/status.cc
    google/protobuf/stubs/statusor.cc
    google/protobuf/stubs/stringpiece.cc
    google/protobuf/stubs/stringprintf.cc
    google/protobuf/stubs/structurally_valid.cc
    google/protobuf/stubs/strutil.cc
    google/protobuf/stubs/time.cc
    google/protobuf/wire_format_lite.cc
)

set(libprotobuf_lite_includes
    google/protobuf/arena.h
    google/protobuf/arenastring.h
    google/protobuf/extension_set.h
    google/protobuf/generated_message_util.h
    google/protobuf/implicit_weak_message.h
    google/protobuf/parse_context.h
    google/protobuf/io/coded_stream.h
    google/protobuf/io/strtod.h
    google/protobuf/io/zero_copy_stream.h
    google/protobuf/io/zero_copy_stream_impl_lite.h
    google/protobuf/message_lite.h
    google/protobuf/repeated_field.h
    google/protobuf/stubs/bytestream.h
    google/protobuf/stubs/common.h
    google/protobuf/stubs/int128.h
    google/protobuf/stubs/once.h
    google/protobuf/stubs/status.h
    google/protobuf/stubs/statusor.h
    google/protobuf/stubs/stringpiece.h
    google/protobuf/stubs/stringprintf.h
    google/protobuf/stubs/strutil.h
    google/protobuf/stubs/time.h
    google/protobuf/wire_format_lite.h
)

set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} -w")
add_library(libprotobuf
    ${libprotobuf_files} ${libprotobuf_includes}
    ${libprotobuf_lite_files} ${libprotobuf_lite_includes}
)

if(CMAKE_USE_PTHREADS_INIT)
    add_definitions(-DHAVE_PTHREAD)
endif()

find_package(ZLIB)
if(ZLIB_FOUND)
    target_include_directories(libprotobuf PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(libprotobuf ${ZLIB_LIBRARIES})
endif()

add_definitions(-DGOOGLE_PROTOBUF_CMAKE_BUILD)

set_target_properties(libprotobuf PROPERTIES COMPILE_FLAGS "-Wno-all -Wno-effc++ -Wno-extra -Wno-pedantic")
set_property(TARGET libprotobuf PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(libprotobuf ${CMAKE_THREAD_LIBS_INIT})

# To avoid tho "has no symbol warnings for empty .o's in static libs
if(CMAKE_HOST_APPLE)
    SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()