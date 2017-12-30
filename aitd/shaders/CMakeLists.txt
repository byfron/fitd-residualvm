set(COMP_SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/glsl/)

#TODO: automatize the script for every shader in the folder
set(FSHADER_FILES "")
set(VSHADER_FILES "")
list(APPEND FSHADER_FILES "fs_cubes.sc")
list(APPEND VSHADER_FILES "vs_cubes.sc")
list(APPEND FSHADER_FILES "fs_backg.sc")
list(APPEND VSHADER_FILES "vs_backg.sc")

function(compileShader SHADER_FILE TYPE)

    string(REPLACE ".sc" ".bin" OUT_FILE ${SHADER_FILE})
    set(OUT_FILE ${COMP_SHADERS_DIR}/${OUT_FILE})

    set(SHADERC_ARGS -f ${SHADER_FILE} -o ${OUT_FILE} -i ${ROOT_DIR}/bgfx/src --type ${TYPE} --platform linux)

    set(COMMANDS "")
    list( APPEND COMMANDS COMMAND "${CMAKE_COMMAND}" -E make_directory "${COMP_SHADERS_DIR}" )
    list( APPEND COMMANDS COMMAND ${SHADER_COMPILER} ${SHADERC_ARGS})

    add_custom_command(
      MAIN_DEPENDENCY
      ${SHADER_FILE}
      OUTPUT
      ${OUT_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      ${COMMANDS}
      COMMENT "Compiling shader ${COMMANDS}"
      VERBATIM
      )

endfunction()

foreach(SHADER ${VSHADER_FILES})
  compileShader(${SHADER} "v")
    set(OUT_FILE ${COMP_SHADERS_DIR}/${SHADER})
  string(REPLACE ".sc" ".bin" OUT_FILE ${OUT_FILE})
  list(APPEND OUTPUT_VSHADERS ${OUT_FILE})
endforeach()

foreach(SHADER ${FSHADER_FILES})
  compileShader(${SHADER} "f")
  set(OUT_FILE ${COMP_SHADERS_DIR}/${SHADER})
  string(REPLACE ".sc" ".bin" OUT_FILE ${OUT_FILE})
  list(APPEND OUTPUT_FSHADERS ${OUT_FILE})
endforeach()


add_custom_target(shaders DEPENDS
  ${OUTPUT_VSHADERS}
  ${OUTPUT_FSHADERS})
