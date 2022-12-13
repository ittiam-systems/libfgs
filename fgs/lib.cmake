add_library(libSources INTERFACE)
add_library(simdFiles INTERFACE)

target_sources(libSources
               INTERFACE "${PROJECT_ROOT}/fgs/src/grain_simulation_api.c")
target_include_directories(
  libSources
  INTERFACE "${PROJECT_ROOT}/fgs/include"
  INTERFACE "${PROJECT_ROOT}/fgs/api")

if(${ENABLE_SIMD})
  if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    target_sources(
      simdFiles INTERFACE "${PROJECT_ROOT}/fgs/arm/grain_simulation_modules.c")
  else()
    target_sources(
      simdFiles INTERFACE "${PROJECT_ROOT}/fgs/x86/grain_simulation_modules.c")
  endif()
else()
  target_sources(simdFiles
                 INTERFACE "${PROJECT_ROOT}/fgs/src/grain_simulation_modules.c")
endif()

add_library(${projName})
target_link_libraries(
  ${projName}
  PRIVATE ithread
  PRIVATE libSources
  PRIVATE simdFiles
  PRIVATE "${projName}CompileOptions")
set_target_properties(${projName} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
