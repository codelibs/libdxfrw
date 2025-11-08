cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(libdxfrw VERSION 0.6.3)

# Enable testing
enable_testing()

file(GLOB libdxfrw_sources src/*.cpp)
file(GLOB libdxfrw_headers include/*.h)
file(GLOB libdxfrw_intern_sources src/intern/*.cpp)

if(WIN32)

include_directories(vs2013/packages/libiconv.1.14.0.11/build/native/include)
link_directories(vs2013/packages/libiconv.1.14.0.11/build/native/lib)
endif()
include_directories(include)

add_library(dxfrw STATIC ${libdxfrw_sources} ${libdxfrw_intern_sources})

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
target_link_libraries(test_basic dxfrw)

add_test(NAME BasicTests COMMAND test_basic)