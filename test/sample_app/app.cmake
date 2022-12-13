add_library(appSources INTERFACE)
add_library(appImports INTERFACE)

target_sources(appSources INTERFACE "${PROJECT_ROOT}/test/sample_app/fgs_app.c")

target_include_directories(appImports INTERFACE "${PROJECT_ROOT}/fgs/api")

set(appName "${projName}app")
add_executable(${appName})
target_link_libraries(
  ${appName}
  PRIVATE appSources
  PRIVATE appImports
  PRIVATE "${projName}CompileOptions"
  PRIVATE ${projName})
set_target_properties(${appName} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

include(GNUInstallDirs)
install(TARGETS ${appName} RUNTIME)
