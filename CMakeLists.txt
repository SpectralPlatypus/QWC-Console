cmake_minimum_required(VERSION 3.10)

project(QWC_Console)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

if (MSVC)
	message(STATUS "Lowering warning level to W3")
    add_definitions(/W3)
	message(STATUS "Adding MSVC parallel build")
	add_definitions(/MP)	
ENDIF ()

set(sources
	src/dxsdk/Direct3D8Wrapper.cpp
	src/dxsdk/Direct3DDevice8Wrapper.cpp
	src/imgui_dx8/imgui_impl_dx8.cpp
	src/QWC/CLIData.cpp
	imgui/imgui_draw.cpp
	imgui/imgui.cpp
)

set(inc
	inc/dxsdk/Direct3D8Wrapper.h
	inc/dxsdk/Direct3DDevice8Wrapper.h
	inc/imgui_dx8/imgui_impl_dx8.h
	inc/QWC/CLIData.h
	inc/Console.h
	imgui/imconfig.h
	imgui/imgui_internal.h
	imgui/stb_rect_pack.h
	imgui/stb_textedit.h
	imgui/stb_truetype.h
)

add_library(d3d8 SHARED dllmain.cpp d3d8.def ${sources} ${inc})

target_include_directories(d3d8 PUBLIC 
	${PROJECT_SOURCE_DIR}/inc/
	${PROJECT_SOURCE_DIR}/inc/dxsdk
	${PROJECT_SOURCE_DIR}/inc/imgui_dx8
	${PROJECT_SOURCE_DIR}/inc/QWC	
	${PROJECT_SOURCE_DIR}/imgui
)
