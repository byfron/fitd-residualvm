SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

add_definitions(-DENTRY_CONFIG_IMPLEMENT_MAIN)

find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(${ROOT_DIR}/bgfx/examples)
include_directories(${ROOT_DIR}/bgfx/include/)
include_directories(${ROOT_DIR}/bimg/include/)
include_directories(${ROOT_DIR}/bx/include/)
include_directories(${ROOT_DIR}/bgfx/examples/common/)
include_directories(${ROOT_DIR}/bgfx/3rdparty)

SET(IMGUI_SRC
  ${ROOT_DIR}/bgfx/3rdparty/ocornut-imgui/imgui_draw.cpp
  ${ROOT_DIR}/bgfx/3rdparty/ocornut-imgui/imgui_demo.cpp
  ${ROOT_DIR}/bgfx/3rdparty/ocornut-imgui/imgui.cpp
  ${ROOT_DIR}/bgfx/examples/common/imgui/imgui.cpp
  ${ROOT_DIR}/bgfx/examples/common/imgui/scintilla.cpp
  )

SET(BIMG_LIBRARIES
  ${ROOT_DIR}/bimg/.build/linux64_gcc/bin/libbimgRelease.a
  ${ROOT_DIR}/bimg/.build/linux64_gcc/bin/libbimg_encodeRelease.a
  ${ROOT_DIR}/bimg/.build/linux64_gcc/bin/libbimg_decodeRelease.a
  )

SET(BX_LIBRARIES
  ${ROOT_DIR}/bx/.build/linux64_gcc/bin/libbxRelease.a
  )

SET(BGFX_LIBRARIES
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libexample-commonRelease.a
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libexample-glueRelease.a
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libbgfxRelease.a
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libbimg_decodeRelease.a
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libbimgRelease.a
  ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libbxRelease.a
  )

SET(BGFX_COMMON_LIBRARIES ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/libexample-commonRelease.a)
SET(SHADER_COMPILER ${ROOT_DIR}/bgfx/.build/linux64_gcc/bin/shadercRelease)

# add_library(GraphicsEngine
#     GraphicsEngine.hpp
# 	GraphicsEngine.cpp
# 	Shader.hpp
# 	Shader.cpp
# 	Camera.hpp
# 	Camera.cpp
# 	InputManager.hpp
# 	InputManager.cpp
# 	ResourceManager.hpp
# 	ResourceManager.cpp)

#   target_link_libraries(GraphicsEngine
# 	X11
# 	GL
# 	dl
# 	#${BGFX_COMMON_LIBRARIES}
# 	${BGFX_LIBRARIES}
#   )

# add_executable(test test.cpp ${IMGUI_SRC})

# target_link_libraries(test
#   ${BGFX_LIBRARIES}
#   rt
#   pthread
#   GL
#   X11
#   dl
#   )


add_executable(model_viewer
  model_viewer.cpp
  GraphicsEngine.hpp
  GraphicsEngine.cpp
  Shader.hpp
  Shader.cpp
  Camera.hpp
  Camera.cpp
  InputManager.hpp
  InputManager.cpp
  ResourceManager.hpp
  ResourceManager.cpp)

target_link_libraries(model_viewer
   ${BGFX_LIBRARIES}
   rt
   pthread
   GL
   X11
   dl)

 add_dependencies(model_viewer shaders)

 add_subdirectory(shaders)
