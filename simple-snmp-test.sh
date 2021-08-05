#!/bin/bash

dir_name=`basename "$0"`
export MIBDIRS=+${dir_name}/mibs
export MIBS=ALL

set -u
set -e
set -x

# cleanup config files
rm -f snmpv3_boot_counter
rm -rf config

mkdir -p config

killall agent || echo OK
pkill agent || echo OK

# start as bg job
examples/static_table/src/agent 4700 &
sleep 1
cat snmpv3_boot_counter

snmpstatus -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700

snmpEngineBoots=`snmpget -v1 -c public -Onqv localhost:4700 snmpEngineBoots.0`
test ${snmpEngineBoots} -eq 1 || exit 1
echo "OK, first boot"

snmpOutTraps=`snmpget -v1 -c public -Onqv localhost:4700 snmpOutTraps.0`
test ${snmpOutTraps} -eq 1 || exit 1
echo "OK, cold start trap sent"

snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEnableAuthenTraps.0 = enabled

snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmp
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngine

snmpbulkwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 iso

snmptable -Cib -v1 -c public  localhost:4700 snmptargetaddrtable
snmptable -Cib -v2c -c public localhost:4700 snmptargetaddrtable
snmptable -Cib -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmptargetaddrtable

snmpwalk -v3 -l AuthNoPriv -u MD5 -a SHA -A MD5UserAuthPassword -n "" localhost:4700 system || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A SHAUserAuthPassword -n "" localhost:4700 system
snmpwalk -v3 -l AuthNoPriv -u MD5 -a MD5 -A MD5UserAuthPassword -n "" localhost:4700 system

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "wrong" localhost:4700 snmpEnableAuthenTraps.0 || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A WrongUserAuthPassword -n "" localhost:4700 || echo OK
snmpwalk -v3 -l AuthNoPriv -u MD5 -n "" localhost:4700 || echo OK

snmpOutTraps=`snmpget -v1 -c public -Onqv localhost:4700 snmpOutTraps.0`
test ${snmpOutTraps} -ne 1 || exit 1
echo "OK, authentication failure trap sent"

# start snmp_pp test_app too
_deps/snmp_pp-build/test_app 127.0.0.1 get

# pkill agent
kill -s TERM %%
wait

ls -lrta config

# start as bg job
examples/static_table/src/agent 4700 &
sleep 1
cat snmpv3_boot_counter

snmpEngineBoots=`snmpget -v2c -c public -Onqv localhost:4700 snmpEngineBoots.0`
test ${snmpEngineBoots} -eq 2 || exit 1
echo "OK, second boot"

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngineBoots.0
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngineBoots.0 = 1 || echo OK
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 sysContact.0 = clausklein
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngine
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 SNMPv2-MIB::snmpOutTraps.0

# pkill agent
kill -s TERM %%
wait

echo done

