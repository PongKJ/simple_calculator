include(cmake/SystemLink.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(cmake/CompilerCheck.cmake)

macro(simple_calculator_setup_options)
  # NOTE: enable hardening may cause build failed in debug mode
  option(simple_calculator_ENABLE_HARDENING "Enable hardening" OFF)
  option(simple_calculator_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    simple_calculator_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    simple_calculator_ENABLE_HARDENING
    OFF)

  if(NOT PROJECT_IS_TOP_LEVEL OR simple_calculator_PACKAGING_MAINTAINER_MODE)
    option(simple_calculator_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(simple_calculator_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(simple_calculator_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(simple_calculator_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(simple_calculator_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(simple_calculator_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(simple_calculator_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(simple_calculator_ENABLE_PCH "Enable precompiled headers" OFF)
    option(simple_calculator_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(simple_calculator_ENABLE_IPO "Enable IPO/LTO" ON)
    option(simple_calculator_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(simple_calculator_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(simple_calculator_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(simple_calculator_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(simple_calculator_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(simple_calculator_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(simple_calculator_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(simple_calculator_ENABLE_PCH "Enable precompiled headers" OFF)
    option(simple_calculator_ENABLE_CACHE "Enable ccache" ON)
  endif()
  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      simple_calculator_ENABLE_IPO
      simple_calculator_WARNINGS_AS_ERRORS
      simple_calculator_ENABLE_USER_LINKER
      simple_calculator_ENABLE_SANITIZER_ADDRESS
      simple_calculator_ENABLE_SANITIZER_LEAK
      simple_calculator_ENABLE_SANITIZER_UNDEFINED
      simple_calculator_ENABLE_SANITIZER_THREAD
      simple_calculator_ENABLE_SANITIZER_MEMORY
      simple_calculator_ENABLE_UNITY_BUILD
      simple_calculator_ENABLE_CLANG_TIDY
      simple_calculator_ENABLE_CPPCHECK
      simple_calculator_ENABLE_COVERAGE
      simple_calculator_ENABLE_PCH
      simple_calculator_ENABLE_CACHE)
  endif()

  simple_calculator_check_sanitizer_support("address" SUPPORTS_ASAN)
  simple_calculator_check_sanitizer_support("memory" SUPPORTS_MSAN)
  simple_calculator_check_sanitizer_support("undefined" SUPPORTS_UBSAN)
  simple_calculator_check_sanitizer_support("leak" SUPPORTS_LSAN)
  simple_calculator_check_sanitizer_support("thread" SUPPORTS_TSAN)
  if(NOT SUPPORTS_ASAN AND simple_calculator_ENABLE_SANITIZER_ADDRESS)
    message(WARNING "Address sanitizer is not supported. Disabling simple_calculator_ENABLE_SANITIZER_ADDRESS")
    set(simple_calculator_ENABLE_SANITIZER_ADDRESS OFF)
  endif()
  if(NOT SUPPORTS_MSAN AND simple_calculator_ENABLE_SANITIZER_MEMORY)
    message(WARNING "Memory sanitizer is not supported. Disabling simple_calculator_ENABLE_SANITIZER_MEMORY")
    set(simple_calculator_ENABLE_SANITIZER_MEMORY OFF)
  endif()
  if(NOT SUPPORTS_UBSAN AND simple_calculator_ENABLE_SANITIZER_UNDEFINED)
    message(WARNING "Undefined sanitizer is not supported. Disabling simple_calculator_ENABLE_SANITIZER_UNDEFINED")
    set(simple_calculator_ENABLE_SANITIZER_UNDEFINED OFF)
  endif()
  if(NOT SUPPORTS_LSAN AND simple_calculator_ENABLE_SANITIZER_LEAK)
    message(WARNING "Leak sanitizer is not supported. Disabling simple_calculator_ENABLE_SANITIZER_LEAK")
    set(simple_calculator_ENABLE_SANITIZER_LEAK OFF)
  endif()
  if(NOT SUPPORTS_TSAN AND simple_calculator_ENABLE_SANITIZER_THREAD)
    message(WARNING "Thread sanitizer is not supported. Disabling simple_calculator_ENABLE_SANITIZER_THREAD")
    set(simple_calculator_ENABLE_SANITIZER_THREAD OFF)
  endif()

  simple_calculator_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED
     AND (simple_calculator_ENABLE_SANITIZER_ADDRESS
          OR simple_calculator_ENABLE_SANITIZER_THREAD
          OR simple_calculator_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(simple_calculator_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(simple_calculator_global_options)
  if(simple_calculator_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    simple_calculator_enable_ipo()
  endif()

  if(simple_calculator_ENABLE_HARDENING AND simple_calculator_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR simple_calculator_ENABLE_SANITIZER_UNDEFINED
       OR simple_calculator_ENABLE_SANITIZER_ADDRESS
       OR simple_calculator_ENABLE_SANITIZER_THREAD
       OR simple_calculator_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message(
      "${simple_calculator_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${simple_calculator_ENABLE_SANITIZER_UNDEFINED}")
    simple_calculator_enable_hardening(simple_calculator_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(simple_calculator_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(simple_calculator_warnings INTERFACE)
  add_library(simple_calculator_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  simple_calculator_set_project_warnings(
    simple_calculator_warnings
    ${simple_calculator_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(simple_calculator_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    simple_calculator_configure_linker(simple_calculator_options)
  endif()

  include(cmake/Sanitizers.cmake)
  simple_calculator_enable_sanitizers(
    simple_calculator_options
    ${simple_calculator_ENABLE_SANITIZER_ADDRESS}
    ${simple_calculator_ENABLE_SANITIZER_LEAK}
    ${simple_calculator_ENABLE_SANITIZER_UNDEFINED}
    ${simple_calculator_ENABLE_SANITIZER_THREAD}
    ${simple_calculator_ENABLE_SANITIZER_MEMORY})

  set_target_properties(simple_calculator_options PROPERTIES UNITY_BUILD ${simple_calculator_ENABLE_UNITY_BUILD})

  if(simple_calculator_ENABLE_PCH)
    target_precompile_headers(
      simple_calculator_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(simple_calculator_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    simple_calculator_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(simple_calculator_ENABLE_CLANG_TIDY)
    simple_calculator_enable_clang_tidy(simple_calculator_options ${simple_calculator_WARNINGS_AS_ERRORS})
  endif()

  if(simple_calculator_ENABLE_CPPCHECK)
    simple_calculator_enable_cppcheck(${simple_calculator_WARNINGS_AS_ERRORS} "") # override cppcheck options
  endif()

  if(simple_calculator_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    simple_calculator_enable_coverage(simple_calculator_options)
  endif()

  if(simple_calculator_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(simple_calculator_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(simple_calculator_ENABLE_HARDENING AND NOT simple_calculator_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR simple_calculator_ENABLE_SANITIZER_UNDEFINED
       OR simple_calculator_ENABLE_SANITIZER_ADDRESS
       OR simple_calculator_ENABLE_SANITIZER_THREAD
       OR simple_calculator_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    simple_calculator_enable_hardening(simple_calculator_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
