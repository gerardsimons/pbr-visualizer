# set( component_SOURCES ... ) # Add the source-files for the component here
# Optionally you can use file glob (uncomment the next line)
file( GLOB rivdata_sources *.cpp )

add_library( rivdata ${rivdata_sources} )
target_link_libraries( rivdata helper )