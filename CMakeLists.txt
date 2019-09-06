cmake_minimum_required( VERSION 3.7 )
project( Zaboard LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 11 )
# set( CMAKE_BUILD_TYPE Debug )

find_program(iwyu_path NAMES include-what-you-use iwyu)

file( GLOB zaboard_files src/*.cpp )
if( MSVC )
  file( GLOB zaboard_os_files src/windows/*.cpp )
else()
  file( GLOB zaboard_os_files src/unix/*.cpp )
endif()

add_executable( zaboard ${zaboard_files} ${zaboard_os_files} )

if( MSVC )
  target_link_libraries( zaboard zml )
  target_compile_options( zaboard PRIVATE /W4 /WX )
else()
  target_link_libraries( zaboard zml pthread )
  target_compile_options( zaboard PRIVATE -Wall -Wextra -pedantic )  # -Werror
endif()

if( iwyu_path )
  set_property( TARGET zaboard PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path} )
endif()