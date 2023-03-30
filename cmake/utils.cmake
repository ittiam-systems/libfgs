function(defineInterfaces)
  add_library("${projName}CompileOptions" INTERFACE)
  target_compile_features("${projName}CompileOptions" INTERFACE c_std_99)

  if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    target_compile_options(
      "${projName}CompileOptions"
      INTERFACE /INCREMENTAL:NO
                /Oi
                /GL
                /Os
                /MP
                /W4
                /WX)
    target_compile_definitions("${projName}CompileOptions"
                               INTERFACE WINDOWS _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options("${projName}CompileOptions"
                           INTERFACE -Wall -Wextra -Wpedantic -Werror)

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
      target_compile_definitions("${projName}CompileOptions" INTERFACE MACOS)
    else()
      target_compile_definitions("${projName}CompileOptions" INTERFACE LINUX)
    endif()

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
      target_compile_options("${projName}CompileOptions"
                             INTERFACE -march=armv8-a)
    else()
      target_compile_options("${projName}CompileOptions"
                             INTERFACE -march=native)
    endif()
  endif()
endfunction()
