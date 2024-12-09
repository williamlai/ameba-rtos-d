#!/bin/sh

set -x

DEVICE="/dev/ttyUSB0"

PROGNAME="$0"
readonly PROGNAME

TOP_DIR=$(readlink -f "$(dirname "$PROGNAME")/../../..")

IMAGE_TOOL_DIR="$TOP_DIR/tools/AmebaD/Image_Tool_Linux"
IMAGE_TOOL="$IMAGE_TOOL_DIR/AmebaD_ImageTool"

IMAGE_KM0="$TOP_DIR/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp/asdk/image/km0_boot_all.bin"
IMAGE_KM4="$TOP_DIR/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km4_boot_all.bin"
IMAGE_KM0_KM4_IMAGE2="$TOP_DIR/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/image/km0_km4_image2.bin"

# remove file Image_All.bin whether it exists or not
rm -f Image_All.bin

$IMAGE_TOOL -set chip AmebaD
$IMAGE_TOOL -set baudrate 1500000
$IMAGE_TOOL -add device /dev/ttyUSB0

$IMAGE_TOOL -set address 0x08000000

$IMAGE_TOOL -combine \
    $IMAGE_KM0 0x0000 \
    $IMAGE_KM4 0x4000 \
    $IMAGE_KM0_KM4_IMAGE2 0x6000

$IMAGE_TOOL -download
