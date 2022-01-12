cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(concertistamidi VERSION 1.1)
configure_file(codigo/version.h++.in version.h++)

#Se requiere C++17 para utilizar std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)

find_library(ICUUC_LIBRARY icuuc)
find_library(SQLITE3_LIBRERIA NAMES sqlite3)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(ALSA REQUIRED alsa)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${ICUUC_INCLUDE_DIR})
include_directories(${ALSA_INCLUDE_DIRS})

#La version de Lanzamiento es predeterminada
if(NOT DEFINED TIPO_CONSTRUCCION)
	set(TIPO_CONSTRUCCION Liberar)
endif()

option(OPTIMIZAR_NATIVO "Construye con -march=native" OFF)

if(TIPO_CONSTRUCCION STREQUAL Liberar)
	message("Versión de lanzamiento")
	set(NIVEL_REGISTRO 2)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -O2")
elseif(TIPO_CONSTRUCCION STREQUAL Depurar)
	message("Versión de depuración")
	set(NIVEL_REGISTRO 3)
	set(advertencias "-Wall -Wextra -Wdelete-non-virtual-dtor -Wswitch-default -Wshadow -Wsign-promo -Wctor-dtor-privacy -Wcast-align -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wstrict-null-sentinel -Wundef -Wformat=2 -Wunused-macros -Wno-endif-labels -Wstrict-overflow=4 -Warray-bounds=2 -Wold-style-cast -Wconversion -Wunused -Wdouble-promotion -Wformat-overflow=2 -Wno-ignored-attributes -Wattributes -Wunused-but-set-variable -Wduplicated-branches -Wduplicated-cond -Wtrampolines -Wfloat-equal -Wshadow=global -Wunsafe-loop-optimizations -Wpointer-arith -Wcast-qual -Wdangling-else -Wfloat-conversion -Wlogical-op -Wredundant-decls -Wsign-conversion")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs ${advertencias} -g3")
else()
	message(SEND_ERROR "Tipo de construccion no definido, solo Liberar o Depurar")
endif()

if(OPTIMIZAR_NATIVO)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

configure_file(codigo/configuracion_cmake.h++.in configuracion_cmake.h++)

add_subdirectory(codigo)

set(todo
		${codigo}
)

add_executable(${PROJECT_NAME} ${todo})

#Incluye .h++ que se genera a partir de .h++.in
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PUBLIC ${SDL2_CFLAGS_OTHER})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ICUUC_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALSA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SQLITE3_LIBRERIA})
