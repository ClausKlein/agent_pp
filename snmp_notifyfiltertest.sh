#!/bin/bash

# ('-quote is for fixed length strings)
# ("-quote is for variable length strings)
SNMP_PORT=4700
SNMP_TARGET=\'defaultV2Trap\' # fix length octetstring
SNMP_HOST="-v2c -c public -r 0 localhost:${SNMP_PORT}"
myindex="6.65.66.67.68.69.70" # ABCDEF -> variable length octetstring

set -x  # be verbose
set -u

##############################################################
# active(1) notInService(2) notReady(3) createAndGo(4) createAndWait(5) destroy(6)
##############################################################

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.1.3.6.1.2.1.4.21.1 = destroy || echo ignored

##############################################################
# RFC1213-MIB::ipRouteTable excluded:
##############################################################
# "The Name of the filter profile to be used when generating
#  notifications using the corresponding entry in the snmpTargetAddrTable."
# INDEX { IMPLIED snmpTargetParamsName } -> 'snmpd'
snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = createAndWait \
    snmpNotifyFilterProfileName."'snmpd'" = ${myindex} \
    snmpNotifyFilterStorageType."'snmpd'" = nonVolatile \
    snmpNotifyFilterProfileRowStatus."'snmpd'" = active || echo Error ignored

snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.1.3.6.1.2.1.4.21.1 = createAndWait \
    snmpNotifyFilterMask.${myindex}.1.3.6.1.2.1.4.21.1 = fffb \
    snmpNotifyFilterType.${myindex}.1.3.6.1.2.1.4.21.1 = excluded \
    snmpNotifyFilterStorageType.${myindex}.1.3.6.1.2.1.4.21.1 = nonVolatile \
    snmpNotifyFilterRowStatus.${myindex}.1.3.6.1.2.1.4.21.1 = active || echo Error ignored

##############################################################

set -e  # exit on error

############################# following commands must be OK #############################

snmpset ${SNMP_HOST} \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = destroy || echo ignored
snmpset ${SNMP_HOST} \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = destroy || echo ignored

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = createAndGo \
    snmpNotifyFilterProfileName."'snmpd'" = '"link-status"' \
    snmpNotifyFilterStorageType."'snmpd'" = nonVolatile \
    snmpNotifyFilterProfileRowStatus."'snmpd'" = active | echo FIXME!

snmpset ${SNMP_HOST} \
    snmpNotifyFilterMask.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = "" \
    snmpNotifyFilterType.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = included \
    snmpNotifyFilterStorageType.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = nonVolatile \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = createAndGo

snmpset ${SNMP_HOST}  \
    snmpNotifyFilterMask.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = "" \
    snmpNotifyFilterType.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = included \
    snmpNotifyFilterStorageType.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = nonVolatile \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = createAndGo

snmptable -Cib ${SNMP_HOST} snmpNotifyFilterTable

# Following is an example configuration of a named log for logging only linkUp and linkDown Notifications.
snmpset ${SNMP_HOST} \
    nlmConfigLogEntryStatus.'"links"' = destroy || echo ignored
snmpset ${SNMP_HOST} \
    nlmConfigLogFilterName.'"links"'  = '"link-status"' \
    nlmConfigLogEntryLimit.'"links"'  = 2 \
    nlmConfigLogAdminStatus.'"links"' = enabled \
    nlmConfigLogStorageType.'"links"' = nonVolatile \
    nlmConfigLogEntryStatus.'"links"' = createAndGo


snmpset ${SNMP_HOST} nlmConfigLogEntryLimit.0 = 4 || echo "OK, readonly!"

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 11 \
    NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 1 # minutes

snmpset ${SNMP_HOST} nlmConfigLogEntryStatus.'"testGroup"' = destroy || echo ignored
#XXX NOTIFICATION-LOG-MIB::nlmConfigLogFilterName.'"testGroup"' = ${myindex} \
snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.'"testGroup"' = createAndWait \
    NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus.'"testGroup"' = enabled \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.'"testGroup"' = 8 \
    NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.'"testGroup"' = nonVolatile \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.'"testGroup"' = active

snmpwalk ${SNMP_HOST} nlmConfigGlobal
snmpwalk ${SNMP_HOST} nlmStats
snmpwalk ${SNMP_HOST} nlmConfigLogOperStatus
snmptable -Cib ${SNMP_HOST} nlmConfigLogTable

snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
sleep 60 # 1 minutes
snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1

snmpwalk ${SNMP_HOST} nlmLogNotificationID
snmpwalk ${SNMP_HOST} nlmStats

nlmStatsGlobalNotificationsLogged=`snmpget -OnqUv ${SNMP_HOST} nlmStatsLogNotificationsLogged.0`
snmpOutTraps=`snmpget -OnqUv ${SNMP_HOST} snmpOutTraps.0`
test ${snmpOutTraps} -eq ${nlmStatsGlobalNotificationsLogged}

MIBTABLES="snmpTargetParamsTable snmpTargetAddrTable
  snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable
  nlmStatsLogTable nlmLogTable nlmConfigLogTable nlmLogVariableTable
"

for Table in ${MIBTABLES}; do snmptable -Cibw 135 ${SNMP_HOST} ${Table}; done

snmpwalk ${SNMP_HOST} nlmStats

