add_library(ParsingUtils
  DataParsing.hpp
  DataParsing.cpp)

add_library(GeometryUtils
  Geometry.hpp
  Geometry.cpp)

add_library(CollisionUtils
  CollisionUtils.hpp
  CollisionUtils.cpp)

if (BUILD_TESTS)
cxx_test(geometry_test
  test_Geometry.cpp)
endif()
