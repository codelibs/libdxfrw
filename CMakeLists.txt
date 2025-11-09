cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(libdxfrw VERSION 0.6.3)

# Enable testing
enable_testing()

file(GLOB libdxfrw_sources src/*.cpp)
file(GLOB libdxfrw_headers include/*.h)
file(GLOB libdxfrw_intern_sources src/intern/*.cpp)

# Find iconv library
if(WIN32)
    include_directories(vs2013/packages/libiconv.1.14.0.11/build/native/include)
    link_directories(vs2013/packages/libiconv.1.14.0.11/build/native/lib)
    set(ICONV_LIBRARY iconv)
else()
    # On Unix-like systems (Linux, macOS)
    find_package(Iconv REQUIRED)
    if(Iconv_FOUND)
        include_directories(${Iconv_INCLUDE_DIRS})
        set(ICONV_LIBRARY ${Iconv_LIBRARIES})
    else()
        # Fallback: try to link against iconv directly
        set(ICONV_LIBRARY iconv)
    endif()
endif()

include_directories(include)

add_library(dxfrw STATIC ${libdxfrw_sources} ${libdxfrw_intern_sources})
target_link_libraries(dxfrw ${ICONV_LIBRARY})

install(FILES ${libdxfrw_headers} DESTINATION include)

if(WIN32)
  install(TARGETS dxfrw
          CONFIGURATIONS Debug
          LIBRARY DESTINATION Debug/lib
          ARCHIVE DESTINATION Debug/lib)
  install(TARGETS dxfrw
          CONFIGURATIONS Release
          LIBRARY DESTINATION Release/lib
          ARCHIVE DESTINATION Release/lib)
else()
  install(TARGETS dxfrw
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
endif()

# Tests
add_executable(test_basic tests/test_basic.cpp)
target_include_directories(test_basic PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_basic dxfrw ${ICONV_LIBRARY})
add_test(NAME BasicTests COMMAND test_basic)

add_executable(test_entities tests/test_entities.cpp)
target_include_directories(test_entities PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_entities dxfrw ${ICONV_LIBRARY})
add_test(NAME EntityTests COMMAND test_entities)

add_executable(test_polylines tests/test_polylines.cpp)
target_include_directories(test_polylines PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_polylines dxfrw ${ICONV_LIBRARY})
add_test(NAME PolylineTests COMMAND test_polylines)

add_executable(test_text tests/test_text.cpp)
target_include_directories(test_text PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_text dxfrw ${ICONV_LIBRARY})
add_test(NAME TextTests COMMAND test_text)

add_executable(test_tables tests/test_tables.cpp)
target_include_directories(test_tables PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_tables dxfrw ${ICONV_LIBRARY})
add_test(NAME TableTests COMMAND test_tables)

add_executable(test_blocks tests/test_blocks.cpp)
target_include_directories(test_blocks PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_blocks dxfrw ${ICONV_LIBRARY})
add_test(NAME BlockTests COMMAND test_blocks)

add_executable(test_versions tests/test_versions.cpp)
target_include_directories(test_versions PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_versions dxfrw ${ICONV_LIBRARY})
add_test(NAME VersionTests COMMAND test_versions)

add_executable(test_errors tests/test_errors.cpp)
target_include_directories(test_errors PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_errors dxfrw ${ICONV_LIBRARY})
add_test(NAME ErrorTests COMMAND test_errors)