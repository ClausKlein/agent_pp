# Find snmp++ library
#
# Optional: SNMP_PP_ROOT_DIR - where snmp++ library is installed
#
# The following variables are set:
#  SNMP_PP_FOUND
#  SNMP_PP_INCLUDE_DIR
#  SNMP_PP_LIBRARIES

find_path(SNMP_PP_INCLUDE_DIR_TMP NAMES snmp_pp/snmp_pp.h PATHS ${SNMP_PP_ROOT_DIR} ../snmp++ ../../snmp++ PATH_SUFFIXES include)

if(SNMP_PP_INCLUDE_DIR_TMP)
  set(SNMP_PP_INCLUDE_DIR "${SNMP_PP_INCLUDE_DIR_TMP}" "${SNMP_PP_INCLUDE_DIR_TMP}/snmp_pp")
endif()

unset(SNMP_PP_INCLUDE_DIR_TMP)

find_library(SNMP_PP_LIBRARIES NAMES snmp++ libsnmp++ PATHS ${SNMP_PP_ROOT_DIR} ../snmp++ ../../snmp++ PATH_SUFFIXES lib build)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(snmp_pp DEFAULT_MSG SNMP_PP_INCLUDE_DIR SNMP_PP_LIBRARIES)

if(SNMP_PP_FOUND)
  message(STATUS "Found snmp++ (include: ${SNMP_PP_INCLUDE_DIR}, library: ${SNMP_PP_LIBRARIES})")
  mark_as_advanced(SNMP_PP_INCLUDE_DIR SNMP_PP_LIBRARIES)
else()
  message(STATUS "snmp++ NOT found (include: ${SNMP_PP_INCLUDE_DIR}, library: ${SNMP_PP_LIBRARIES})")
endif()


