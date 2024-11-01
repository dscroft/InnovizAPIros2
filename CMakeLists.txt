cmake_minimum_required(VERSION 3.8)
project(innoviz_ros2)

set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DINVZ_API_IMPORT)
add_definitions(-DNDARRAY_IMPORT)

# set c++14
set(CMAKE_CXX_STANDARD 14)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  #add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

include_directories(include)

find_library(INVZ_API_LIB innovizApi PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/linux_x86)
    #target_link_libraries(${PROJECT_NAME} ${INVZ_API_LIB} stdc++fs pthread)


ament_auto_add_executable(device
  src/device.cpp)
target_link_libraries(device ${INVZ_API_LIB} stdc++fs pthread)

install(DIRECTORY 
  launch
  config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_package()
