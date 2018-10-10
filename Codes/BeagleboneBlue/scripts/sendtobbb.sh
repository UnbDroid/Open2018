#!/bin/bash
if [ "$1" != "" ]; then
	sshpass -p temppwd scp -r ./*.h* debian@192.168.8.1:~/"$1"/ 
    sshpass -p temppwd scp -r ./*.c* debian@192.168.8.1:~/"$1"/
else
    sshpass -p temppwd scp -r ./*.c* debian@192.168.8.1:~/code/
    sshpass -p temppwd scp -r ./*.h* debian@192.168.8.1:~/code/
fi

