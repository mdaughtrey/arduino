PROJ=macropad
PORT=/dev/ttyUSB0
#PORT=/dev/ttyS0
#PLATFORM=rp2040:rp2040:generic
#PLATFORM=esp8266:esp8266:esp8266
#PLATFORM=arduino:avr:micro
PLATFORM=esp32:esp32:esp32
#PLATFORM=esp32:esp32:uPesy_wroom
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=~/acli/bin/arduino-cli

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
	${ACLI} compile ${PROJ}.ino -b ${PLATFORM} --output-dir  .

# Upload
u: 
	${ACLI} upload  -b ${PLATFORM} -p ${PORT} -i  ${ELF}

s:
	screen ${PORT} 115200

e:
	python3 ${HOME}/.arduino15/packages/esp32/tools/esptool_py/4.6/esptool.py --port ${PORT} --chip esp32 erase_flash
