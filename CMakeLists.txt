cmake_minimum_required(VERSION 3.12)
project(vulkanLearning)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(. DIRSRCS)

include_directories(
 #SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../Third_party_libraries/include
 #SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../../VulkanSDK/1.3.283.0/Include
SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../3edlib/include
SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../../VulkanSDK/1.3.283.0/Include
)

link_directories(
 #SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../Third_party_libraries/lib
 #SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../../VulkanSDK/1.3.283.0/Lib
SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../3edlib/lib
SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../../VulkanSDK/1.3.283.0/Lib
)

add_subdirectory(vulkanWrapper)

add_executable(vulkanLearning-1 ${DIRSRCS})

target_link_libraries(vulkanLearning-1 vulkanLib vulkan-1.lib glfw3.lib) 