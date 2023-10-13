cmake_minimum_required(VERSION 2.8)

project(Godzilla)



#determine if it is in debug mode
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #determine the compilation architecture
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
        set(THIRD_PARTY_DIR ./third_party/x86-windows)
        set(THIRD_PARTY_STATIC_DIR ./third_party/x86-windows-static)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        set(THIRD_PARTY_DIR ./third_party/x64-windows)
        set(THIRD_PARTY_STATIC_DIR ./third_party/x64-windows-static)
    else ()
        message(FATAL_ERROR "unknown compilation architecture")
    endif ()

endif()

#the third-party library that needs to be imported.
set(THIRD_PATRY_LIST
    "capstone"
    "keystone"
)
  
#import third-party library.
foreach(LIB_NAME IN LISTS THIRD_PATRY_LIST)
    set(${LIB_NAME}_INCLUDE_DIRS "${THIRD_PARTY_DIR}/${LIB_NAME}/include")
    set(${LIB_NAME}_STATIC_INCLUDE_DIRS "${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/include")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(GLOB FILE_LIST ${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/debug/bin/*)
        set(${LIB_NAME}_LIBRARIES "${THIRD_PARTY_DIR}/${LIB_NAME}/debug/lib")
        set(${LIB_NAME}_STATIC_LIBRARIES "${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/debug/lib")
    else()
        set(${LIB_NAME}_LIBRARIES "${THIRD_PARTY_DIR}/${LIB_NAME}/lib")
        set(${LIB_NAME}_STATIC_LIBRARIES "${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/lib")
        file(GLOB FILE_LIST "${THIRD_PARTY_STATIC_DIR}/${LIB_NAME}/bin/*")
    endif()
    
    set(${LIB_NAME}_FOUND true)
    message("importing library ${LIB_NAME}.")
endforeach()

include_directories(.)
include_directories(${capstone_STATIC_INCLUDE_DIRS})
include_directories(${keystone_STATIC_INCLUDE_DIRS})

aux_source_directory(./ GODZILLA_ROOT_DIR)
aux_source_directory(./asm GODZILLA_ASM_DIR)
aux_source_directory(./gbase GODZILLA_BASE_DIR)
aux_source_directory(./memory GODZILLA_MEMORY_DIR)
aux_source_directory(./process GODZILLA_PROCESS_DIR)
aux_source_directory(./common GODZILLA_COMMON_DIR)
aux_source_directory(./system GODZILLA_SYSTEM_DIR)
aux_source_directory(./hook GODZILLA_HOOK_DIR)
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
    ${GODZILLA_SYSTEM_DIR}
    ${GODZILLA_ASM_DIR}
    ${GODZILLA_HOOK_DIR}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${capstone_STATIC_LIBRARIES}/capstone.lib
    ${capstone_STATIC_LIBRARIES}/keystone.lib
)

function(is_subdirectory SUBDIRECTORY PARENT_LIST RESULT_VAR)
    set(IS_SUBDIRECTORY FALSE)
    foreach(PARENT_DIR ${${PARENT_LIST}})
        if ("${SUBDIRECTORY}" MATCHES "^${PARENT_DIR}")
            set(IS_SUBDIRECTORY TRUE)
            break()
        endif()
    endforeach()
    set(${RESULT_VAR} ${IS_SUBDIRECTORY} PARENT_SCOPE)
endfunction()


set(
    EXCLUDE_DIRS 
    "${CMAKE_SOURCE_DIR}/out"
    "${CMAKE_SOURCE_DIR}/third_party"
)
file(GLOB_RECURSE header_files RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/*.h")
foreach(header ${header_files})
    is_subdirectory("${CMAKE_SOURCE_DIR}/${header}" EXCLUDE_DIRS is_exclude)
    if (NOT is_exclude)
        set(src "${CMAKE_SOURCE_DIR}/${header}")
        set(dst "${CMAKE_BINARY_DIR}/godzilla/${header}")
        configure_file("${src}" "${dst}" COPYONLY)
        message(STATUS "Copied ${src} to ${dst}")
    endif()
endforeach()
