#!/bin/bash

set -u
set -e

agent="$1"
test_app="$2"
dir_name=`dirname "$0"`
test_name=`basename "$1"`

export MIBDIRS=+${dir_name}/mibs
export MIBS=ALL
export TSAN_OPTIONS=second_deadlock_stack=1

# cleanup config files
killall ${test_name} || echo OK
pwd
find ${dir_name}/.. -name snmpv3_boot_counter -delete || echo OK
rm -rf config
mkdir -p config

set -x

# start agent as bg job
${agent} 4700 &
sleep 1

snmpstatus -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700

snmpEngineBoots=`snmpget -v2c -c public -Onqv localhost:4700 snmpEngineBoots.0`
test ${snmpEngineBoots} -eq 1 || killall ${test_name} # && exit 1
echo "OK, first boot"

snmpOutTraps=`snmpget -v2c -c public -Onqv localhost:4700 snmpOutTraps.0`
test ${snmpOutTraps} -eq 1 || killall ${test_name} # && exit 1
echo "OK, cold start trap sent"

snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEnableAuthenTraps.0 = enabled

snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmp
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngine

snmpbulkwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 iso

snmptable -Cib -v2c -c public localhost:4700 snmptargetaddrtable
snmptable -Cib -v2c -c public localhost:4700 snmptargetParamsTable

snmpwalk -v3 -l AuthNoPriv -u MD5 -a SHA -A MD5UserAuthPassword -n "" localhost:4700 system || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A SHAUserAuthPassword -n "" localhost:4700 system || echo ignored
snmpwalk -v3 -l AuthNoPriv -u MD5 -a MD5 -A MD5UserAuthPassword -n "" localhost:4700 system

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "wrong" localhost:4700 snmpEnableAuthenTraps.0 || echo OK
snmpwalk -v3 -l AuthNoPriv -u SHA -a SHA -A WrongUserAuthPassword -n "" localhost:4700 || echo OK
snmpwalk -v3 -l AuthNoPriv -u MD5 -n "" localhost:4700 || echo OK

snmpOutTraps=`snmpget -v2c -c public -Onqv localhost:4700 snmpOutTraps.0`
test ${snmpOutTraps} -ne 1 &&
echo "OK, authentication failure traps sent"

# start snmp_pp test_app too
${test_app} 127.0.0.1 get || echo OK

snmpget -v2c -c public localhost:4700 agentppNotifyTest.0 && snmpset -v2c -c public localhost:4700 agentppNotifyTest.0 = 1 || echo OK

test "${test_name}" == "cmd_exe_mib" && ${dir_name}/${test_name}.sh

test "${test_name}" == "atm_mib" && ${dir_name}/snmp_config_notify.sh localhost:4700
test "${test_name}" == "atm_mib" && ${dir_name}/snmp_notifyfiltertest.sh

kill -s TERM %%
wait %%

find config -type f

# start agent as bg job
${agent} 4700 &
sleep 1

snmpEngineBoots=`snmpget -v2c -c public -Onqv localhost:4700 snmpEngineBoots.0`
test ${snmpEngineBoots} -eq 2 &&
echo "OK, second boot"

snmpget -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngineBoots.0
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngineBoots.0 = 1 || echo OK
snmpset -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 sysContact.0 = clausklein
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 system
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 snmpEngine
snmpwalk -v3 -l noAuthNoPriv -u MD5DES -n "" localhost:4700 SNMPv2-MIB::snmpOutTraps.0

kill -s TERM %%
wait %%

echo done

