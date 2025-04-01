cmake_minimum_required(VERSION 3.26)
project(KeepAliveUURemoteDesktop)

# 设置 MSVC 特性
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用 GNU 扩展

message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

# UTF-8 编译选项（仅对 GCC/Clang 生效）
if( MSVC)
    message("IS MSVC")
    # UTF-8 需要配置这个
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
elseif (NOT MSVC)
    message("NO ${MSVC}")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

##########################################################################################
##########################################################################################
##########################################################################################


# 手动指定静态库路径和头文件路径
set(CURL_STATIC ON)

# 手动设置 CURL_FOUND 和 CURL_STATIC_LIB_FOUND
set(CURL_FOUND TRUE)
set(CURL_STATIC_LIB_FOUND TRUE)


# 处理默认库冲突
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # 仅在 MSVC 工具链中添加 /NODEFAULTLIB:LIBCMT
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 确保 MinGW 或 Clang 工具链不包含 MSVC 特有选项
    message(STATUS "你正在使用 MinGW 工具链，如果静态库中存在 MSVC 工具链编译的，则无法继续构建，你要自己确保静态库是用 MinGW 编译的。")
endif()

##########################################################################################
##########################################################################################
##########################################################################################


# 递归查找所有的 .h 文件路径
file(GLOB_RECURSE HEADS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} LIST_DIRECTORIES false "constant/*.h" "include/*.h")
# 递归查找所有的 .cpp 文件路径
file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} LIST_DIRECTORIES false "constant/*.cpp" "src/*.cpp")
# 递归查找所有的 .lib 文件路径，必须完整路径
file(GLOB_RECURSE LIBS LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/lib/*")
# 递归查找所有的 .lib 文件路径，必须完整路径
file(GLOB_RECURSE ALL_LIBS
        LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/other/*/lib/*.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/other/*/lib/${CMAKE_BUILD_TYPE}/*.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/other/*/libs/*.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/other/*/libs/${CMAKE_BUILD_TYPE}/*.lib"
)
# 不递归 查找所有的 other 的直接文件夹（不包含子文件夹） 文件路径，必须完整路径
file(GLOB OTHER_READ_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} RECURSE false "other/*" )

# 排除特定目录（支持多个排除项）
set(OTHER_INCLUDE_DIRS "")
set(EXCLUDE_DIRS "")
foreach(DIR IN LISTS OTHER_READ_DIRS)
    set(IS_SKIP "0")
    foreach(EXCLUDE_DIR IN LISTS EXCLUDE_DIRS)
        if(DIR STREQUAL "${EXCLUDE_DIR}")
            set(IS_SKIP "1")
            BREAK()
        endif()
    endforeach()

    if (IS_SKIP STREQUAL "0")
        list(APPEND OTHER_INCLUDE_DIRS "${DIR}")
    endif ()
endforeach()

# 手动 添加新的源文件
# list(APPEND SRCS src/MyFileUtil.cpp src/PidUtil.cpp)



set(OTHER_LIBS "")
foreach(LIB IN LISTS ALL_LIBS)
    if (LIB MATCHES ".*Debug.*")
        if (CMAKE_BUILD_TYPE MATCHES "Debug")
            list(APPEND OTHER_LIBS "${LIB}")
        endif()
    elseif (LIB MATCHES ".*Release.*")
        if (CMAKE_BUILD_TYPE MATCHES "Release")
            list(APPEND OTHER_LIBS "${LIB}")
        endif()
    else ()
        list(APPEND OTHER_LIBS "${LIB}")
    endif ()
endforeach()


##########################################################################################
##########################################################################################
##########################################################################################


# 打印收集到的头文件（用于调试）
message("HEADS == ${HEADS}")
message("SRCS == " ${SRCS})
message("LIBS == " ${LIBS})
message("OTHER_LIBS == " ${OTHER_LIBS})
message("OTHER_INCLUDE_DIRS == " ${OTHER_INCLUDE_DIRS})




##########################################################################################
##########################################################################################
##########################################################################################


# 处理 .h 添加 include
# 获取唯一的目录路径
set(INCLUDE_DIRS "")
foreach(HEAD ${HEADS})
    get_filename_component(DIR ${HEAD} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
endforeach()
# 移除重复的目录路径
list(REMOVE_DUPLICATES INCLUDE_DIRS)
# 打印包含目录（用于调试）
message("HEADS: ${INCLUDE_DIRS}")


##########################################################################################
##########################################################################################
##########################################################################################

#set(YAML_CPP_INCLUDE "other\\yaml-cpp\\include")
include_directories(
        ${INCLUDE_DIRS}
        ${OTHER_INCLUDE_DIRS}
) # 添加 include 目录

##########################################################################################
##########################################################################################
##########################################################################################


# 模式
message("CMAKE_BUILD_TYPE == " ${CMAKE_BUILD_TYPE})


# debug 模式带黑框 release不带
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    # 添加 .cpp 文件
    add_executable(${PROJECT_NAME} main.cpp ${SRCS} res/resource.rc)
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    # 添加 .cpp 文件
    add_executable(${PROJECT_NAME} main.cpp ${SRCS} res/resource.rc)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/subsystem:windows /ENTRY:mainCRTStartup")
endif()

##########################################################################################
##########################################################################################
##########################################################################################

# 在编译可执行文件时，添加YAML_CPP_STATIC_DEFINE宏
#target_compile_definitions(${PROJECT_NAME} PUBLIC YAML_CPP_STATIC_DEFINE)

##########################################################################################
##########################################################################################
##########################################################################################

# 添加 .lib 文件
# 添加 curl 依赖 ws2_32 和 zlib 库的链接
#set(CURL_LIBRARY "D:/gcc/vcpkg/packages/curl_x64-windows-static/lib/libcurl.lib")
#set(ZLIB_LIBRARY "D:/gcc/vcpkg/packages/zlib_x64-windows-static/lib/zlib.lib")

#set(YAML_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}\\other\\yaml-cpp\\lib\\${CMAKE_BUILD_TYPE}\\yaml-cpp.lib")
# target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARY} ${ZLIB_LIBRARY} ws2_32 crypt32)
target_link_libraries(${PROJECT_NAME} ${LIBS} ${OTHER_LIBS})


##########################################################################################
##########################################################################################
##########################################################################################



##########################################################################################
##########################################################################################
##########################################################################################


# 添加自定义命令来复制 config.ini 文件到 build 目录
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/config.ini
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/config.ini
    DEPENDS ${CMAKE_SOURCE_DIR}/config.ini
    COMMENT "Copying config.ini to build directory"
)

# 添加自定义目标来确保 config.ini 文件被复制
# 注意：此目标标记为 ALL，因此会在每次构建时自动执行
add_custom_target(copy_config ALL
    DEPENDS ${CMAKE_BINARY_DIR}/config.ini
)

# 显式依赖主目标
add_dependencies(${PROJECT_NAME} copy_config)


##########################################################################################
##########################################################################################
##########################################################################################
# 复制 DLL 到输出目录

# 设置源码目录
#set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
#set(BUILD_DIR "${CMAKE_BINARY_DIR}")
#file(COPY
#        "${CMAKE_SOURCE_DIR}/other/yaml-cpp_x64-windows/bin/yaml-cpp.dll"
#        DESTINATION "${BUILD_DIR}"
#        FILES_MATCHING PATTERN "*.dll"
#)
##########################################################################################
##########################################################################################
##########################################################################################


# UTF-8 需要配置这个
add_compile_options(/wd4828)
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()