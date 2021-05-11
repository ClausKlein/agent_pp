cmake_minimum_required (VERSION 3.15...3.20)

project (agent_pp VERSION 4.5.0 LANGUAGES CXX)

include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckSymbolExists)
include (CheckLibraryExists)
include (CheckStructHasMember)
include (CheckTypeSize)

set (CMAKE_CXX_STANDARD 14)

list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

option (OPTION_NAMESPACE  "Use Namespaces" ON)
option (OPTION_PROXY  "Enable Proxy" ON)
option (OPTION_PROXY_FORWARDER  "Enable Proxy Forwarder" ON)
option (OPTION_AGENPRO  "build with AgenPro templates (usually disabled)" OFF)
option (OPTION_AGENPRO_SIMAGENT  "build with AgenPro simagent instead regular templates (usually disabled)" OFF)
option (OPTION_EXAMPLES  "build examples" ON)
option (OPTION_TOOLS  "build tools" ON)

set (VERSION ${PROJECT_VERSION})
set (AGENT_PP_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set (AGENT_PP_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set (AGENT_PP_MICRO_VERSION ${PROJECT_VERSION_PATCH})

message (STATUS "agent++ Version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

check_include_files (sys/types.h CNF_HAVE_SYS_TYPES_H)
check_include_files (sys/stat.h CNF_HAVE_SYS_STAT_H)
check_include_files ("cctype;cerrno;climits;csignal;cstddef;cstdio;cstdlib;cstring;ctime" CNF_STDCXX_98_HEADERS)
check_include_files ("iostream" CNF_HAVE_IOSTREAM)
check_include_files ("stdlib.h;stddef.h" CNF_STDC_HEADERS)
check_include_files (stdlib.h CNF_HAVE_STDLIB_H)
check_include_files (string.h CNF_HAVE_STRING_H)
check_include_files (memory.h CNF_HAVE_MEMORY_H)
check_include_files (strings.h CNF_HAVE_STRINGS_H)
check_include_files (inttypes.h CNF_HAVE_INTTYPES_H)
check_include_files (ctype.h CNF_HAVE_CTYPE_H)
check_include_files (ctype.h HAVE_CTYPE_H)
check_include_files (signal.h CNF_HAVE_SIGNAL_H)
check_include_files (errno.h CNF_HAVE_ERRNO_H)
check_include_files (time.h CNF_HAVE_TIME_H)
check_include_files (unistd.h CNF_HAVE_UNISTD_H)
check_include_files (sys/unistd.h CNF_HAVE_SYS_UNISTD_H)
check_include_files (stdint.h CNF_HAVE_STDINT_H)
check_include_files (sys/time.h CNF_HAVE_SYS_TIME_H)
check_include_files (sys/param.h CNF_HAVE_SYS_PARAM_H)
check_include_files (sys/timeb.h CNF_HAVE_SYS_TIMEB_H)
check_include_files (winsock2.h CNF_HAVE_WINSOCK2_H)
check_include_files (ws2tcpip.h CNF_HAVE_WS2TCPIP_H)
check_include_files (wspiapi.h CNF_HAVE_WSPIAPI_H)
check_include_files (winsock.h CNF_HAVE_WINSOCK_H)
check_include_files (netdb.h CNF_HAVE_NETDB_H)
check_include_files (sys/socket.h CNF_HAVE_SYS_SOCKET_H)
check_include_files (arpa/inet.h CNF_HAVE_ARPA_INET_H)
check_include_files (arpa/inet.h HAVE_ARPA_INET_H)
check_include_files (netinet/in.h CNF_HAVE_NETINET_IN_H)
check_include_files (poll.h CNF_HAVE_POLL_H)
check_include_files (sys/select.h CNF_HAVE_SYS_SELECT_H)
check_include_files (io.h CNF_HAVE_IO_H)
check_include_files (process.h CNF_HAVE_PROCESS_H)
check_include_files (pthread.h CNF_HAVE_PTHREAD_H)
check_function_exists ("strcasecmp" CNF_HAVE_STRCASECMP)
check_function_exists ("stricmp" CNF_HAVE_STRICMP)
check_function_exists ("getpid" CNF_HAVE_GETPID)
check_function_exists ("_getpid" CNF_HAVE__GETPID)

check_function_exists ("clock_gettime" HAVE_CLOCK_GETTIME)
check_function_exists ("getaddrinfo" HAVE_GETADDRINFO)
check_function_exists ("gethostbyaddr" HAVE_GETHOSTBYADDR)
check_function_exists ("gethostbyaddr_r" HAVE_GETHOSTBYADDR_R)
check_function_exists ("gethostbyname" HAVE_GETHOSTBYNAME)
check_function_exists ("gethostbyname2" HAVE_GETHOSTBYNAME2)
check_function_exists ("gethostbyname_r" HAVE_GETHOSTBYNAME_R)
check_function_exists ("gethostname" HAVE_GETHOSTNAME)
check_function_exists ("getpid" HAVE_GETPID)
check_function_exists ("gettimeofday" HAVE_GETTIMEOFDAY)
check_function_exists ("htonl" HAVE_HTONL)
check_function_exists ("inet_aton" HAVE_INET_ATON)
check_function_exists ("inet_ntoa" HAVE_INET_NTOA)
check_function_exists ("inet_ntop" HAVE_INET_NTOP)
check_function_exists ("inet_pton" HAVE_INET_PTON)
check_include_files (inttypes.h HAVE_INTTYPES_H)
check_include_files (io.h HAVE_IO_H)
check_function_exists ("isdigit" HAVE_ISDIGIT)
check_include_files (limits.h HAVE_LIMITS_H)
check_function_exists ("localtime_r" HAVE_LOCALTIME_R)
check_function_exists ("malloc" HAVE_MALLOC)
check_include_files (memory.h HAVE_MEMORY_H)
check_function_exists ("memset" HAVE_MEMSET)
check_include_files (netdb.h HAVE_NETDB_H)
check_include_files (netinet/in.h HAVE_NETINET_IN_H)
check_function_exists ("poll" HAVE_POLL)
check_include_files (poll.h HAVE_POLL_H)
check_include_files (process.h HAVE_PROCESS_H)
check_include_files (pthread.h HAVE_PTHREAD) # :-(
check_function_exists ("realloc" HAVE_REALLOC)
check_function_exists ("select" HAVE_SELECT)
check_include_files (signal.h HAVE_SIGNAL_H)
check_function_exists ("socket" HAVE_SOCKET)
check_include_files (stdint.h HAVE_STDINT_H)
check_include_files (stdio.h HAVE_STDIO_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_function_exists ("strcasecmp" HAVE_STRCASECMP)
check_function_exists ("strchr" HAVE_STRCHR)
check_function_exists ("strerror" HAVE_STRERROR)
check_function_exists ("stricmp" HAVE_STRICMP)
check_include_files (strings.h HAVE_STRINGS_H)
check_include_files (string.h HAVE_STRING_H)
check_function_exists ("strstr" HAVE_STRSTR)
check_include_files (sys/param.h HAVE_SYS_PARAM_H)
check_include_files (sys/select.h HAVE_SYS_SELECT_H)
check_include_files (sys/socket.h HAVE_SYS_SOCKET_H)
check_include_files (sys/stat.h HAVE_SYS_STAT_H)
check_include_files (sys/timeb.h HAVE_SYS_TIMEB_H)
check_include_files (sys/time.h HAVE_SYS_TIME_H)
check_include_files (sys/types.h HAVE_SYS_TYPES_H)
check_include_files (sys/unistd.h HAVE_SYS_UNISTD_H)
check_include_files (time.h HAVE_TIME_H)
check_include_files (unistd.h HAVE_UNISTD_H)
check_include_files (winsock2.h HAVE_WINSOCK2_H)
check_include_files (winsock.h HAVE_WINSOCK_H)
check_include_files (ws2tcpip.h HAVE_WS2TCPIP_H)
check_include_files (wspiapi.h HAVE_WSPIAPI_H)
check_function_exists ("_getpid" HAVE__GETPID)

check_type_size ("pthread_t" SIZEOF_PTHREAD_T LANGUAGE CXX)
check_type_size ("long" SIZEOF_LONG LANGUAGE CXX)
check_type_size ("int" SIZEOF_INT LANGUAGE CXX)

if (SIZEOF_PTHREAD_T EQUAL SIZEOF_LONG)
  set (AGENTPP_OPAQUE_PTHREAD_T "long")
elseif ()
  set (AGENTPP_OPAQUE_PTHREAD_T "long")
else ()
  message (FATAL_ERROR "Can't map pthread_t to int or long")
endif ()
message (STATUS "Found AGENTPP_OPAQUE_PTHREAD_T: ${AGENTPP_OPAQUE_PTHREAD_T}")


set (SOCKADDR_HEADERS)
if (CNF_HAVE_WINSOCK2_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} winsock2.h)
endif ()
if (CNF_HAVE_WS2TCPIP_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} ws2tcpip.h)
endif ()
if (CNF_HAVE_WINSOCK_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} winsock.h)
endif ()
if (CNF_HAVE_NETDB_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} netdb.h)
endif ()
if (CNF_HAVE_SYS_SOCKET_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} sys/socket.h)
endif ()
if (CNF_HAVE_ARPA_INET_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} arpa/inet.h)
endif ()
if (CNF_HAVE_NETINET_IN_H)
   set (SOCKADDR_HEADERS ${SOCKADDR_HEADERS} netinet/in.h)
endif ()

check_struct_has_member ("struct sockaddr_storage"
            ss_family "${SOCKADDR_HEADERS}"
            CNF_HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY LANGUAGE CXX)

check_struct_has_member ("struct sockaddr_storage"
            __ss_family "${SOCKADDR_HEADERS}"
            CNF_HAVE_STRUCT_SOCKADDR_STORAGE___SS_FAMILY LANGUAGE CXX)

check_type_size ("bool" SIZEOF_BOOL LANGUAGE CXX)

if (SIZEOF_BOOL)
  message (STATUS "HAVE_BOOL")
  set (NEED_DEFINE_BOOL 0)
else ()
  message (STATUS "NO HAVE_BOOL")
  set (NEED_DEFINE_BOOL 1)
endif ()

if (OPTION_PROXY)
  set (WITH_PROXY 1)
else ()
  set (WITH_PROXY 0)
endif ()

if (OPTION_PROXY_FORWARDER)
  set (WITH_PROXY_FORWARDER 1)
else ()
  set (WITH_PROXY_FORWARDER 0)
endif ()

if (OPTION_NAMESPACE)
  set (WITH_NAMESPACE 1)
else ()
  set (WITH_NAMESPACE 0)
endif ()


if (CNF_STDC_HEADERS)
  set (STDC_HEADERS 1)
else ()
  set (STDC_HEADERS 0)
endif ()

if (CNF_STDCXX_98_HEADERS)
  set (STDCXX_98_HEADERS 1)
else ()
  set (STDCXX_98_HEADERS 0)
endif ()

check_include_files (malloc.h HAVE_MALLOC_H)
check_include_files ("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
check_function_exists ("strcasecmp" HAVE_STRCASECMP)

set (CMAKE_REQUIRED_LINK_OPTIONS "-lpthread")
check_function_exists (pthread_mutex_timedlock HAVE_PTHREAD_MUTEX_TIMEDLOCK)


find_package (snmp_pp)
if (NOT SNMP_PP_FOUND)
  message (FATAL_ERROR " Lib snmp++ NOT found, try to call cmake with '-D SNMP_PP_ROOT_DIR=/install/path')")
endif ()


configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in config.h)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_libagent.h.in libagent.h)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/include/agent_pp/agent++.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/agent_pp/agent++.h)


if (MSVC)
    # warning level 4 (all warnings as errors: /WX)
    add_compile_options (/W4 /WX)
else ()
    add_compile_options (-Wall)
    # TODO: lots of warnings (more: -Wextra -Wpedantic)
endif ()


set (MY_HEADER_FILES
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

set (MY_HEADER_LIB_FILES
  ${CMAKE_BINARY_DIR}/libagent.h
  )

set (MY_SRC_FILES
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

add_library (agent++ SHARED
  ${MY_HEADER_FILES}
  ${MY_HEADER_LIB_FILES}
  ${CMAKE_BINARY_DIR}/config.h
  ${MY_SRC_FILES}
)

set_property (TARGET agent++ PROPERTY POSITION_INDEPENDENT_CODE 1)
# Set .so version as autoconf does: agent++ version 1.2.3 will go to .so.12.0.3!
set_target_properties (agent++ PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}.0.${PROJECT_VERSION_PATCH} SOVERSION ${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR})
set_target_properties (agent++ PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
target_link_libraries (agent++ PUBLIC ${SNMP_PP_LIBRARIES})
# NOT installed! CK target_compile_definitions (agent++ PUBLIC -DHAVE_CONFIG_H)
target_include_directories (agent++ SYSTEM BEFORE PUBLIC ${CMAKE_BINARY_DIR} . include ${SNMP_PP_INCLUDE_DIR})


macro (subdirlist result curdir)
  #message (STATUS " # Get subdirectories for ${curdir} ")
  file (GLOB children RELATIVE ${curdir} ${curdir}/*)
  set (dirlist "")
  foreach (child ${children})
    #message (STATUS " ## check ${child}")
    if (IS_DIRECTORY ${curdir}/${child})
      #message (STATUS " ### Add ${child}")
      list (APPEND dirlist ${child})
    endif ()
  endforeach ()
  set (${result} ${dirlist})
endmacro ()

set (EXE_BASE_DIRS )

if (OPTION_EXAMPLES)
  list (APPEND EXE_BASE_DIRS examples)
endif ()

if (OPTION_TOOLS)
  list (APPEND EXE_BASE_DIRS tools)
endif ()

if (OPTION_AGENPRO OR OPTION_AGENPRO_SIMAGENT)
  list (APPEND EXE_BASE_DIRS agenpro)
endif ()

foreach (EXE_BASE_DIR ${EXE_BASE_DIRS})
  message (STATUS " # Process dir ${EXE_BASE_DIR}")
  subdirlist (EXE_SUB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR})
  foreach (EXE_SUB_DIR ${EXE_SUB_DIRS})
    message (STATUS " ## Process executable in ${EXE_SUB_DIR}")

    if (OPTION_PROXY_FORWARDER AND
       (${EXE_SUB_DIR} STREQUAL "proxy") AND
       (${EXE_BASE_DIR} STREQUAL "examples"))
      message (STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, as OPTION_PROXY_FORWARDER is enabled")
      continue ()
    endif ()

    if (${EXE_BASE_DIR} STREQUAL "agenpro")
      if ((${EXE_SUB_DIR} STREQUAL "agent") AND
         NOT OPTION_AGENPRO)
        message (STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, as OPTION_AGENPRO is disabled")
        continue ()
      endif ()
      if ((${EXE_SUB_DIR} STREQUAL "simagent") AND
         NOT OPTION_AGENPRO_SIMAGENT)
        message (STATUS " ## Do not build ${EXE_BASE_DIR}/${EXE_SUB_DIR}, OPTION_AGENPRO_SIMAGENT is disabled")
        continue ()
      endif ()
    endif ()

    file (GLOB EXE_SRC
      "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/include/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/src/*.cpp"
      )

    add_executable (${EXE_SUB_DIR} ${EXE_SRC})
    target_link_libraries (${EXE_SUB_DIR} agent++ ${SNMP_PP_LIBRARIES})
    target_include_directories (${EXE_SUB_DIR} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/include/")
    set_target_properties (${EXE_SUB_DIR} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${EXE_BASE_DIR}/${EXE_SUB_DIR}/src")
    if (NOT ${EXE_BASE_DIR} STREQUAL "tools")
      set_target_properties (${EXE_SUB_DIR} PROPERTIES OUTPUT_NAME "agent")
    endif ()

  endforeach ()
endforeach ()

add_library (agent++_static STATIC
  ${MY_HEADER_FILES}
  ${MY_HEADER_LIB_FILES}
  ${CMAKE_BINARY_DIR}/config.h
  ${MY_SRC_FILES}
)
target_link_libraries (agent++_static PUBLIC ${SNMP_PP_LIBRARIES})
set_target_properties (agent++_static PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
# NOT installed! CK target_compile_definitions (agent++_static PUBLIC -DHAVE_CONFIG_H)
target_include_directories (agent++_static SYSTEM BEFORE PRIVATE ${CMAKE_BINARY_DIR} . include ${SNMP_PP_INCLUDE_DIR})

install (TARGETS agent++ DESTINATION lib)
install (TARGETS agent++_static DESTINATION lib)
install (FILES ${MY_HEADER_LIB_FILES} DESTINATION include)
install (FILES ${MY_HEADER_FILES} DESTINATION include/agent_pp)

