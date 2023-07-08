export DEVICENAME=/dev/ttyS4
export BAUD_RATE=921600

# Compile the binary
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=sparkfun_edge TAGS=cmsis-nn micro_speech_bin

# Sign the binary
python3 tensorflow/lite/micro/tools/make/downloads/AmbiqSuite-Rel2.2.0/tools/apollo3_scripts/create_cust_image_blob.py --bin tensorflow/lite/micro/tools/make/gen/sparkfun_edge_cortex-m4/bin/micro_speech.bin --load-address 0xC000 --magic-num 0xCB -o main_nonsecure_ota --version 0x0

# Create final binary version
python3 tensorflow/lite/micro/tools/make/downloads/AmbiqSuite-Rel2.2.0/tools/apollo3_scripts/create_cust_wireupdate_blob.py --load-address 0x20000 --bin main_nonsecure_ota.bin -i 6 -o main_nonsecure_wire --options 0x1

# Flash the binary
python3 tensorflow/lite/micro/tools/make/downloads/AmbiqSuite-Rel2.2.0/tools/apollo3_scripts/uart_wired_update.py -b ${BAUD_RATE} ${DEVICENAME} -r1 -f main_nonsecure_wire.bin -i 6

screen ${DEVICENAME} 115200
