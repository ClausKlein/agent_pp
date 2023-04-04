option(BUILD_TESTING "Enable testing support" ${PROJECT_IS_TOP_LEVEL})

if(NOT PROJECT_IS_TOP_LEVEL)
  option(CMAKE_SKIP_INSTALL_RULES "Whether to disable generation of installation rules" YES)
endif()


option(USE_OUTPUT_PATH "build all libaries and runtime files at the bin directory" YES)
if(USE_OUTPUT_PATH)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

option(BUILD_SHARED_LIBS "Global flag to cause add_library() to create shared libraries if on." YES)

if(APPLE)
  set(OPTION_ENABLED_SANITIZER
      "ENABLE_SANITIZER_ADDRESS; ENABLE_SANITIZER_UNDEFINED_BEHAVIOR"
      CACHE STRING "Enabled sanitizer for debug build"
  )
else()
  set(OPTION_ENABLED_SANITIZER
      "ENABLE_SANITIZER_MEMORY"
      CACHE STRING "Enabled sanitizer for debug build"
  )
endif()
option(OPTION_ENABLE_COVERAGE "Enable test coverage of projects" OFF)
if(OPTION_ENABLE_COVERAGE)
  set(ENABLE_COVERAGE "ENABLE_COVERAGE")
endif()

if(SNMP_PP_LOGGING)
  set(OPTION_ENABLE_UNITY
      OFF
      CACHE STRING "forced" FORCE
  )
else()
  option(OPTION_ENABLE_UNITY "Enable Unity builds of projects" ON)
endif()
