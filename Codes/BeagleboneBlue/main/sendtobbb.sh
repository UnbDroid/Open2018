#!/bin/bash
if [[ "$1" != "" ]]; then
	if [[ "$2" != "" ]]; then
		sshpass -p temppwd scp -r ./"$1" debian@192.168.8.1:~/"$2"/
		sshpass -p temppwd scp -r ./"$1" debian@192.168.8.1:~/"$2"/
	else
		echo "pra qual pasta quer enviar seu arquivo"
	fi
else
	sshpass -p temppwd scp -r ./m*.c* debian@192.168.8.1:~/code/
	sshpass -p temppwd scp -r ./m*.h* debian@192.168.8.1:~/code/
fi