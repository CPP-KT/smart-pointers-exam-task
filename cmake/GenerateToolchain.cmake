option(CT_HARDENED "Should the standard library be hardened" OFF)
option(CT_SANITIZED "Should the build be sanitized" OFF)

function(ct_gen_toolchain)
  if(PROJECT_NAME)
    message(FATAL_ERROR "ct_gen_toolchain must be run before the first project directive")
  endif()

  if(NOT CT_TOOLCHAIN_TEMPLATE)
    return()
  endif()

  if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Skipped generating toolchain file because CMAKE_TOOLCHAIN_FILE is already set")
    return()
  endif()

  cmake_path(GET CT_TOOLCHAIN_TEMPLATE STEM CT_TOOLCHAIN_NAME)
  set(CT_GENERATED_TOOLCHAIN "${CMAKE_BINARY_DIR}/toolchains/${CT_TOOLCHAIN_NAME}.cmake")

  configure_file("${CT_TOOLCHAIN_TEMPLATE}" "${CT_GENERATED_TOOLCHAIN}" @ONLY)
  message(STATUS "Generated a toolchain '${CT_TOOLCHAIN_NAME}' from template")

  set(CMAKE_TOOLCHAIN_FILE "${CT_GENERATED_TOOLCHAIN}" CACHE FILEPATH "")
endfunction()
