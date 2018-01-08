add_library(AITDViewerCore
  AITDViewer.hpp
  AITDViewer.cpp
)

target_link_libraries(AITDViewerCore
  AITDCore
  EntitiesCore
  )


target_include_directories(AITDViewerCore PUBLIC
  ${SDL_INCLUDE_DIR})

