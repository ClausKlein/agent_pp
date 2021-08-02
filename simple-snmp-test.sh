#!/bin/bash
set -u
set -e
set -x

#TBD rm -rf config
#TBD rm -f snmpv3_boot_counter

mkdir -p config

killall agent || echo OK
pkill agent || echo OK

examples/static_table/src/agent &
sleep 1
cat snmpv3_boot_counter

snmpstatus -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEnableAuthenTraps.0 = enabled

snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmp
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngine

snmpbulkwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 iso

snmptable -Cib -v1 -c public -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmptargetaddrtable
snmptable -Cib -v2c -c public -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmptargetaddrtable
snmptable -Cib -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmptargetaddrtable

snmpwalk -v3 -l AuthNoPriv -u MD5 -a SHA -A MD5UserAuthPassword -n "" localhost:4700 system || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A SHAUserAuthPassword -n "" localhost:4700 system
snmpwalk -v3 -l AuthNoPriv -u MD5 -a MD5 -A MD5UserAuthPassword -n "" localhost:4700 system

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "wrong" localhost:4700 snmpEnableAuthenTraps.0 || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A WrongUserAuthPassword -n "" localhost:4700 || echo OK
snmpwalk -v3 -l AuthNoPriv -u MD5 -n "" localhost:4700 || echo OK

snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 SNMPv2-MIB::snmpOutTraps.0

_deps/snmp_pp-build/test_app

# pkill agent
kill -s TERM %%
wait

ls -lrta config

examples/static_table/src/agent 4711 &
sleep 1
cat snmpv3_boot_counter

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4711 snmpEngineBoots.0
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4711 snmpEngineBoots.0 = 1 || echo OK
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4711 sysContact.0 = clausklein
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4711 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4711 snmpEngine

# pkill agent
kill -s TERM %%
wait

echo done

