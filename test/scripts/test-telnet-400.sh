#!/bin/sh
host=localhost
port=8080
cmd="hkjhkjhk"
( echo open ${host} ${port}
sleep 1
echo ${cmd}
sleep 1
echo quit ) | telnet 