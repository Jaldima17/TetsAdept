#.rst:
# FindFirebird
# -----------
#
# Find the Firebird encryption library.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Firebird::fbclient``
#   The Firebird ``fbclient`` library, if found.
# ``Firebird::ib_util``
#   The Firebird ``ib_util`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``FIREBIRD_FOUND``
#   System has the Firebird library.
# ``FIREBIRD_INCLUDE_DIR``
#   The Firebird include directory.
# ``FIREBIRD_IBUTIL_LIBRARY``
#   The Firebird ib_util library.
# ``FIREBIRD_FBCLIENT_LIBRARY``
#   The Firebird fbclient library.
# ``FIREBIRD_LIBRARIES``
#   All Firebird libraries.
# ``FIREBIRD_VERSION``
#   This is set to ``$major.$minor.$revision$patch`` (e.g. ``0.9.8s``).
#
# Hints
# ^^^^^
#
# Set ``FIREBIRD_ROOT_DIR`` to the root directory of an Firebird installation.

function (detect_64bit_target)
  if (CMAKE_CROSSCOMPILING)
    return ()
  endif ()

  # Collect information about the build platform.
  include(CheckTypeSize)

  check_type_size(void* void_ptr_size BUILTIN_TYPES_ONLY)
  if (void_ptr_size EQUAL 8)
    set(_is_64bit TRUE PARENT_SCOPE)
  elseif (void_ptr_size EQUAL 4)
    set(_is_64bit FALSE  PARENT_SCOPE)
  else ()
    if (WIN32)
      set(extra_message "Are you in a Visual Studio command prompt?")
    else ()
      set(extra_message "Do you have working compilers?")
    endif ()
    message(FATAL_ERROR "Failed to determine whether the target architecture is 32bit or 64bit. ${extra_message}")
  endif ()
endfunction ()

detect_64bit_target()


if (UNIX)
  find_package(PkgConfig QUIET)
  pkg_check_modules(_FIREBIRD QUIET Firebird)
endif ()

if (WIN32)
  file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _programfiles)
  set(_FIREBIRD_ROOT_HINTS
    ${FIREBIRD_ROOT_DIR}
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\FBDBServer_2_5_is1;Inno Setup: App Path]"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\FBDBServer_2_5_x64_is1;Inno Setup: App Path]"
    ENV FIREBIRD_ROOT_DIR
    "${_programfiles}/Firebird/*/"
    )
  set(_FIREBIRD_ROOT_PATHS
    "${_programfiles}/Firebird"
    "C:/Firebird/"
    )
  unset(_programfiles)
else ()
  set(_FIREBIRD_ROOT_HINTS
    ${FIREBIRD_ROOT_DIR}
    ENV FIREBIRD_ROOT_DIR
    )
endif ()

set(_FIREBIRD_ROOT_HINTS_AND_PATHS
    HINTS ${_FIREBIRD_ROOT_HINTS}
    PATHS ${_FIREBIRD_ROOT_PATHS}
    )

find_path(FIREBIRD_INCLUDE_DIR
  NAMES
    ibase.h
    ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
  HINTS
    ${_FIREBIRD_INCLUDEDIR}
  PATH_SUFFIXES
    include
)

if(WIN32)
  if (_is_64bit)
    set(FIREBIRD_PLAT_SUFFIXES)
    set(FIREBIRD_PLAT_BIN bin)
  else ()
    set(FIREBIRD_PLAT_SUFFIXES "WOW64/")
    set(FIREBIRD_PLAT_BIN)
  endif ()

  find_library(FIREBIRD_IBUTIL
    NAMES
      ib_util_ms
    NAMES_PER_DIR
      ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    HINTS
      ${_FIREBIRD_LIBDIR}
    PATH_SUFFIXES
      ${FIREBIRD_PLAT_SUFFIXES}lib
    NO_DEFAULT_PATH
  )

  find_library(FIREBIRD_FBCLIENT
    NAMES
      fbclient_ms
    NAMES_PER_DIR
      ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    HINTS
      ${_FIREBIRD_LIBDIR}
    PATH_SUFFIXES
      ${FIREBIRD_PLAT_SUFFIXES}lib
    NO_DEFAULT_PATH
  )

  #=============================================================================
  # Register imported libraries:
  # 1. If we can find a Windows .dll file (or if we can find both Debug and
  #    Release libraries), we will set appropriate target properties for these.
  # 2. However, for most systems, we will only register the import location and
  #    include directory.

  find_file(FIREBIRD_IBUTIL_DLL
    #NAMES
      ib_util.dll
    PATHS
      ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      ${FIREBIRD_PLAT_SUFFIXES}${FIREBIRD_PLAT_BIN}
    NO_DEFAULT_PATH
  )

  find_file(FIREBIRD_FBCLIENT_DLL
    #NAMES
      fbclient.dll
    PATHS
      ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      ${FIREBIRD_PLAT_SUFFIXES}${FIREBIRD_PLAT_BIN}
    NO_DEFAULT_PATH
  )

  mark_as_advanced(FIREBIRD_FBCLIENT FIREBIRD_IBUTIL)

  set(FIREBIRD_FBCLIENT_LIBRARY     ${FIREBIRD_FBCLIENT} )
  set(FIREBIRD_FBCLIENT_LIBRARY_DLL ${FIREBIRD_FBCLIENT_DLL} )
  set(FIREBIRD_IBUTIL_LIBRARY       ${FIREBIRD_IBUTIL} )
  set(FIREBIRD_IBUTIL_LIBRARY_DLL   ${FIREBIRD_IBUTIL_DLL} )
  set(FIREBIRD_LIBRARIES ${FIREBIRD_FBCLIENT} ${FIREBIRD_IBUTIL_LIBRARY} )
else()

  find_library(FIREBIRD_FBCLIENT_LIBRARY
    NAMES
      fbclient
    NAMES_PER_DIR
    ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    HINTS
      ${_FIREBIRD_LIBDIR}
    PATH_SUFFIXES
      lib
  )

  find_library(FIREBIRD_IBUTIL_LIBRARY
    NAMES
      ib_util
    NAMES_PER_DIR
    ${_FIREBIRD_ROOT_HINTS_AND_PATHS}
    HINTS
      ${_FIREBIRD_LIBDIR}
    PATH_SUFFIXES
      lib
  )

  mark_as_advanced(FIREBIRD_IBUTIL_LIBRARY FIREBIRD_FBCLIENT_LIBRARY)

  # compat defines
  set(FIREBIRD_FBCLIENT_LIBRARIES ${FIREBIRD_FBCLIENT_LIBRARY})
  set(FIREBIRD_IBUTIL_LIBRARIES ${FIREBIRD_IBUTIL_LIBRARY})
  set(FIREBIRD_LIBRARIES ${FIREBIRD_FBCLIENT_LIBRARY} ${FIREBIRD_IBUTIL_LIBRARY})

endif()

if (FIREBIRD_INCLUDE_DIR)
  if(FIREBIRD_INCLUDE_DIR AND EXISTS "${FIREBIRD_INCLUDE_DIR}/ibase.h")
    file(STRINGS "${FIREBIRD_INCLUDE_DIR}/ibase.h" Firebird_version_str REGEX "^#[\t ]*define[\t ]+FB_API_VER[\t ]+([0-9a-fA-F])+.*")

    # The version number is encoded as 0xMNNFFPPS: major minor fix patch status
    # The status gives if this is a developer or prerelease and is ignored here.
    # Major, minor, and fix directly translate into the version numbers shown in
    # the string. The patch field translates to the single character suffix that
    # indicates the bug fix state, which 00 -> nothing, 01 -> a, 02 -> b and so
    # on.

    string(REGEX REPLACE "^.*FB_API_VER[\t ]+([0-9a-fA-F])([0-9a-fA-F])" "\\1;\\2" FIREBIRD_VERSION_LIST "${Firebird_version_str}")
    list(GET FIREBIRD_VERSION_LIST 0 FIREBIRD_VERSION_MAJOR)
    list(GET FIREBIRD_VERSION_LIST 1 FIREBIRD_VERSION_MINOR)

    set(FIREBIRD_VERSION "${FIREBIRD_VERSION_MAJOR}.${FIREBIRD_VERSION_MINOR}")
  endif ()
endif ()

include(FindPackageHandleStandardArgs)

if (FIREBIRD_VERSION)
  find_package_handle_standard_args(Firebird
    REQUIRED_VARS
      FIREBIRD_LIBRARIES
      FIREBIRD_INCLUDE_DIR
    VERSION_VAR
      FIREBIRD_VERSION
    FAIL_MESSAGE
      "Could NOT find Firebird, try to set the path to Firebird root folder in the system variable FIREBIRD_ROOT_DIR"
  )
else ()
  find_package_handle_standard_args(Firebird "Could NOT find Firebird, try to set the path to Firebird root folder in the system variable FIREBIRD_ROOT_DIR"
    FIREBIRD_LIBRARIES
    FIREBIRD_INCLUDE_DIR
  )
endif ()

mark_as_advanced(FIREBIRD_INCLUDE_DIR FIREBIRD_LIBRARIES)

if(FIREBIRD_FOUND)

  if(NOT TARGET Firebird::ib_util AND (EXISTS "${FIREBIRD_IBUTIL_LIBRARY}"))

    add_library(Firebird::ib_util SHARED IMPORTED)
    set_target_properties(Firebird::ib_util PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FIREBIRD_INCLUDE_DIR}")
    if (EXISTS "${FIREBIRD_IBUTIL_LIBRARY_DLL}")
      set_target_properties(Firebird::ib_util PROPERTIES
        #IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${FIREBIRD_IBUTIL_LIBRARY_DLL}"
        IMPORTED_IMPLIB "${FIREBIRD_IBUTIL_LIBRARY}")
    else ()
      set_target_properties(Firebird::ib_util PROPERTIES
        #IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${FIREBIRD_IBUTIL_LIBRARY}")
    endif()

  endif()


  if(NOT TARGET Firebird::fbclient AND (EXISTS "${FIREBIRD_FBCLIENT_LIBRARY}"))

    add_library(Firebird::fbclient SHARED IMPORTED)
    set_target_properties(Firebird::fbclient PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FIREBIRD_INCLUDE_DIR}")
    if (EXISTS "${FIREBIRD_FBCLIENT_LIBRARY_DLL}")
      set_target_properties(Firebird::fbclient PROPERTIES
        #IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${FIREBIRD_FBCLIENT_LIBRARY_DLL}"
        IMPORTED_IMPLIB "${FIREBIRD_FBCLIENT_LIBRARY}")
    else ()
      set_target_properties(Firebird::fbclient PROPERTIES
        #IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${FIREBIRD_FBCLIENT_LIBRARY}")
    endif ()

  endif()

endif()
