PROJ=macropad
PORT=/dev/ttyACM0
PORT2=/dev/ttyUSB0
#PORT=/dev/ttyUSB0
#PLATFORM=rp2040:rp2040:generic
#PLATFORM=esp8266:esp8266:esp8266
#PLATFORM=arduino:avr:micro
PLATFORM=esp32:esp32:esp32s3
#PLATFORM=esp32:esp32:uPesy_wroom
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=~/acli/bin/arduino-cli
GDB=/home/mattd/.arduino15/packages/esp32/tools/xtensa-esp-elf-gdb/12.1_20231023/bin/xtensa-esp32s3-elf-gdb
OPEN_OCD=/home/mattd/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/bin/openocd
OPEN_OCD_SCRIPTS=/home/mattd/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/share/openocd/scripts/
#ACLI_PROPERTIES=--build-property build.partitions=minimal --build-property upload.maximum_size=1966080
#ACLI_PROPERTIES=--build-property build.partitions=min_spiffs --build-property upload.maximum_size=1966080
#esp32c2.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs
#esp32c2.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080
#ACLI_DEFINES=--build-property compiler.cpp.extra_flags=-DFORMAT_SPIFFS_IF_FAILED=true
#" --file "board/esp32s3-builtin.cfg" -c "gdb_port pipe" -c "telnet_port 0"

all:
	make b
	
init:
	yq -yi '.board_manager.additional_urls=["https://dl.espressif.com/dl/package_esp32_index.json"]' ~/.arduino15/arduino-cli.yaml
	${ACLI} core install esp32:esp32

ctags:
	ctags --recurse ~/.arduino15 
	ctags --append --recurse ~/Arduino 
	ctags --append --recurse *
# Build
b:
	${ACLI} compile ${PROJ}.ino ${ACLI_PROPERTIES} ${ACLI_DEFINES} -b ${PLATFORM} --verbose --output-dir  . 2>&1 | tee build.log

# Upload
u: 
	${ACLI} upload  -b ${PLATFORM} -p ${PORT} -i ${ELF} 2>&1 | tee upload.log

s:
	screen ${PORT} 115200

d:
	${ACLI} debug -b ${PLATFORM}

o:
	${OPEN_OCD} -s ${OPEN_OCD_SCRIPTS} --file board/esp32s3-builtin.cfg

g:
	${GDB} -x .gdbinit ${ELF}

dg:
	${OPEN_OCD} -s ${OPEN_OCD_SCRIPTS} --file board/esp32s3-builtin.cfg -c "gdb_port pipe" -c "telnet_port 0"
#	${GDB} -x .gdbinit ${ELF}

#	OPENOCD_CONFIG=openocd.cfg ${ACLI} debug -b ${PLATFORM} 

e:
	python3 ${HOME}/.arduino15/packages/esp32/tools/esptool_py/4.6/esptool.py --port ${PORT} --chip esp32s3 erase_flash


# https://codeblog.dotsandbrackets.com/arduino-cli-partition-scheme/
