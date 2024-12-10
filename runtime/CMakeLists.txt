add_library(runtime runtime.cpp)

target_link_libraries(runtime PRIVATE ArgParser)
target_link_libraries(runtime PRIVATE HamArchiver)

target_include_directories(runtime PUBLIC ${PROJECT_SOURCE_DIR})
