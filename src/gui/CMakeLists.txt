find_package(spdlog REQUIRED)
find_package(dbg-macro REQUIRED)
find_package(Qt6 REQUIRED)

qt_standard_project_setup()
qt_add_executable(
  gui
  main.cc
  mainwindow.cc
  displayer.cc)

target_link_libraries(gui PRIVATE simple_calculator::simple_calculator_options
                                  simple_calculator::simple_calculator_warnings)
target_link_system_libraries(
  gui
  PRIVATE
  spdlog::spdlog
  dbg-macro::dbg-macro
  qt::qt)

target_include_directories(gui PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_include_directories(gui ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

set_target_properties(gui PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)

qt_generate_deploy_app_script(
  TARGET
  gui
  #NO_COMPILER_RUNTIME
  OUTPUT_SCRIPT
  deploy_script)
install(SCRIPT ${deploy_script})
