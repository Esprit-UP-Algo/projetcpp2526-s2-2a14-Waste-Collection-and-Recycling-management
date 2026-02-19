# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Tuniwaste_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Tuniwaste_autogen.dir\\ParseCache.txt"
  "Tuniwaste_autogen"
  )
endif()
