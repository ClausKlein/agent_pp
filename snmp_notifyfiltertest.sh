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

snmpset ${SNMP_HOST} \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.3 = destroy || echo ignored
snmpset ${SNMP_HOST} \
    snmpNotifyFilterRowStatus.'"link-status"'.1.3.6.1.6.3.1.1.5.4 = destroy || echo ignored

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus."'snmpd'" = createAndGo \
    snmpNotifyFilterProfileName."'snmpd'" = '"link-status"' \
    snmpNotifyFilterStorageType."'snmpd'" = nonVolatile \
    snmpNotifyFilterProfileRowStatus."'snmpd'" = active

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

set -e  # exit on error

# Following is an example configuration of a named log for logging only linkUp and linkDown Notifications.
snmpset ${SNMP_HOST} \
    nlmConfigLogEntryStatus.'"links"' = destroy || echo ignored
snmpset ${SNMP_HOST} \
    nlmConfigLogFilterName.'"links"'  = '"link-status"' \
    nlmConfigLogEntryLimit.'"links"'  = 4 \
    nlmConfigLogAdminStatus.'"links"' = enabled \
    nlmConfigLogStorageType.'"links"' = nonVolatile \
    nlmConfigLogEntryStatus.'"links"' = createAndGo

############################# following commands must be OK #############################

snmpset ${SNMP_HOST} nlmConfigLogEntryLimit.0 = 4 || echo "OK, readonly!"

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 10 \
    NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 3 # minutes

snmpset ${SNMP_HOST} nlmConfigLogEntryStatus.'"testGroup"' = destroy || echo ignored
#XXX NOTIFICATION-LOG-MIB::nlmConfigLogFilterName.'"testGroup"' = ${myindex} \
snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.'"testGroup"' = createAndWait \
    NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus.'"testGroup"' = enabled \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.'"testGroup"' = 4 \
    NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.'"testGroup"' = nonVolatile \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.'"testGroup"' = active

snmpwalk ${SNMP_HOST} nlmConfigGlobal
snmpwalk ${SNMP_HOST} nlmStats
snmpwalk ${SNMP_HOST} nlmConfigLogOperStatus
snmptable -Cib ${SNMP_HOST} nlmConfigLogTable

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

for Table in ${MIBTABLES}; do snmptable -Cib ${SNMP_HOST} ${Table}; done

snmpwalk ${SNMP_HOST} nlmStats

# + set -u
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterProfileRowStatus.'\''snmpd'\''' = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileRowStatus.'snmpd' = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."ABCDEF".1.3.6.1.2.1.4.21.1 = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterProfileRowStatus.'\''snmpd'\''' = createAndWait 'snmpNotifyFilterProfileName.'\''snmpd'\''' = 6.65.66.67.68.69.70 'snmpNotifyFilterStorageType.'\''snmpd'\''' = nonVolatile 'snmpNotifyFilterProfileRowStatus.'\''snmpd'\''' = active
# snmpNotifyFilterStorageType.'snmpd': Unknown Object Identifier ('-quote is for fixed length strings)
# + echo Error ignored
# Error ignored
# + snmpset -v2c -c public -r 0 localhost:4700 snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = createAndWait snmpNotifyFilterMask.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = fffb snmpNotifyFilterType.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = excluded snmpNotifyFilterStorageType.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = nonVolatile snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.1.3.6.1.2.1.4.21.1 = active
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."ABCDEF".1.3.6.1.2.1.4.21.1 = INTEGER: createAndWait(5)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterMask."ABCDEF".1.3.6.1.2.1.4.21.1 = STRING: "fffb"
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterType."ABCDEF".1.3.6.1.2.1.4.21.1 = INTEGER: excluded(2)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterStorageType."ABCDEF".1.3.6.1.2.1.4.21.1 = INTEGER: nonVolatile(3)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."ABCDEF".1.3.6.1.2.1.4.21.1 = INTEGER: active(1)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.3' = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.3 = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.4' = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.4 = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterMask."link-status".1.3.6.1.6.3.1.1.5.3' = '' 'snmpNotifyFilterType."link-status".1.3.6.1.6.3.1.1.5.3' = included 'snmpNotifyFilterStorageType."link-status".1.3.6.1.6.3.1.1.5.3' = nonVolatile 'snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.3' = createAndGo
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterMask."link-status".1.3.6.1.6.3.1.1.5.3 = ""
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterType."link-status".1.3.6.1.6.3.1.1.5.3 = INTEGER: included(1)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterStorageType."link-status".1.3.6.1.6.3.1.1.5.3 = INTEGER: nonVolatile(3)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.3 = INTEGER: createAndGo(4)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterMask."link-status".1.3.6.1.6.3.1.1.5.4' = '' 'snmpNotifyFilterType."link-status".1.3.6.1.6.3.1.1.5.4' = included 'snmpNotifyFilterStorageType."link-status".1.3.6.1.6.3.1.1.5.4' = nonVolatile 'snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.4' = createAndGo
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterMask."link-status".1.3.6.1.6.3.1.1.5.4 = ""
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterType."link-status".1.3.6.1.6.3.1.1.5.4 = INTEGER: included(1)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterStorageType."link-status".1.3.6.1.6.3.1.1.5.4 = INTEGER: nonVolatile(3)
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus."link-status".1.3.6.1.6.3.1.1.5.4 = INTEGER: createAndGo(4)
# + snmpset -v2c -c public -r 0 localhost:4700 'nlmConfigLogEntryStatus."links"' = destroy
# NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus."links" = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'nlmConfigLogFilterName."links"' = '"link-status"' 'nlmConfigLogEntryLimit."links"' = 0 'nlmConfigLogAdminStatus."links"' = enabled 'nlmConfigLogStorageType."links"' = nonVolatile 'nlmConfigLogEntryStatus."links"' = createAndGo
# Error in packet.
# Reason: inconsistentValue (The set value is illegal or unsupported in some way)
# Failed object: NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit."links"
#
# + set -e
# + snmpset -v2c -c public -r 0 localhost:4700 nlmConfigLogEntryLimit.0 = 4
# Error in packet.
# Reason: inconsistentValue (The set value is illegal or unsupported in some way)
# Failed object: NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.""
#
# + echo 'OK, readonly!'
# OK, readonly!
# + snmpset -v2c -c public -r 0 localhost:4700 NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 10 NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 1
# NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = Gauge32: 10
# NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = Gauge32: 1 minutes
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmConfigGlobal
# NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = Gauge32: 1 minutes
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmStats
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 1 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 0 notifications
# NOTIFICATION-LOG-MIB::nlmStatsLogNotificationsLogged."" = Counter32: 1 notifications
# + snmpgetnext -v2c -c public -r 0 localhost:4700 nlmConfigLogOperStatus
# NOTIFICATION-LOG-MIB::nlmConfigLogOperStatus."" = INTEGER: noFilter(3)
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmConfigLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmConfigLogTable
#
#  index FilterName                                      EntryLimit AdminStatus OperStatus StorageType EntryStatus
#     ""            Wrong Type (should be Gauge32 or Unsigned32): 0     enabled   noFilter    readOnly      active
# + snmpset -v2c -c public -r 0 localhost:4700 AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
# AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = INTEGER: agentppNotifyTestAllTypes(1)
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmLogVariableTable
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.0 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.1 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.2 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.3 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.4 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.5 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.6 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOidVal."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.7 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID."".2.8 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal."".1
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.0 = INTEGER: counter32(1)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.1 = INTEGER: unsigned32(2)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.2 = INTEGER: timeTicks(3)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.3 = INTEGER: integer32(4)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.4 = INTEGER: octetString(6)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.5 = INTEGER: ipAddress(5)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.6 = INTEGER: objectId(7)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.7 = INTEGER: counter64(8)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType."".2.8 = INTEGER: opaque(9)
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val."".2.0 = Counter32: 719885386
# NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val."".2.1 = Gauge32: 1649760492
# NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal."".2.2 = Timeticks: (596516649) 69 days, 0:59:26.49
# NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val."".2.3 = INTEGER: 1189641421
# NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal."".2.4 = Hex-STRING: 90 6E
# NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal."".2.5 = IpAddress: 162.234.243.131
# NOTIFICATION-LOG-MIB::nlmLogVariableOidVal."".2.6 = OID: SNMPv2-SMI::mib-2.340775556.0
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val."".2.7 = Counter64: 1306053050348102338
# NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal."".2.8 = OPAQUE: 90 6E
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmStats
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 2 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 0 notifications
# NOTIFICATION-LOG-MIB::nlmStatsLogNotificationsLogged."" = Counter32: 2 notifications
# ++ snmpget -OnqUv -v2c -c public -r 0 localhost:4700 nlmStatsGlobalNotificationsLogged.0
# + nlmStatsGlobalNotificationsLogged=2
# ++ snmpget -OnqUv -v2c -c public -r 0 localhost:4700 snmpOutTraps.0
# + snmpOutTraps=2
# + test 2 -eq 2
# + MIBTABLES='snmpTargetParamsTable snmpTargetAddrTable
#   snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable
#   nlmStatsLogTable nlmLogTable nlmConfigLogTable nlmLogVariableTable
# '
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetParamsTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetParamsTable
#
#           index MPModel SecurityModel SecurityName SecurityLevel StorageType RowStatus
# 'defaultV2Trap'       1             2       public  noAuthNoPriv nonVolatile    active
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetAddrTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetAddrTable
#
#           index                  TDomain             TAddress Timeout RetryCount TagList        Params StorageType RowStatus
# '127.0.0.1/162' SNMPv2-TM::snmpUDPDomain "7F 00 00 01 00 A2 "    1500          3  v2trap defaultV2Trap nonVolatile    active
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyTable
#
#           index    Tag Type StorageType RowStatus
# 'defaultV2Trap' v2trap trap nonVolatile    active
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterProfileTable
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileTable: No entries
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyFilterTable
#
#                             index   Mask     Type StorageType RowStatus
#       "ABCDEF".1.3.6.1.2.1.4.21.1 "fffb" excluded nonVolatile    active
# "link-status".1.3.6.1.6.3.1.1.5.3     "" included nonVolatile    active
# "link-status".1.3.6.1.6.3.1.1.5.4     "" included nonVolatile    active
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmStatsLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmStatsLogTable
#
#  index          Logged Bumped
#     "" 2 notifications 0 notifications
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogTable
#
#  index         Time              DateAndTime                                        EngineID       EngineTAddress            EngineTDomain ContextEngineID ContextName                                    NotificationID
#   "".1 0:0:00:00.08 2023-4-12,18:58:1.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C " "7F 00 00 01 00 A2 " SNMPv2-TM::snmpUDPDomain              ""                                         SNMPv2-MIB::coldStart
#   "".2 0:0:00:11.09 2023-4-12,18:58:1.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C " "7F 00 00 01 00 A2 " SNMPv2-TM::snmpUDPDomain              ""             AGENTPP-NOTIFYTEST-MIB::agentppNotifyTestAllTypes
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmConfigLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmConfigLogTable
#
#  index FilterName                                      EntryLimit AdminStatus OperStatus StorageType EntryStatus
#     ""            Wrong Type (should be Gauge32 or Unsigned32): 0     enabled   noFilter    readOnly      active
# + for Table in ${MIBTABLES}
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogVariableTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogVariableTable
#
#  index                                                      ID   ValueType Counter32Val Unsigned32Val  TimeTicksVal Integer32Val OctetStringVal    IpAddressVal                        OidVal        Counter64Val OpaqueVal
# "".2.0   NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val."".1   counter32    719885386             ?             ?            ?              ?               ?                             ?                   ?         ?
# "".2.1  NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val."".1  unsigned32            ?    1649760492             ?            ?              ?               ?                             ?                   ?         ?
# "".2.2   NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal."".1   timeTicks            ?             ? 69:0:59:26.49            ?              ?               ?                             ?                   ?         ?
# "".2.3   NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val."".1   integer32            ?             ?             ?   1189641421              ?               ?                             ?                   ?         ?
# "".2.4 NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal."".1 octetString            ?             ?             ?            ?       "90 6E "               ?                             ?                   ?         ?
# "".2.5   NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal."".1   ipAddress            ?             ?             ?            ?              ? 162.234.243.131                             ?                   ?         ?
# "".2.6         NOTIFICATION-LOG-MIB::nlmLogVariableOidVal."".1    objectId            ?             ?             ?            ?              ?               ? SNMPv2-SMI::mib-2.340775556.0                   ?         ?
# "".2.7   NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val."".1   counter64            ?             ?             ?            ?              ?               ?                             ? 1306053050348102338         ?
# "".2.8      NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal."".1      opaque            ?             ?             ?            ?              ?               ?                             ?                   ?    90 6E
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmStats
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 2 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 0 notifications
# NOTIFICATION-LOG-MIB::nlmStatsLogNotificationsLogged."" = Counter32: 2 notifications
