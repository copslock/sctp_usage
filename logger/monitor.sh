#!/bin/bash

while read line; do
    if [[ $line =~ ^"["(.+)"]"$ ]]; then
        obj_num=${BASH_REMATCH[1]}
        interface="/tmp/remote$obj_num"
        intf="$intf -i $interface"

        while read line; do
          if [[ $line =~ "host="(.+) ]]; then 
            host=${BASH_REMATCH[1]}
            echo $host
          elif [[ $line =~ "username="(.*) ]]; then
            username=${BASH_REMATCH[1]}
            echo $username
          elif [[ $line =~ ^"num="(.*) ]]; then
            num=${BASH_REMATCH[1]}
            echo $num
          else
            break
          fi
        done
      
      echo "ssh $username@$host \"sudo cat /tmp/local\" > $interface"
      ssh $username@$host "sudo cat /tmp/local" > $interface &
    fi
done < $1

sudo wireshark -k $intf
