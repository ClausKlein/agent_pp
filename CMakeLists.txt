cmake_minimum_required(VERSION 3.21...3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# NOTE: We need LANGUAGE C too for libdes. It may be required to build at Windows! CK
project(
  agent_pp
  VERSION 4.5.1.12
  LANGUAGES CXX C
)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckStructHasMember)
include(CheckTypeSize)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# =======================================
set(CMAKE_CXX_STANDARD 17)
# =======================================
# NOTE: in consideration of std=c++17! CK
set(CNF_STDCXX_98_HEADERS YES)
set(HAVE_INTTYPES_H YES)
set(AGENT_PP_NAMESPACE on) # "Enable Namespaces (needed!)" ON)

include(Features.cmake)

option(AGENT_PP_PROXY "Enable Proxy" ON)
option(AGENT_PP_PROXY_FORWARDER "Enable Proxy Forwarder" ON)
option(AGENT_PP_AGENPRO "build with AgenPro templates (usually disabled)" OFF)
option(AGENT_PP_AGENPRO_SIMAGENT "build with AgenPro simagent instead regular templates (usually disabled)" OFF)
option(AGENT_PP_EXAMPLES "build examples" ON)
option(AGENT_PP_TOOLS "build tools" OFF)

set(VERSION ${PROJECT_VERSION})
set(AGENT_PP_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(AGENT_PP_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(AGENT_PP_MICRO_VERSION ${PROJECT_VERSION_PATCH})

message(STATUS "agent++ Version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

check_include_files(sys/param.h CNF_HAVE_SYS_PARAM_H)
check_include_files(sys/stat.h CNF_HAVE_SYS_STAT_H)
check_include_files(sys/time.h CNF_HAVE_SYS_TIME_H)
check_include_files(sys/timeb.h CNF_HAVE_SYS_TIMEB_H)
check_include_files(sys/types.h CNF_HAVE_SYS_TYPES_H)

if(MSVC)
  check_include_files(io.h CNF_HAVE_IO_H)
  check_include_files(process.h CNF_HAVE_PROCESS_H)
  check_include_files(winsock2.h CNF_HAVE_WINSOCK2_H)
  check_include_files(ws2tcpip.h CNF_HAVE_WS2TCPIP_H)
  check_include_files(wspiapi.h CNF_HAVE_WSPIAPI_H)
  check_function_exists("_getpid" CNF_HAVE__GETPID)
else()
  check_include_files(sys/unistd.h CNF_HAVE_SYS_UNISTD_H)
  check_include_files(unistd.h CNF_HAVE_UNISTD_H)
endif()

check_include_files(arpa/inet.h CNF_HAVE_ARPA_INET_H)
check_include_files(netdb.h CNF_HAVE_NETDB_H)
check_include_files(netinet/in.h CNF_HAVE_NETINET_IN_H)
check_include_files(poll.h CNF_HAVE_POLL_H)
check_include_files(pthread.h CNF_HAVE_PTHREAD_H)
check_include_files(sys/select.h CNF_HAVE_SYS_SELECT_H)
check_include_files(sys/socket.h CNF_HAVE_SYS_SOCKET_H)
set(HAVE_PTHREAD ${CNF_HAVE_PTHREAD_H})

check_function_exists("strcasecmp" CNF_HAVE_STRCASECMP)
check_function_exists("stricmp" CNF_HAVE_STRICMP)
check_function_exists("getpid" CNF_HAVE_GETPID)
set(HAVE_GETPID ${CNF_HAVE_GETPID})
set(HAVE_STRCASECMP ${CNF_HAVE_STRCASECMP})

if(NOT MSVC)
  check_function_exists("clock_gettime" HAVE_CLOCK_GETTIME)
  check_function_exists("getaddrinfo" HAVE_GETADDRINFO)
  check_function_exists("gethostbyaddr" HAVE_GETHOSTBYADDR)
  check_function_exists("gethostbyaddr_r" HAVE_GETHOSTBYADDR_R)
  check_function_exists("gethostbyname" HAVE_GETHOSTBYNAME)
  check_function_exists("gethostbyname2" HAVE_GETHOSTBYNAME2)
  check_function_exists("gethostbyname_r" HAVE_GETHOSTBYNAME_R)
  check_function_exists("gethostname" HAVE_GETHOSTNAME)
  check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
  check_function_exists("htonl" HAVE_HTONL)
  check_function_exists("inet_aton" HAVE_INET_ATON)
  check_function_exists("inet_ntoa" HAVE_INET_NTOA)
  check_function_exists("inet_ntop" HAVE_INET_NTOP)
  check_function_exists("inet_pton" HAVE_INET_PTON)
  check_function_exists("isdigit" HAVE_ISDIGIT)
  check_function_exists("localtime_r" HAVE_LOCALTIME_R)
  check_function_exists("malloc" HAVE_MALLOC)
  check_function_exists("memset" HAVE_MEMSET)
  check_function_exists("poll" HAVE_POLL)
  check_function_exists("realloc" HAVE_REALLOC)
  check_function_exists("select" HAVE_SELECT)
  check_function_exists("socket" HAVE_SOCKET)
  check_function_exists("strchr" HAVE_STRCHR)
  check_function_exists("strerror" HAVE_STRERROR)
  check_function_exists("strstr" HAVE_STRSTR)
else()
  set(HAVE_GETADDRINFO YES)
  set(HAVE_HTONL YES)
  set(HAVE_INET_NTOP YES)
  set(HAVE_INET_PTON YES)
  set(HAVE_MALLOC YES)
  set(HAVE_MEMSET YES)
  set(HAVE_REALLOC YES)
  set(HAVE_SELECT YES)
  set(HAVE_STRCHR YES)
  set(HAVE_STRERROR YES)
  set(HAVE_STRSTR YES)
endif()

check_type_size("pthread_t" SIZEOF_PTHREAD_T LANGUAGE CXX)
check_type_size("long" SIZEOF_LONG LANGUAGE CXX)
check_type_size("int" SIZEOF_INT LANGUAGE CXX)

if(SIZEOF_PTHREAD_T EQUAL SIZEOF_LONG)
  set(AGENTPP_OPAQUE_PTHREAD_T "long")
else()
  set(AGENTPP_OPAQUE_PTHREAD_T "int")
endif()
message(STATUS "Found AGENTPP_OPAQUE_PTHREAD_T: ${AGENTPP_OPAQUE_PTHREAD_T}")

set(SOCKADDR_HEADERS)
if(CNF_HAVE_WINSOCK2_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} winsock2.h)
endif()
if(CNF_HAVE_WS2TCPIP_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} ws2tcpip.h)
endif()
if(CNF_HAVE_NETDB_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} netdb.h)
endif()
if(CNF_HAVE_SYS_SOCKET_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} sys/socket.h)
endif()
if(CNF_HAVE_ARPA_INET_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} arpa/inet.h)
endif()
if(CNF_HAVE_NETINET_IN_H)
  set(SOCKADDR_HEADERS ${SOCKADDR_HEADERS} netinet/in.h)
endif()

check_struct_has_member(
  "struct sockaddr_storage" ss_family "${SOCKADDR_HEADERS}" CNF_HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY LANGUAGE CXX
)

check_type_size("bool" SIZEOF_BOOL LANGUAGE CXX)
if(SIZEOF_BOOL)
  set(NEED_DEFINE_BOOL 0)
else()
  message(FATAL_ERROR "NO BOOL?")
endif()

if(AGENT_PP_PROXY)
  set(WITH_PROXY 1)
  if(AGENT_PP_PROXY_FORWARDER)
    set(WITH_PROXY_FORWARDER 1)
  else()
    set(WITH_PROXY_FORWARDER 0)
  endif()
else()
  set(WITH_PROXY 0)
  set(WITH_PROXY_FORWARDER 0)
endif()

if(AGENT_PP_NAMESPACE)
  set(WITH_NAMESPACE 1)
else()
  message(FATAL_ERROR "NO NAMESPACE?")
  set(WITH_NAMESPACE 0)
endif()

if(CNF_STDCXX_98_HEADERS)
  set(STDCXX_98_HEADERS 1)
else()
  message(FATAL_ERROR "NO STD C++?")
endif()

check_include_files("sys/mount.h" HAVE_SYS_MOUNT_H)

if(CNF_HAVE_PTHREAD_H)
  set(CMAKE_REQUIRED_LINK_AGENT_PPS "-lpthread")
  check_function_exists(pthread_mutex_timedlock HAVE_PTHREAD_MUTEX_TIMEDLOCK)
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/cpm-cmake/CPM.cmake for more info

include(CPM)

set(SNMP_PP_VERSION 3.4.7)
option(SNMP_PP_LOGGING "build all libaries with logging" NO)
# see https://github.com/ClausKlein/snmp_pp/releases/tag/v3.4.7.13.beta
CPMAddPackage(
  NAME snmp_pp
  GIT_TAG 4e53d61fdcdee7ff4781e9c256c5e38ddcad0dd5 # VERSION ${SNMP_PP_VERSION}.13
  GITHUB_REPOSITORY ClausKlein/snmp_pp
  OPTIONS "SNMP_PP_EXAMPLES YES" "CMAKE_SKIP_INSTALL_RULES NO"
)

CPMAddPackage("gh:aminya/project_options@0.26.3")
list(APPEND CMAKE_MODULE_PATH ${ProjectOptions_SRC_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_libagent.h.in libagent.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/agent_pp/agent++.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/agent_pp/agent++.h
)

set(MY_HEADER_FILES
    include/agent_pp/agent++.h
    include/agent_pp/agentpp_config_mib.h
    include/agent_pp/agentpp_simulation_mib.h
    include/agent_pp/avl_map.h
    include/agent_pp/entry.h
    include/agent_pp/List.h
    include/agent_pp/map.h
    include/agent_pp/mib_avl_map.h
    include/agent_pp/mib_complex_entry.h
    include/agent_pp/mib_context.h
    include/agent_pp/mib_entry.h
    include/agent_pp/mib.h
    include/agent_pp/mib_map.h
    include/agent_pp/mib_policy.h
    include/agent_pp/mib_proxy.h
    include/agent_pp/notification_log_mib.h
    include/agent_pp/notification_originator.h
    include/agent_pp/oidx_defs.h
    include/agent_pp/oidx_ptr.h
    include/agent_pp/proxy_forwarder.h
    include/agent_pp/request.h
    include/agent_pp/sim_mib.h
    include/agent_pp/snmp_community_mib.h
    include/agent_pp/snmp_counters.h
    include/agent_pp/snmp_group.h
    include/agent_pp/snmp_notification_mib.h
    include/agent_pp/snmp_pp_ext.h
    include/agent_pp/snmp_proxy_mib.h
    include/agent_pp/snmp_request.h
    include/agent_pp/snmp_target_mib.h
    include/agent_pp/snmp_textual_conventions.h
    include/agent_pp/system_group.h
    include/agent_pp/threads.h
    include/agent_pp/tools.h
    include/agent_pp/v3_mib.h
    include/agent_pp/vacm.h
    include/agent_pp/WIN32DLL.h
)

set(MY_HEADER_LIB_FILES ${CMAKE_CURRENT_BINARY_DIR}/libagent.h)

set(MY_SRC_FILES
    src/agentpp_config_mib.cpp
    src/agentpp_simulation_mib.cpp
    src/avl_map.cpp
    src/map.cpp
    src/mib_avl_map.cpp
    src/mib_complex_entry.cpp
    src/mib_context.cpp
    src/mib.cpp
    src/mib_entry.cpp
    src/mib_map.cpp
    src/mib_policy.cpp
    src/mib_proxy.cpp
    src/notification_log_mib.cpp
    src/notification_originator.cpp
    src/proxy_forwarder.cpp
    src/request.cpp
    src/sim_mib.cpp
    src/snmp_community_mib.cpp
    src/snmp_counters.cpp
    src/snmp_group.cpp
    src/snmp_notification_mib.cpp
    src/snmp_pp_ext.cpp
    src/snmp_proxy_mib.cpp
    src/snmp_request.cpp
    src/snmp_target_mib.cpp
    src/snmp_textual_conventions.cpp
    src/system_group.cpp
    src/threads.cpp
    src/tools.cpp
    src/v3_mib.cpp
    src/vacm.cpp
)

project_options(
  PREFIX
  ${PROJECT_NAME}
  ENABLE_CACHE
  ${ENABLE_COVERAGE}
  ${OPTION_ENABLED_SANITIZER}
  #XXX ENABLE_SANITIZER_ADDRESS
  #XXX ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
  #XXX ENABLE_SANITIZER_THREAD
  #XXX ENABLE_SANITIZER_MEMORY
  # TODO: WARNINGS_AS_ERRORS
)

set(PROJECT_EXPORT_HEADER ${PROJECT_NAME}/export.h)
add_library(${PROJECT_NAME} ${MY_HEADER_FILES} ${MY_HEADER_LIB_FILES} ${CMAKE_CURRENT_BINARY_DIR}/config.h ${MY_SRC_FILES})

if(CMAKE_SKIP_INSTALL_RULES)
  include(GenerateExportHeader)
  generate_export_header(
    ${PROJECT_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/PackageProjectInclude/${PROJECT_EXPORT_HEADER}
  )
  add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_project_options ${PROJECT_NAME}_project_warnings)

  # target_link_libraries(
  #   ${PROJECT_NAME} PRIVATE $<BUILD_LOCAL_INTERFACE:${PROJECT_NAME}_project_options>
  #                           $<BUILD_LOCAL_INTERFACE:${PROJECT_NAME}_project_warnings>
  # )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${OPTION_ENABLE_UNITY})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W3 /wd4251)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(
    ${PROJECT_NAME} PUBLIC -Wno-conversion -Wshadow -Wno-unused-parameter -Wno-deprecated-declarations -Wno-old-style-cast
                           -Wno-extra-semi
  )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(
  ${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
                             SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
target_link_libraries(${PROJECT_NAME} PUBLIC snmp_pp::snmp_pp)
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_CONFIG_H)

#
# NOTE: we have to prevent the use of older installed project headers
#
target_include_directories(
  ${PROJECT_NAME} SYSTEM BEFORE PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/PackageProjectInclude>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
)

# Note: for SameMajorVersion upgrade stategy
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             VISIBILITY_INLINES_HIDDEN ${BUILD_SHARED_LIBS}
             CMAKE_VISIBILITY_INLINES_HIDDEN ${BUILD_SHARED_LIBS}
)

macro(subdirlist result curdir)
  # message(STATUS " # Get subdirectories for ${curdir} ")
  file(
    GLOB children
    RELATIVE ${curdir}
    ${curdir}/*
  )
  set(dirlist "")
  foreach(child ${children})
    # message(STATUS " ## check ${child}")
    if(IS_DIRECTORY ${curdir}/${child})
      # message(STATUS " ### Add ${child}")
      if(NOT "${child}" STREQUAL "multi_agent")
        list(APPEND dirlist ${child})
      endif()
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

set(EXE_BASE_DIRS)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)

  if(AGENT_PP_EXAMPLES)
    list(APPEND EXE_BASE_DIRS examples)
  endif()
endif()

if(AGENT_PP_TOOLS)
  list(APPEND EXE_BASE_DIRS tools)
endif()

if(AGENT_PP_AGENPRO OR AGENT_PP_AGENPRO_SIMAGENT)
  list(APPEND EXE_BASE_DIRS agenpro)
endif()

foreach(EXE_BASE_DIR ${EXE_BASE_DIRS})
  message(STATUS " # Process dir ${EXE_BASE_DIR}")
  subdirlist(EXE_SUB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR})

  foreach(EXE_SUB_DIR ${EXE_SUB_DIRS})
    message(STATUS " ## Process executable in ${EXE_SUB_DIR}")

    if(AGENT_PP_PROXY_FORWARDER
       AND (${EXE_SUB_DIR} STREQUAL "proxy")
       AND (${EXE_BASE_DIR} STREQUAL "examples")
    )
      message(STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, as AGENT_PP_PROXY_FORWARDER is enabled")
      continue()
    endif()

    if(${EXE_BASE_DIR} STREQUAL "agenpro")
      if((${EXE_SUB_DIR} STREQUAL "agent") AND NOT AGENT_PP_AGENPRO)
        message(STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, as AGENT_PP_AGENPRO is disabled")
        continue()
      endif()
      if((${EXE_SUB_DIR} STREQUAL "simagent") AND NOT AGENT_PP_AGENPRO_SIMAGENT)
        message(STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, AGENT_PP_AGENPRO_SIMAGENT is disabled")
        continue()
      endif()
    endif()

    file(GLOB EXE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/include/*.h"
         "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/src/*.cpp"
    )

    # NOTE: all examples are named agent! CK
    add_executable(${EXE_SUB_DIR} ${EXE_SRC})
    target_link_libraries(${EXE_SUB_DIR} ${PROJECT_NAME})
    target_include_directories(${EXE_SUB_DIR} SYSTEM BEFORE PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(
      ${EXE_SUB_DIR} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/include/"
    )

    if(NOT ${EXE_BASE_DIR} STREQUAL "tools")
      set_target_properties(${EXE_SUB_DIR} PROPERTIES OUTPUT_NAME "${EXE_SUB_DIR}")
      add_test(
        NAME ${EXE_SUB_DIR}-test
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/simple-snmp-test.sh $<TARGET_FILE:${EXE_SUB_DIR}> $<TARGET_FILE:test_app>
        WORKING_DIRECTORY $<TARGET_FILE_DIR:test_app>
      )
    endif()

  endforeach()
endforeach()

if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

install(FILES ${MY_HEADER_LIB_FILES} DESTINATION include)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.10.0")
packageProject(
  # the name of the target to export
  NAME ${PROJECT_NAME}
  # the version of the target to export
  VERSION ${PROJECT_VERSION}
  # a temporary directory to create the config files
  BINARY_DIR ${PROJECT_BINARY_DIR}
  # location of the target's public headers
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
  # should match the target's INSTALL_INTERFACE include directory
  INCLUDE_DESTINATION include
  # (optional) option to install only header files with matching pattern
  INCLUDE_HEADER_PATTERN "*.h"
  # semicolon separated list of the project's dependencies
  DEPENDENCIES "snmp_pp ${SNMP_PP_VERSION}"
  # (optional) create a header containing the version info
  # Note: that the path to headers should be lowercase
  VERSION_HEADER "${PROJECT_NAME}/version.h"
  # (optional) create a export header using GenerateExportHeader module
  EXPORT_HEADER "${PROJECT_EXPORT_HEADER}"
  # (optional) install your library with a namespace (Note: do NOT add extra '::')
  NAMESPACE ${PROJECT_NAME}
  # (optional) define the project's version compatibility, defaults to `AnyNewerVersion`
  # supported values: `AnyNewerVersion|SameMajorVersion|SameMinorVersion|ExactVersion`
  COMPATIBILITY SameMajorVersion
  # (optional) option to disable the versioning of install destinations
  DISABLE_VERSION_SUFFIX YES
  # (optional) option to ignore target architecture for package resolution
  # defaults to YES for header only (i.e. INTERFACE) libraries
  ## ARCH_INDEPENDENT YES
)
