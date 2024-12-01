add_library(pico_emu_utils INTERFACE)

pico_generate_pio_header(pico_emu_utils ${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd_clk0_dat.pio)
pico_generate_pio_header(pico_emu_utils ${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd_clk2_dat.pio)
pico_generate_pio_header(pico_emu_utils ${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd_clk6_dat.pio)
pico_generate_pio_header(pico_emu_utils ${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd_dat_clk.pio)

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
	${CMAKE_CURRENT_LIST_DIR}/src/ps2kbd.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/ff_util.c
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsFilePath.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsDirCacheInputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsDirCacheOutputStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsDirCache.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FatFsDirCacheSorter.cpp
)

target_include_directories(pico_emu_utils INTERFACE
    src
)

