add_library(GraphicsCore
  Camera.hpp
  Camera.cpp
  GraphicsEngine.hpp
  GraphicsEngine.cpp
  RenderSystem.hpp
  RenderSystem.cpp)


target_link_libraries(GraphicsCore
  ${BGFX_LIBRARIES})

  
target_include_directories(GraphicsCore PUBLIC
  ${SDL_INCLUDE_DIR})
