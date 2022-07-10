add_library(pico_emu_utils INTERFACE)

pico_generate_pio_header(pico_emu_utils ${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd.pio)

target_sources(pico_emu_utils INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ArrayInputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/BufferedInputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsSpiDirReader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsSpiInputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsSpiOutputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/InputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/OutputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/SdCardFatFsSpi.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/SizingOutputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/hid_host_utils.c
	${CMAKE_CURRENT_LIST_DIR}/src/hid_host_joy.c
	${CMAKE_CURRENT_LIST_DIR}/src/hid_host_info.c
	${CMAKE_CURRENT_LIST_DIR}/src/hid_app.c
	${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd.cpp
)

target_include_directories(pico_emu_utils INTERFACE
    src
)

