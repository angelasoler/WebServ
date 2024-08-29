#!/bin/sh
host=localhost
port=8080
cmd="GET"
( echo open ${host} ${port}
sleep 1
echo ${cmd}
sleep 1
echo quit ) | telnet 