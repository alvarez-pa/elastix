# Example project for using elastix code from external projects.
project( elxExternalProject )

cmake_minimum_required( VERSION 3.10.2 )

find_package( Elastix REQUIRED )

# Use the version of ITK from Elastix.
set( ITK_DIR ${ELASTIX_ITK_DIR} )
find_package( ITK REQUIRED )

# Build a small example executable.
add_executable( elastix_translation_example ElastixTranslationExample.cxx )

target_include_directories( elastix_translation_example
  PRIVATE ${ELASTIX_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS} )

target_link_directories( elastix_translation_example
  PRIVATE ${ELASTIX_LIBRARY_DIRS} )

target_link_libraries( elastix_translation_example
  ${ITK_LIBRARIES}
  ${ELASTIX_LIB_DEPENDENCIES}
  ${CMAKE_STATIC_LIBRARY_PREFIX}ANNlib${ELASTIX_LIB_SUFFIX}
  ${CMAKE_STATIC_LIBRARY_PREFIX}KNNlib${ELASTIX_LIB_SUFFIX}
  ${ELASTIX_LIB_FILENAME}
)

# TODO Remve these two "Debug" messages:
message("elastix_translation_example CMAKE_STATIC_LIBRARY_PREFIX = ${CMAKE_STATIC_LIBRARY_PREFIX}.")
message("elastix_translation_example CMAKE_STATIC_LIBRARY_SUFFFIX = ${CMAKE_STATIC_LIBRARY_SUFFIX}.")