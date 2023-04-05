#!/bin/sh

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

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus.${myindex} = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus.\'defaultV2Trap\' = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = destroy || echo ignored
snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = destroy || echo ignored

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus.\'defaultV2Trap\' = createAndWait \
    snmpNotifyFilterProfileName.\'defaultV2Trap\' = ${myindex} \
    snmpNotifyFilterStorageType.\'defaultV2Trap\' = nonVolatile \
    snmpNotifyFilterProfileRowStatus.\'defaultV2Trap\' = active || echo Error ignored

##############################################################
# MgmtMaxSize.Log NOT allowed!
##############################################################
snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = createAndWait \
    snmpNotifyFilterMask.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = fffb \
    snmpNotifyFilterType.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = excluded \
    snmpNotifyStorageType.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = nonVolatile \
    snmpNotifyFilterRowStatus.${myindex}.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = active || echo Error ignored

set -e  # exit on error

############################# must be OK #############################

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 100 \
    NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 30 # minutes

snmpwalk ${SNMP_HOST} nlmConfigGlobal
snmpwalk ${SNMP_HOST} nlmStats

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = createAndWait \
    NOTIFICATION-LOG-MIB::nlmConfigLogFilterName.\"testGroup\" = ${myindex} \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.\"testGroup\" = 10 \
    NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus.\"testGroup\" = enabled \
    NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.\"testGroup\" = nonVolatile \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = active

snmpgetnext ${SNMP_HOST} nlmConfigLogOperStatus

snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1

snmpwalk ${SNMP_HOST} nlmLogVariableTable
snmpwalk ${SNMP_HOST} nlmStats

MIBTABLES="snmpTargetParamsTable snmpTargetAddrTable
  snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable
  nlmStatsLogTable nlmLogTable nlmConfigLogTable nlmLogVariableTable
"

for Table in ${MIBTABLES}; do snmptable -Cib ${SNMP_HOST} ${Table}; done

# + set -u
# + snmpset -v2c -c public -r 0 localhost:4700 snmpNotifyFilterProfileRowStatus.6.65.66.67.68.69.70 = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileRowStatus.\'.ABCDEF\' = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterProfileRowStatus.'\''defaultV2Trap'\''' = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileRowStatus.\'defaultV2Trap\' = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = destroy
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterRowStatus.\"ABCDEF\".1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus."testGroup"' = destroy
# NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = INTEGER: destroy(6)
# + snmpset -v2c -c public -r 0 localhost:4700 'snmpNotifyFilterProfileRowStatus.'\''defaultV2Trap'\''' = createAndWait 'snmpNotifyFilterProfileName.'\''defaultV2Trap'\''' = 6.65.66.67.68.69.70 'snmpNotifyFilterStorageType.'\''defaultV2Trap'\''' = nonVolatile 'snmpNotifyFilterProfileRowStatus.'\''defaultV2Trap'\''' = active
# snmpNotifyFilterStorageType.'defaultV2Trap': Unknown Object Identifier ('-quote is for fixed length strings)
# + echo Error ignored
# Error ignored
# + snmpset -v2c -c public -r 0 localhost:4700 snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = createAndWait snmpNotifyFilterMask.6.65.66.67.68.69.70.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = fffb snmpNotifyFilterType.6.65.66.67.68.69.70.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = excluded snmpNotifyStorageType.6.65.66.67.68.69.70.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = nonVolatile snmpNotifyFilterRowStatus.6.65.66.67.68.69.70.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = active
# snmpNotifyStorageType.6.65.66.67.68.69.70.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2: Unknown Object Identifier (Index out of range: 59999 (snmpNotifyName))
# + echo Error ignored
# Error ignored
# + set -e
# + snmpset -v2c -c public -r 0 localhost:4700 NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 100 NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 30
# NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = Gauge32: 100
# NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = Gauge32: 30 minutes
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmConfigGlobal
# NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = Gauge32: 30 minutes
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmStats
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 10 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 0 notifications
# + snmpset -v2c -c public -r 0 localhost:4700 'NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus."testGroup"' = createAndWait 'NOTIFICATION-LOG-MIB::nlmConfigLogFilterName."testGroup"' = 6.65.66.67.68.69.70 'NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit."testGroup"' = 10 'NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus."testGroup"' = enabled 'NOTIFICATION-LOG-MIB::nlmConfigLogStorageType."testGroup"' = nonVolatile 'NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus."testGroup"' = active
# NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = INTEGER: createAndWait(5)
# NOTIFICATION-LOG-MIB::nlmConfigLogFilterName.\"testGroup\" = STRING: 6.65.66.67.68.69.70
# NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.\"testGroup\" = Gauge32: 10
# NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus.\"testGroup\" = INTEGER: enabled(1)
# NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.\"testGroup\" = INTEGER: nonVolatile(3)
# NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = INTEGER: active(1)
# + snmpgetnext -v2c -c public -r 0 localhost:4700 nlmConfigLogOperStatus
# NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.\"testGroup\" = INTEGER: nonVolatile(3)
# + snmpset -v2c -c public -r 0 localhost:4700 AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1
# AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = INTEGER: agentppNotifyTestAllTypes(1)
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmLogVariableTable
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.0 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.1 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.2 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.3 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.4 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.5 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.6 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.7 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".1.8 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.0 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.1 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.2 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.3 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.4 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.5 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.6 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.7 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableID.\"testGroup\".2.8 = OID: NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"\".1
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.0 = INTEGER: counter32(1)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.1 = INTEGER: unsigned32(2)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.2 = INTEGER: timeTicks(3)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.3 = INTEGER: integer32(4)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.4 = INTEGER: octetString(6)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.5 = INTEGER: ipAddress(5)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.6 = INTEGER: objectId(7)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.7 = INTEGER: counter64(8)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".1.8 = INTEGER: opaque(9)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.0 = INTEGER: counter32(1)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.1 = INTEGER: unsigned32(2)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.2 = INTEGER: timeTicks(3)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.3 = INTEGER: integer32(4)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.4 = INTEGER: octetString(6)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.5 = INTEGER: ipAddress(5)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.6 = INTEGER: objectId(7)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.7 = INTEGER: counter64(8)
# NOTIFICATION-LOG-MIB::nlmLogVariableValueType.\"testGroup\".2.8 = INTEGER: opaque(9)
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"testGroup\".1.0 = Counter32: 997389814
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"testGroup\".2.0 = Counter32: 997389814
# NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"testGroup\".1.1 = Gauge32: 2020739063
# NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"testGroup\".2.1 = Gauge32: 2020739063
# NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"testGroup\".1.2 = Timeticks: (107554536) 12 days, 10:45:45.36
# NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"testGroup\".2.2 = Timeticks: (107554536) 12 days, 10:45:45.36
# NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"testGroup\".1.3 = INTEGER: 1635339425
# NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"testGroup\".2.3 = INTEGER: 1635339425
# NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"testGroup\".1.4 = Hex-STRING: A9 50 42
# NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"testGroup\".2.4 = Hex-STRING: A9 50 42
# NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"testGroup\".1.5 = IpAddress: 176.222.161.188
# NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"testGroup\".2.5 = IpAddress: 176.222.161.188
# NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"testGroup\".1.6 = OID: SNMPv2-SMI::mib-2.340775556.0
# NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"testGroup\".2.6 = OID: SNMPv2-SMI::mib-2.340775556.0
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"testGroup\".1.7 = Counter64: 9218371850371484900
# NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"testGroup\".2.7 = Counter64: 9218371850371484900
# NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"testGroup\".1.8 = OPAQUE: A9 50 42
# NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"testGroup\".2.8 = OPAQUE: A9 50 42
# + snmpwalk -v2c -c public -r 0 localhost:4700 nlmStats
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 12 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 0 notifications
# NOTIFICATION-LOG-MIB::nlmStatsLogNotificationsLogged.\"testGroup\" = Counter32: 2 notifications
# + MIBTABLES='snmpTargetParamsTable snmpTargetAddrTable
#   snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable
#   nlmStatsLogTable nlmLogTable nlmConfigLogTable nlmLogVariableTable
# '
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetParamsTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetParamsTable
#
#             index MPModel SecurityModel SecurityName SecurityLevel StorageType RowStatus
# \'defaultV2Trap\'       1             2       public  noAuthNoPriv nonVolatile    active
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetAddrTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetAddrTable
#
#             index                  TDomain             TAddress Timeout RetryCount TagList        Params StorageType RowStatus
# \'127.0.0.1/162\' SNMPv2-TM::snmpUDPDomain "7F 00 00 01 00 A2 "    1500          3  v2trap defaultV2Trap nonVolatile    active
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyTable
#
#             index    Tag Type StorageType RowStatus
# \'defaultV2Trap\' v2trap trap nonVolatile    active
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterProfileTable
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileTable: No entries
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterTable
# SNMP-NOTIFICATION-MIB::snmpNotifyFilterTable: No entries
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmStatsLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmStatsLogTable
#
#         index          Logged Bumped
# \"testGroup\" 2 notifications      ?
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogTable
#
#           index         Time            DateAndTime                                                                             EngineID       EngineTAddress            EngineTDomain                                                                      ContextEngineID ContextName                                    NotificationID
# \"testGroup\".1 0:0:22:30.79 2023-4-5,8:48:6.0,+1:0 "80 00 13 70 05 43 6C 61 75 73 2D 69 4D 61 63 2E  66 72 69 74 7A 2E 62 6F 78 12 5C "                   "" SNMPv2-TM::snmpUDPDomain "80 00 13 70 05 43 6C 61 75 73 2D 69 4D 61 63 2E  66 72 69 74 7A 2E 62 6F 78 12 5C "             AGENTPP-NOTIFYTEST-MIB::agentppNotifyTestAllTypes
# \"testGroup\".2 0:0:22:30.79 2023-4-5,8:48:6.0,+1:0 "80 00 13 70 05 43 6C 61 75 73 2D 69 4D 61 63 2E  66 72 69 74 7A 2E 62 6F 78 12 5C " "7F 00 00 01 00 A2 " SNMPv2-TM::snmpUDPDomain                                                                                   ""             AGENTPP-NOTIFYTEST-MIB::agentppNotifyTestAllTypes
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmConfigLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmConfigLogTable
#
#         index          FilterName EntryLimit AdminStatus OperStatus StorageType EntryStatus
# \"testGroup\" 6.65.66.67.68.69.70         10     enabled          ? nonVolatile      active
# + for Table in '${MIBTABLES}'
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogVariableTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogVariableTable
#
#             index                                                        ID   ValueType Counter32Val Unsigned32Val   TimeTicksVal Integer32Val OctetStringVal    IpAddressVal                        OidVal        Counter64Val OpaqueVal
# \"testGroup\".1.0   NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"\".1   counter32    997389814             ?              ?            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".1.1  NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"\".1  unsigned32            ?    2020739063              ?            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".1.2   NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"\".1   timeTicks            ?             ? 12:10:45:45.36            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".1.3   NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"\".1   integer32            ?             ?              ?   1635339425              ?               ?                             ?                   ?         ?
# \"testGroup\".1.4 NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"\".1 octetString            ?             ?              ?            ?    "A9 50 42 "               ?                             ?                   ?         ?
# \"testGroup\".1.5   NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"\".1   ipAddress            ?             ?              ?            ?              ? 176.222.161.188                             ?                   ?         ?
# \"testGroup\".1.6         NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"\".1    objectId            ?             ?              ?            ?              ?               ? SNMPv2-SMI::mib-2.340775556.0                   ?         ?
# \"testGroup\".1.7   NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"\".1   counter64            ?             ?              ?            ?              ?               ?                             ? 9218371850371484900         ?
# \"testGroup\".1.8      NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"\".1      opaque            ?             ?              ?            ?              ?               ?                             ?                   ? A9 50 42
# \"testGroup\".2.0   NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val.\"\".1   counter32    997389814             ?              ?            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".2.1  NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val.\"\".1  unsigned32            ?    2020739063              ?            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".2.2   NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal.\"\".1   timeTicks            ?             ? 12:10:45:45.36            ?              ?               ?                             ?                   ?         ?
# \"testGroup\".2.3   NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val.\"\".1   integer32            ?             ?              ?   1635339425              ?               ?                             ?                   ?         ?
# \"testGroup\".2.4 NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal.\"\".1 octetString            ?             ?              ?            ?    "A9 50 42 "               ?                             ?                   ?         ?
# \"testGroup\".2.5   NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal.\"\".1   ipAddress            ?             ?              ?            ?              ? 176.222.161.188                             ?                   ?         ?
# \"testGroup\".2.6         NOTIFICATION-LOG-MIB::nlmLogVariableOidVal.\"\".1    objectId            ?             ?              ?            ?              ?               ? SNMPv2-SMI::mib-2.340775556.0                   ?         ?
# \"testGroup\".2.7   NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val.\"\".1   counter64            ?             ?              ?            ?              ?               ?                             ? 9218371850371484900         ?
# \"testGroup\".2.8      NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal.\"\".1      opaque            ?             ?              ?            ?              ?               ?                             ?                   ? A9 50 42
