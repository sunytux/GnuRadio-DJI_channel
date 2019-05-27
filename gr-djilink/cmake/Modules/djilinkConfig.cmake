INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DJILINK djilink)

FIND_PATH(
    DJILINK_INCLUDE_DIRS
    NAMES djilink/api.h
    HINTS $ENV{DJILINK_DIR}/include
        ${PC_DJILINK_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DJILINK_LIBRARIES
    NAMES gnuradio-djilink
    HINTS $ENV{DJILINK_DIR}/lib
        ${PC_DJILINK_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DJILINK DEFAULT_MSG DJILINK_LIBRARIES DJILINK_INCLUDE_DIRS)
MARK_AS_ADVANCED(DJILINK_LIBRARIES DJILINK_INCLUDE_DIRS)

