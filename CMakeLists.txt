cmake_minimum_required( VERSION 3.7 )
project( Zaboard LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 17 )
# set( CMAKE_BUILD_TYPE Debug )

find_program(iwyu_path NAMES include-what-you-use iwyu)

if( MSVC )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
endif()

file( GLOB zaboard_files
            src/zaboard.cpp
            src/notemap.cpp
            src/instrument.cpp
            src/conductor.cpp
            # src/zbtrack.cpp
            # src/preprocessor/preprocessor.cpp
)
if( MSVC )
  file( GLOB zaboard_os_files src/windows/zkeyboard.cpp )
else()
  file( GLOB zaboard_os_files src/unix/zkeyboard.cpp )
endif()

add_executable( zaboard ${zaboard_files} ${zaboard_os_files} )

if( MSVC )
  file( GLOB zmldlls "C:/Program Files/ZaberMotionLibrary/bin/*" )
  file( COPY ${zmldlls} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release" )
  file( COPY ${zmldlls} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug" )

  file( COPY "NoteMap.csv" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release" )
  file( COPY "NoteMap.csv" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug" )

  target_include_directories( zaboard SYSTEM PRIVATE "C:\\Program Files\\ZaberMotionLibrary\\include" )

  add_library( winzml STATIC IMPORTED )
  set_target_properties( winzml PROPERTIES
                                IMPORTED_LOCATION_DEBUG "C:\\Program Files\\ZaberMotionLibrary\\lib\\zml.lib"
                                IMPORTED_LOCATION_RELEASE "C:\\Program Files\\ZaberMotionLibrary\\lib\\zml.lib"
  )

  target_link_libraries( zaboard winzml )
  target_compile_options( zaboard PRIVATE /W4 )  # /WX
else()
  file( COPY "NoteMap.csv" DESTINATION "${CMAKE_BINARY_DIR}" )

  target_link_libraries( zaboard zml pthread midifile )
  target_compile_options( zaboard PRIVATE -Wall -Wextra -pedantic )  # -Werror
endif()

if( iwyu_path )
  set_property( TARGET zaboard PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path} )
endif()