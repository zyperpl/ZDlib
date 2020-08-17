add_executable(demo main.cpp minimal_test.cpp network_test.cpp model_test.cpp file_test.cpp shader_test.cpp network_test.cpp image_test.cpp)
target_link_libraries(demo PRIVATE zd)

add_custom_target(run
  COMMAND pwd && ../build/test/demo
  DEPENDS demo
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
