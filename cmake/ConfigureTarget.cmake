function(ct_configure_target TARGET)
  if(CT_COMPILER_FLAGS OR CT_LINKER_FLAGS)
    message(STATUS "No need to configure '${TARGET}': Using global toolchain options")
    return()
  endif()

  include(ConfigureCompiler)
  ct_configure_compiler("${CMAKE_CXX_COMPILER_ID}" "${CT_HARDENED}" "${CT_SANITIZED}" ON)

  separate_arguments(CT_COMPILER_FLAGS)
  separate_arguments(CT_LINKER_FLAGS)

  target_compile_options("${TARGET}" PRIVATE ${CT_COMPILER_FLAGS})
  target_link_options("${TARGET}" PRIVATE ${CT_LINKER_FLAGS})
endfunction()
