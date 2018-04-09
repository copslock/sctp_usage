#!/bin/bash
usage() {
    cat <<EOM
    Usage:
    $(basename $0) <interface>
    Provide the interface to start the capture on
EOM
    exit 0
}

[ -z $1 ] && { usage; }

pipe=/tmp/local

if [[ -p $pipe ]]; then
  rm -f /tmp/local
fi

mkfifo /tmp/local
exec 3<>/tmp/local

tcpdump -s 0 -U -n -w - -i $1 not port 22 > /tmp/local

if [ $? -ne 0 ]; then
   usage
fi
