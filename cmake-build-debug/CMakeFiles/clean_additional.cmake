# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Oving_2_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Oving_2_autogen.dir/ParseCache.txt"
  "Oving_2_autogen"
  )
endif()
