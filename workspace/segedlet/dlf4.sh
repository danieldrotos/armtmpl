#!/bin/bash

DEF="demo_iofw.hex"


if [ ! -f "$1" ]; then
    if [ $(ls *.hex 2>/dev/null|wc -l) -ne 1 ]; then
	if [ -f "${DEF}" ]; then
	    HEX="$DEF"
	else
	    echo >&2 "Use: dlf4.sh hexfile"
	    exit 1
	fi
    else
	HEX="$(ls *.hex)"
    fi
else
    HEX="$1"
fi

openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg &
sleep 1

echo "
init
halt
reset halt
flash write_image erase "${HEX}" 0 ihex
reset run
shutdown"|nc localhost 4444
