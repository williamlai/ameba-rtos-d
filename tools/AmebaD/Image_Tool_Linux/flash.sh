#!/usr/bin/env bash

# Flash firmware usage
# ./flash.sh <Device Port> <Path to km0_boot_all.bin> <Path to km4_boot_all.bin> <Path to km0_km4_image2.bin> <address[optional]>
# Flash facotrydata binary usage
# ./flash.sh <Device Port> <Path to ameba_factory.bin> <address>

PROGNAME="$0"
readonly PROGNAME

TOP_DIR=$(readlink -f "$(dirname "$PROGNAME")/../../..")
AMEBAD_IMAGE_TOOL="$TOP_DIR/tools/AmebaD/Image_Tool_Linux/AmebaD_ImageTool"

if [ -z "$1" ]; then
    echo "No device port provided"
    exit
fi

if [ -z "$2" ]; then
    echo "No output directory provided"
    exit
fi

if [ "$#" -eq 3 ]; then
    FACTORY_DATA_IMG=$2

    sudo $AMEBAD_IMAGE_TOOL -add device $1
    sudo $AMEBAD_IMAGE_TOOL -set baudrate 1500000
    sudo $AMEBAD_IMAGE_TOOL -set image $FACTORY_DATA_IMG
    if [ -n "$3" ]; then
        echo "Flashing to address $3"
        sudo $AMEBAD_IMAGE_TOOL -set address $3
    else
        # if no address is given, use default address 0x08000000
        echo "Flashing to address 0x08000000"
        sudo $AMEBAD_IMAGE_TOOL -set address 0x08000000
    fi
    sudo $AMEBAD_IMAGE_TOOL -show
    read -p "Check if the settings are correct, then enter UART DOWNLOAD mode
    1) Push the UART DOWNLOAD button and keep it pressed.
    2) Re-power on the board or press the Reset button.
    3) Release the UART DOWNLOAD button.
    Once AmebaD entered UART Download mode, press Enter to continue"
    sudo $AMEBAD_IMAGE_TOOL -download
else
    SDK_ROOT_DIR=$PWD/../../..
    KM0_BOOT_ALL_IMG=$2
    KM4_BOOT_ALL_IMG=$3
    KM0_KM4_IMG=$4
    sudo $AMEBAD_IMAGE_TOOL -set chip AmebaD
    rm -rf log* Image_All.bin*
    sudo $AMEBAD_IMAGE_TOOL -add device $1
    sudo $AMEBAD_IMAGE_TOOL -set baudrate 1500000
    sudo $AMEBAD_IMAGE_TOOL -combine $KM0_BOOT_ALL_IMG 0x0000 $KM4_BOOT_ALL_IMG 0x4000 $KM0_KM4_IMG 0x6000
    sudo $AMEBAD_IMAGE_TOOL -set image $PWD/Image_All.bin
    if [ -n "$5" ]; then
        echo "Flashing to address $5"
        sudo $AMEBAD_IMAGE_TOOL -set address $5
    else
        # if no address is given, use default address 0x08000000
        echo "Flashing to address 0x08000000"
        sudo $AMEBAD_IMAGE_TOOL -set address 0x08000000
    fi
    sudo $AMEBAD_IMAGE_TOOL -show
    read -p "Check if the settings are correct, then enter UART DOWNLOAD mode
    1) Push the UART DOWNLOAD button and keep it pressed.
    2) Re-power on the board or press the Reset button.
    3) Release the UART DOWNLOAD button.
    Once AmebaD entered UART Download mode, press Enter to continue"
    sudo $AMEBAD_IMAGE_TOOL -download
fi

