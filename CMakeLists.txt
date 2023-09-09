cmake_minimum_required(VERSION 2.8)

project(Godzilla)



#determine if it is in debug mode
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #determine the compilation architecture
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
        set(THIRD_PARTY_DIR ./third_party/x86-windows)
        set(THIRD_PATRY_STATIC_DIR ./third_party/x86-windows-static)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        set(THIRD_PARTY_DIR ./third_party/x64-windows)
        set(THIRD_PATRY_STATIC_DIR ./third_party/x64-windows-static)
    else ()
        message(FATAL_ERROR "unknown compilation architecture")
    endif ()

endif()

#the third-party library that needs to be imported.
set(THIRD_PATRY_LIST
    "capstone"
    "keystone"
)

set(INSTALL_FILE_LIST)    
#import third-party library.
foreach(LIB_NAME IN LISTS THIRD_PATRY_LIST)
    set(${LIB_NAME}_INCLUDE_DIRS ${THIRD_PARTY_DIR}/${LIB_NAME}/include)
    set(${LIB_NAME}_STATIC_INCLUDE_DIRS ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/include)
    set(${LIB_NAME}_STATIC_INCLUDE_DIRS ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/include)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(GLOB FILE_LIST ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/debug/bin/*)
        set(${LIB_NAME}_LIBRARIES ${THIRD_PARTY_DIR}/${LIB_NAME}/debug/lib)
        set(${LIB_NAME}_STATIC_LIBRARIES ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/debug/lib)
    else()
        set(${LIB_NAME}_LIBRARIES ${THIRD_PARTY_DIR}/${LIB_NAME}/lib)
        set(${LIB_NAME}_STATIC_LIBRARIES ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/lib)
        file(GLOB FILE_LIST ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/bin/*)
    endif()
    
    list(APPEND INSTALL_FILE_LIST ${FILE_LIST})
    set(${LIB_NAME}_FOUND true)
    message("importing library ${LIB_NAME}.")
endforeach()


include_directories(.)

aux_source_directory(./ GODZILLA_ROOT_DIR)
aux_source_directory(./gbase GODZILLA_BASE_DIR)
aux_source_directory(./memory GODZILLA_MEMORY_DIR)
aux_source_directory(./process GODZILLA_PROCESS_DIR)
aux_source_directory(./common GODZILLA_COMMON_DIR)
aux_source_directory(./file/dexfile GODZILLA_FILE_DEXFILE_DIR)

add_library(
    ${PROJECT_NAME}
    STATIC
    ${GODZILLA_ROOT_DIR}
    ${GODZILLA_BASE_DIR}
    ${GODZILLA_MEMORY_DIR}
    ${GODZILLA_PROCESS_DIR}
    ${GODZILLA_FILE_DEXFILE_DIR}
    ${GODZILLA_COMMON_DIR}
)

foreach(file IN LISTS INSTALL_FILE_LIST)
    install(COPY ${file} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()
