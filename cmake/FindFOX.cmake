# FindFOX.cmake - locate the FOX Toolkit (version 2.x)
#
# Search order:
#   1. pkg-config module fox-2.0
#   2. fox-config executable (from FOX_ROOT, then PATH)
#   3. Manual lookup of fxver.h and libFOX-2.0
#
# Hints:
#   FOX_ROOT, FOX_DIR, FOX_PREFIX   installation prefix
#   FOX_INCLUDE_DIR                 directory containing fx.h and fxver.h
#   FOX_LIBRARY                     full path to libFOX-2.0
#   FOX_CONFIG                      full path to fox-config
#
# Result:
#   FOX_FOUND
#   FOX::FOX                        imported interface target

if(TARGET FOX::FOX)
  set(FOX_FOUND TRUE)
  return()
endif()

set(_fox_version_major "${FOX_FIND_VERSION_MAJOR}")
set(_fox_version_minor "${FOX_FIND_VERSION_MINOR}")
if(NOT _fox_version_major)
  set(_fox_version_major 2)
endif()
if(NOT _fox_version_minor)
  set(_fox_version_minor 0)
endif()

# Collect user supplied prefixes.
set(_fox_hints)
foreach(_fox_hint_var FOX_ROOT FOX_DIR FOX_PREFIX)
  if(DEFINED ${_fox_hint_var} AND NOT "${${_fox_hint_var}}" STREQUAL "")
    list(APPEND _fox_hints "${${_fox_hint_var}}")
  endif()
endforeach()

# 1. pkg-config.
set(_fox_library_dirs)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(PC_FOX QUIET fox-2.0)
  if(PC_FOX_FOUND)
    find_path(FOX_INCLUDE_DIR fxver.h
      HINTS ${PC_FOX_INCLUDEDIR} ${PC_FOX_INCLUDE_DIRS}
      NO_DEFAULT_PATH)
    list(APPEND _fox_library_dirs ${PC_FOX_LIBDIR} ${PC_FOX_LIBRARY_DIRS})
    set(_fox_pkg_config_libraries ${PC_FOX_LIBRARIES})
    pkg_get_variable(_fox_platform_libs fox-2.0 PLATFORM_LIBS)
  endif()
endif()

# 2. fox-config.
if(NOT FOX_CONFIG)
  find_program(FOX_CONFIG NAMES fox-config HINTS ${_fox_hints} PATH_SUFFIXES bin)
endif()
if(FOX_CONFIG AND NOT _fox_pkg_config_libraries AND NOT _fox_hints)
  execute_process(COMMAND "${FOX_CONFIG}" --version
    OUTPUT_VARIABLE _fox_config_version
    RESULT_VARIABLE _fox_config_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND "${FOX_CONFIG}" --cflags
    OUTPUT_VARIABLE _fox_config_cflags
    RESULT_VARIABLE _fox_config_cflags_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND "${FOX_CONFIG}" --libs
    OUTPUT_VARIABLE _fox_config_libs
    RESULT_VARIABLE _fox_config_libs_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(_fox_config_version_result EQUAL 0
     AND _fox_config_version MATCHES "^2\\."
     AND _fox_config_cflags_result EQUAL 0
     AND _fox_config_libs_result EQUAL 0)
    separate_arguments(_fox_config_cflags UNIX_COMMAND "${_fox_config_cflags}")
    separate_arguments(_fox_config_libs UNIX_COMMAND "${_fox_config_libs}")
    set(_fox_platform_libs "${_fox_config_libs}")
    foreach(_flag IN LISTS _fox_config_cflags)
      if(_flag MATCHES "^-I(.*)")
        set(FOX_INCLUDE_DIR "${CMAKE_MATCH_1}" CACHE PATH "FOX include directory")
      endif()
    endforeach()
    foreach(_flag IN LISTS _fox_config_libs)
      if(_flag MATCHES "^-l(.*)")
        list(APPEND _fox_pkg_config_libraries "${CMAKE_MATCH_1}")
      endif()
    endforeach()
  endif()
endif()

# 3. Manual lookup.
if(NOT FOX_INCLUDE_DIR)
  find_path(FOX_INCLUDE_DIR
    NAMES fxver.h
    HINTS ${_fox_hints}
    PATHS /usr/local /usr /opt/local /opt/homebrew
    PATH_SUFFIXES include/fox-2.0 include/fox-2.0.0 include)
endif()

if(NOT FOX_LIBRARY)
  find_library(FOX_LIBRARY
    NAMES FOX-2.0 libFOX-2.0 FOX
    HINTS ${_fox_hints} ${_fox_library_dirs}
    PATHS /usr/local /usr /opt/local /opt/homebrew
    PATH_SUFFIXES lib lib64)
endif()

# Validate the version in fxver.h.
set(_fox_version_ok TRUE)
if(FOX_INCLUDE_DIR AND EXISTS "${FOX_INCLUDE_DIR}/fxver.h")
  file(STRINGS "${FOX_INCLUDE_DIR}/fxver.h" _fox_major_line REGEX "^#define[ \t]+FOX_MAJOR[ \t]+[0-9]+")
  file(STRINGS "${FOX_INCLUDE_DIR}/fxver.h" _fox_minor_line REGEX "^#define[ \t]+FOX_MINOR[ \t]+[0-9]+")
  file(STRINGS "${FOX_INCLUDE_DIR}/fxver.h" _fox_level_line REGEX "^#define[ \t]+FOX_LEVEL[ \t]+[0-9]+")
  if(_fox_major_line MATCHES "([0-9]+)")
    set(FOX_VERSION_MAJOR "${CMAKE_MATCH_1}")
  endif()
  if(_fox_minor_line MATCHES "([0-9]+)")
    set(FOX_VERSION_MINOR "${CMAKE_MATCH_1}")
  endif()
  if(_fox_level_line MATCHES "([0-9]+)")
    set(FOX_VERSION_LEVEL "${CMAKE_MATCH_1}")
  endif()
  if(FOX_VERSION_MAJOR AND FOX_VERSION_MINOR)
    set(FOX_VERSION "${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}")
    if(FOX_VERSION_LEVEL)
      string(APPEND FOX_VERSION ".${FOX_VERSION_LEVEL}")
    endif()
  endif()
  if(FOX_VERSION_MAJOR AND NOT FOX_VERSION_MAJOR EQUAL _fox_version_major)
    set(_fox_version_ok FALSE)
  endif()
  if(FOX_VERSION_MINOR AND FOX_VERSION_MINOR LESS _fox_version_minor)
    set(_fox_version_ok FALSE)
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FOX
  REQUIRED_VARS FOX_LIBRARY FOX_INCLUDE_DIR _fox_version_ok
  VERSION_VAR FOX_VERSION
  REASON_FAILURE_MESSAGE
    "Install the FOX Toolkit 2.x or set FOX_ROOT to its installation prefix.")

# Tell the compiler which backend the library was built for, so the public
# headers select matching platform declarations.
if(_fox_platform_libs MATCHES "wayland-client")
  set(FOX_PLATFORM "wayland")
elseif(_fox_platform_libs MATCHES "-lX11")
  set(FOX_PLATFORM "x11")
endif()

if(FOX_FOUND AND NOT TARGET FOX::FOX)
  add_library(FOX::FOX INTERFACE IMPORTED)
  set_target_properties(FOX::FOX PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${FOX_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${FOX_LIBRARY}")
  if(_fox_pkg_config_libraries)
    list(REMOVE_DUPLICATES _fox_pkg_config_libraries)
    set_property(TARGET FOX::FOX APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES ${_fox_pkg_config_libraries})
  endif()
  if(FOX_PLATFORM STREQUAL "wayland")
    set_property(TARGET FOX::FOX APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS FOX_WAYLAND)
  elseif(FOX_PLATFORM STREQUAL "x11")
    set_property(TARGET FOX::FOX APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS FOX_X11)
  endif()
endif()

mark_as_advanced(FOX_INCLUDE_DIR FOX_LIBRARY FOX_CONFIG)
