ACLI=~/acli/bin/arduino-cli
PROJ=fingerkey
PORT=/dev/ttyACM0
#PORT=/dev/ttyS0
PLATFORM=rp2040:rp2040:rpipico
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=/home/mattd/acli/bin/arduino-cli
DEV=/dev/sdb1
MOUNT=/media/rp2040

#DOCKERHOST=DESKTOP-TOJK787
DOCKERHOST=pop-os
DOCKERNAME=acli
DOCKER=docker
RBDIR=~/remotebuild/arduino/${PROJ}

#ACLI_PROPERTIES=--build-property build.partitions=min_spiffs # --build-property upload.maximum_size=1966080
#ACLI_DEFINES=--build-property compiler.cpp.extra_flags=-DFORMAT_SPIFFS_IF_FAILED=true
ACLI_DEFINES=--build-property compiler.cpp.extra_flags=-DDEBUG_RP2040_PORT=Serial
ACLI_PROPERTIES=--build-property upload.maximum_size=2027520
ACLI_PROPERTIES+=--build-property build.flash_total=2097152
ACLI_PROPERTIES+=--build-property build.flash_length=2027520
ACLI_PROPERTIES+=--build-property build.eeprom_start=270528512
ACLI_PROPERTIES+=--build-property build.fs_start=270462976
ACLI_PROPERTIES+=--build-property build.fs_end=270528512

all:
	make b
#ssh mattd@${DOCKERHOST}  ${DOCKER} run --rm -v ~/dev/personal-projects:/personal-projects 
# Build remote
br:
	rsync -r --mkpath . mattd@${DOCKERHOST}:${RBDIR}
	ssh mattd@${DOCKERHOST}  ${DOCKER} run --rm -v ${RBDIR}:/${PROJ}  \
	--entrypoint /home/mattd/acli/bin/arduino-cli \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile /${PROJ}/${PROJ}.ino -b ${PLATFORM} --output-dir /${PROJ} rsync mattd@${DOCKERHOST}:${RBDIR}/${PROJ}.ino.bin :${RBDIR}/${PROJ}.ino.elf :${RBDIR}/${PROJ}.ino.map :${RBDIR}/${PROJ}.ino.uf2 .
#	ssh mattd@pop-os  ${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
#        --entrypoint /home/mattd/acli/bin/arduino-cli \
#		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}
	
# Build
b:
	${ACLI} compile ${ACLI_PROPERTIES} ${ACLI_DEFINES} ${PROJ}.ino -b ${PLATFORM} --output-dir . 2>&1 | tee build.log

# Upload remote
#ur: 
#	scp pop-os:~${PROJDIR}/${IMAGE} /tmp
#	sudo bossac -U --port=${PORT} -e -w /tmp/${IMAGE}

# Upload
u: 
	${ACLI} upload  -b ${PLATFORM} -p ${PORT} -i  ${UF2}
#	stty -F ${PORT} 1200
#	bossac --port=${PORT} -e -w -v -b ${IMAGE}
ur: 
	${ACLI} upload --log-level trace --log-file acli.log -b ${PLATFORM} -p ${PORT} -i /home/mattd/mnt/arduino/projv4/projv4.ino.uf2

a:
	docker attach acli

s:
	-rm screenlog.0
	screen -L ${PORT} 115200

c:
	cat ${PORT} > serial.log

#f:
#	sudo scp mattd@pop-os:~${PROJDIR}/${UF2} /media/rp2040/
#
#m:
#	sudo mount /dev/sdc1 /media/rp2040  -o rw,users
#
#mc: 	# mount and copy
#	./mntrp2040.sh
#	sudo cp  ${UF2} ${MOUNT}
#us:
#	esptool --port ${PORT}. --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ~${PROJDIR}/${IMAGE}
#	screen ${PORT} 115200

sshfs:
	sudo sshfs -o allow_other mattd@${DOCKERHOST}:/home/mattd/dev/personal-projects /home/mattd/mnt
	#sudo sshfs -o allow_other mattd@pop-os:/home/mattd/personal-projects /home/mattd/mnt

unmount:
	sudo fusermount -u /home/mattd/mnt

t:
	stty -F ${PORT} 1200

m:
	sudo mount ${DEV} ${MOUNT}

um:
	sudo umount ${MOUNT}

g:
	ctags --recurse --languages=c++ . ~/Arduino ~/.arduino15

cp:
	sudo cp ${UF2} ${MOUNT}
