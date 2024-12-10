add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ArgParser)
target_link_libraries(${PROJECT_NAME} PRIVATE HamArchiver)
target_link_libraries(${PROJECT_NAME} PRIVATE runtime)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
