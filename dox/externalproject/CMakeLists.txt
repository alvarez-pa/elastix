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
  "ANNlib${ELASTIX_LIB_SUFFIX}"
  "KNNlib${ELASTIX_LIB_SUFFIX}"
  ${ELASTIX_LIB_FILENAME}
)
