include(FetchContent)

option(CT_FETCH_VCPKG "Should the vcpkg be fetched via FetchContent" ON)

function(ct_fetch_vcpkg)
  if(PROJECT_NAME)
    message(FATAL_ERROR "ct_fetch_vcpkg must be run before the first project directive")
  endif()

  if(NOT CT_FETCH_VCPKG)
    message(STATUS "Skipped fetching vcpkg because CT_FETCH_VCPKG=OFF")
    return()
  endif()

  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake$")
    message(STATUS "Skipped fetching vcpkg because CMAKE_TOOLCHAIN_FILE is already set to vcpkg.cmake")
    return()
  endif()

  message(STATUS "Fetching vcpkg...")
  FetchContent_Declare(
    vcpkg
    URL https://github.com/microsoft/vcpkg/archive/refs/tags/2025.02.14.tar.gz
  )
  FetchContent_MakeAvailable(vcpkg)

  set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics" CACHE STRING "")

  if(CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE FILEPATH "")
  endif()
  set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake"
    CACHE FILEPATH "Vcpkg toolchain file" FORCE)

  if(CT_TRIPLET_TEMPLATE)
    cmake_path(GET CT_TRIPLET_TEMPLATE STEM CT_TRIPLET_NAME)

    configure_file(
      "${CT_TRIPLET_TEMPLATE}"
      "${CMAKE_BINARY_DIR}/vcpkg-triplets/${CT_TRIPLET_NAME}.cmake"
      @ONLY
    )
    message(STATUS "Generated a vcpkg triplet '${CT_TRIPLET_NAME}' from template")

    set(VCPKG_TARGET_TRIPLET "${CT_TRIPLET_NAME}" CACHE FILEPATH "")
    set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_BINARY_DIR}/vcpkg-triplets" CACHE FILEPATH "")
  endif()
endfunction()
