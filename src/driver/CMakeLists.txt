find_package(Threads REQUIRED)

add_library(driver STATIC
        # interface
        src/async_port/async_serial.cpp
        src/async_port/async_can.cpp
        # imu sensor
        src/imu_driver/ch_serial.c
        src/imu_driver/imu_hipnuc.cpp
        # vesc
        src/vesc_driver/vesc_can_interface.cpp
        src/vesc_driver/vesc_status_packet.cpp
        src/vesc_driver/vesc_cmd_packet.cpp)
target_link_libraries(driver PUBLIC interface Threads::Threads)
target_compile_definitions(driver PUBLIC ASIO_ENABLE_OLD_SERVICES)
target_compile_options(driver PUBLIC -fPIC)
target_include_directories(driver PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()

## installation configuration
install(TARGETS driver
        EXPORT roboswTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
