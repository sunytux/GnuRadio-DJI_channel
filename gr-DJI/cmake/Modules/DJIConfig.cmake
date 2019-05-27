INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DJI DJI)

FIND_PATH(
    DJI_INCLUDE_DIRS
    NAMES DJI/api.h
    HINTS $ENV{DJI_DIR}/include
        ${PC_DJI_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DJI_LIBRARIES
    NAMES gnuradio-DJI
    HINTS $ENV{DJI_DIR}/lib
        ${PC_DJI_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DJI DEFAULT_MSG DJI_LIBRARIES DJI_INCLUDE_DIRS)
MARK_AS_ADVANCED(DJI_LIBRARIES DJI_INCLUDE_DIRS)

