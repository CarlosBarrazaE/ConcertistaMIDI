cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(concertistamidi VERSION 1.2.0 LANGUAGES C CXX DESCRIPTION "Juego para aprender a tocar piano")
configure_file(codigo/version.h++.in version.h++)

#Se requiere C++17 para utilizar std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(ALSA REQUIRED)

find_library(ICUUC_LIBRERIA icuuc)
find_library(SQLITE3_LIBRERIA NAMES sqlite3)

include_directories(${FREETYPE_INCLUDE_DIRS})

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

if(NOT DEFINED CONSTRUCCION_LOCAL)
	if(TIPO_CONSTRUCCION STREQUAL Liberar)
		set(CONSTRUCCION_LOCAL FALSE)
	elseif(TIPO_CONSTRUCCION STREQUAL Depurar)
		set(CONSTRUCCION_LOCAL TRUE)
	endif()
endif()

if(UNIX)
	if(CONSTRUCCION_LOCAL)
		if(NOT DEFINED PREFIJO_INSTALACION)
			set(PREFIJO_INSTALACION "..")#CMAKE_SOURCE_DIR
		endif()
		set(RUTA_ARCHIVOS ${PREFIJO_INSTALACION})
	else()
		if(NOT DEFINED PREFIJO_INSTALACION)
			set(PREFIJO_INSTALACION "/usr")
		endif()
		set(INSTALACION_BINARIO "${PREFIJO_INSTALACION}/bin")
		set(INSTALACION_ARCHIVOS "${PREFIJO_INSTALACION}/share/concertistamidi")
		set(INSTALACION_LANZADOR "${PREFIJO_INSTALACION}/share/applications")
		set(INSTALACION_ICONO "${PREFIJO_INSTALACION}/share/icons/hicolor")
		set(RUTA_ARCHIVOS ${INSTALACION_ARCHIVOS})
	endif()
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

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ICUUC_LIBRERIA})
target_link_libraries(${PROJECT_NAME} ${ALSA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SQLITE3_LIBRERIA})

#INSTALACION
if(NOT CONSTRUCCION_LOCAL)
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALACION_BINARIO})
	install(DIRECTORY "${PROJECT_SOURCE_DIR}/musica" DESTINATION ${INSTALACION_ARCHIVOS})
	install(DIRECTORY "${PROJECT_SOURCE_DIR}/sombreadores" DESTINATION ${INSTALACION_ARCHIVOS})
	install(DIRECTORY "${PROJECT_SOURCE_DIR}/texturas" DESTINATION ${INSTALACION_ARCHIVOS})
	install(FILES "${PROJECT_SOURCE_DIR}/codigo/libmidi/COPYING" DESTINATION ${INSTALACION_ARCHIVOS} RENAME "licencia_libmidi.txt")
	install(FILES "${PROJECT_SOURCE_DIR}/Licencia.txt" DESTINATION ${INSTALACION_ARCHIVOS} RENAME "licencia_concertistamidi.txt")
	install(FILES "${PROJECT_SOURCE_DIR}/tipografias/NotoSans-Regular.ttf" DESTINATION "${INSTALACION_ARCHIVOS}/tipografias")
	install(FILES "${PROJECT_SOURCE_DIR}/varios/Concertista MIDI.desktop" DESTINATION ${INSTALACION_LANZADOR})
	install(FILES "${PROJECT_SOURCE_DIR}/varios/concertista_128.png" DESTINATION "${INSTALACION_ICONO}/128x128/apps" RENAME "concertistamidi.png")

	#EMPAQUETAR
	set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_PACKAGE_DESCRIPTION "Concertista MIDI es un juego para aprender a tocar piano facilmente usando el teclado, el raton o teclados MIDI, ahora incluso puede controlar las teclas luminosas del organo Casio LK-S250 y posiblemente muchos otros. Concertista MIDI utiliza la libreria midi de linthesia para reproducir archivos y conectarse con dispositivos MIDI pero incluye muchas mejoras.")
	set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/CarlosBarrazaE/ConcertistaMIDI")
	set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/varios/concertista_128.png")
	set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/LEEME.md")
	set(CPACK_PACKAGE_VENDOR "Carlos Barraza")
	set(CPACK_PACKAGE_CONTACT, "carlosbarrazaes@gmail.com")

	#cpack -G DEB
	set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
	#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopengl0,libglew2.1,libsdl2-2.0-0,libfreetype6,libicu66,libsqlite3-0,libasound2,timidity")#ubuntu 20.04
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopengl0,libglew2.1,libsdl2-2.0-0,libfreetype6,libicu67,libsqlite3-0,libasound2,timidity")#debian 11, ubuntu 21.10
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Carlos Barraza <carlosbarrazaes@gmail.com>")

	#cpack -G RPM instalar rpm-build
	set(
		CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
		/usr/share/icons
		/usr/share/icons/hicolor
		/usr/share/icons/hicolor/128x128
		/usr/share/icons/hicolor/128x128/apps
		/usr/share/applications
	)
	set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_RPM_PACKAGE_REQUIRES "libglvnd-opengl,libGLEW,SDL2,freetype,libicu,sqlite-libs,alsa-lib,timidity++")#fluid-soundfont-lite-patches
	set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
	set(CPACK_RPM_PACKAGE_LICENSE "MIT")
	include(CPack)
endif()
