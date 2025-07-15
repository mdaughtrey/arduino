ACLI=~/acli/bin/arduino-cli
PROJ=climate_sensor
#PORT=/dev/ttyACM0
#PORT=/dev/ttyAMA0
#PORT=/dev/ttyAMA10
PORT=/dev/ttyUSB0
#PLATFORM=rp2040:rp2040:rpipico
PLATFORM=esp32:esp32:esp32
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=/home/mattd/acli/bin/arduino-cli
DEV=/dev/sdb1
MOUNT=/media/rp2040

DOCKERHOST=pop-os
DOCKERNAME=acli
DOCKER=docker
RBDIR=~/remotebuild/arduino/${PROJ}
#GDB=/home/mattd/.arduino15/packages/esp32/tools/xtensa-esp-elf-gdb/12.1_20231023/bin/xtensa-esp32s3-elf-gdb
#GDB=/home/mattd/.arduino15/packages/rp2040/tools/pqt-gcc/4.0.1-8ec9d6f/bin/arm-none-eabi-gdb
#OPEN_OCD=/home/mattd/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/bin/openocd
#OPEN_OCD=/home/mattd/.arduino15/packages/rp2040/tools/pqt-openocd/4.0.1-8ec9d6f/bin/openocd
#OPEN_OCD_SCRIPTS=/home/mattd/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/share/openocd/scripts/
#OPEN_OCD_SCRIPTS=/home/mattd/.arduino15/packages/rp2040/tools/pqt-openocd/4.0.1-8ec9d6f/share/openocd/scripts/
#OPEN_OCD_SCRIPTS=/home/mattd/.arduino15/packages/arduino/tools/openocd/0.11.0-arduino2/share/openocd/scripts/
#ACLI_PROPERTIES=--build-property build.partitions=min_spiffs # --build-property upload.maximum_size=1966080
#ACLI_DEFINES=--build-property compiler.cpp.extra_flags=-DFORMAT_SPIFFS_IF_FAILED=true
#ACLI_DEFINES=--build-property compiler.cpp.extra_flags=-DDEBUG_RP2040_PORT=Serial
#ACLI_PROPERTIES=--build-property upload.maximum_size=2027520
#ACLI_PROPERTIES+=--build-property build.flash_total=2097152
#ACLI_PROPERTIES+=--build-property build.flash_length=2027520
#ACLI_PROPERTIES+=--build-property build.eeprom_start=270528512
#ACLI_PROPERTIES+=--build-property build.fs_start=270462976
#ACLI_PROPERTIES+=--build-property build.fs_end=270528512

all:
	make b
	make u
	make s
	
# Upload - remove and reinsert SS micro every  time
u:
	${ACLI} upload -i ${IMAGE} -b ${PLATFORM} -p ${PORT} 

d:
	${ACLI} debug --input-dir ~/${PROJDIR} -b ${PLATFORM} -p ${PORT}
# Build
b:
	${ACLI} compile --build-path buildfiles --clean ${ACLI_PROPERTIES} ${ACLI_DEFINES} ${PROJ}.ino -b ${PLATFORM} --output-dir . 2>&1 | tee build.log

s:
	screen ${PORT} 115200
