#!/bin/bash

DEF="fx"

if [ ! -f "$1" ]; then
    if [ $(ls *.hex 2>/dev/null|wc -l) -ne 1 ]; then
	if [ -f "${DEF}" ]; then
	    BIN="$DEF"
	else
	    echo >&2 "Use: dbgf4.sh binfile"
	    exit 1
	fi
    else
	BIN="$(ls *.hex|sed 's/\.hex//')"
    fi
else
    BIN="$1"
fi

openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg &
sleep 3

CMD="/tmp/gdb.cmd"
cat >$CMD <<EOF
target remote localhost:3333
monitor reset halt
break main
continue
EOF

gdb-multiarch -x $CMD "$BIN"

echo "reset run
shutdown"|nc localhost 4444

# End of dbgf4.sh
