cmake_minimum_required( VERSION 3.2 )

include(libs/ShoreSDK/shore.cmake)

find_package(Boost REQUIRED)
find_library(LIB_JPEG jpeg)
if (NOT LIB_JPEG)
    message(FATAL_ERROR "Needed library jpeg not found!")
endif()

set (MAIN_TARGET helloSHORE )
set (MAIN_TARGET_SRC
    main.cpp
    $<TARGET_PROPERTY:shore,SHORE_MODEL_SOURCE>
    $<TARGET_PROPERTY:shore,SHORE_CREATE_FACE_ENGINE_CPP>
)

add_executable(${MAIN_TARGET} ${MAIN_TARGET_SRC})
# if shore_stub target exists, we must link againts it
if (TARGET shore_stub)
    set (MAIN_TARGET_DEPENDENCIES shore_stub)
else ()
    set (MAIN_TARGET_DEPENDENCIES shore)
endif()
set (MAIN_TARGET_DEPENDENCIES ${MAIN_TARGET_DEPENDENCIES} ${LIB_JPEG} )

target_link_libraries(${MAIN_TARGET} PRIVATE ${MAIN_TARGET_DEPENDENCIES})
# we need only header only boost lib
target_include_directories(${MAIN_TARGET} PRIVATE ${BOOST_INCLUDE_DIRS})
# we want to automatically register all shore models
target_compile_definitions(${MAIN_TARGET}
    PRIVATE
        SHORE_REGISTER_MODELS
        SHORE_VERSION=$<TARGET_PROPERTY:shore,SHORE_VERSION> 
)
set_target_properties(${MAIN_TARGET}
    PROPERTIES
        CXX_STANDARD 11
        INSTALL_RPATH "."
)

###########################################################################
set (INSTALL_ROOT "${CMAKE_BINARY_DIR}/INSTALL")

install (TARGETS ${MAIN_TARGET}
         RUNTIME DESTINATION ${INSTALL_ROOT}
         LIBRARY DESTINATION ${INSTALL_ROOT}
         ARCHIVE DESTINATION ${INSTALL_ROOT}
)
get_target_property(SHORE_LIB shore IMPORTED_LOCATION)
install(FILES
        ${SHORE_LIB} DESTINATION ${INSTALL_ROOT}
)