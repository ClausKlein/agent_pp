#!/bin/sh

# ('-quote is for fixed length strings)
# ("-quote is for variable length strings)
SNMP_PORT=4700
SNMP_TARGET=\'defaultV2Trap\'   # fix length octetstring
SNMP_HOST="-v2c -c public -r 0 localhost:${SNMP_PORT}"
myindex="6.65.66.67.68.69.70" # ABCDEF -> variable length octetstring

set -x  # be verbose
set -u

##############################################################
# active(1) notInService(2) notReady(3) createAndGo(4) createAndWait(5) destroy(6)
##############################################################

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus.${SNMP_TARGET} = destroy || echo ignored
snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = destroy || echo ignored
snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = destroy || echo ignored

############################# OK #############################

set -e  # exit on error

snmpset ${SNMP_HOST} snmpNotifyFilterProfileRowStatus.${SNMP_TARGET} = createAndWait \
    snmpNotifyFilterProfileName.${SNMP_TARGET} = ${myindex} \
    snmpNotifyFilterProfileRowStatus.${SNMP_TARGET} = active

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = 10 \
    NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = 4 # minutes

snmpwalk ${SNMP_HOST} nlmConfigGlobal
snmpwalk ${SNMP_HOST} nlmStats

##############################################################
#SVFUA-LOG-MIB::svfuaLogMgmtMaxSize.securityLog NOT allowed!
##############################################################
snmpset ${SNMP_HOST} snmpNotifyFilterRowStatus.${myindex}.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = createAndWait \
    snmpNotifyFilterMask.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = fffb \
    snmpNotifyFilterType.${myindex}.1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = excluded \
    snmpNotifyFilterRowStatus.${myindex}.2.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 = active || echo ignored

snmpset ${SNMP_HOST} NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = createAndWait \
    NOTIFICATION-LOG-MIB::nlmConfigLogFilterName.\"testGroup\" = ${myindex} \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryLimit.\"testGroup\" = 10 \
    NOTIFICATION-LOG-MIB::nlmConfigLogAdminStatus.\"testGroup\" = enabled \
    NOTIFICATION-LOG-MIB::nlmConfigLogStorageType.\"testGroup\" = nonVolatile \
    NOTIFICATION-LOG-MIB::nlmConfigLogEntryStatus.\"testGroup\" = active

snmpgetnext ${SNMP_HOST} nlmConfigLogOperStatus  
snmptable -Cib ${SNMP_HOST} nlmConfigLogTable 

snmpset ${SNMP_HOST} AGENTPP-NOTIFYTEST-MIB::agentppNotifyTest.0 = 1

MIBTABLES="snmpTargetParamsTable snmpTargetAddrTable 
  snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable
  nlmStatsLogTable nlmLogTable nlmLogVariableTable
"

for Table in ${MIBTABLES}; do snmptable -Cib ${SNMP_HOST} ${Table}; done

# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetParamsTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetParamsTable
# 
#           index MPModel SecurityModel SecurityName SecurityLevel StorageType RowStatus
# 'defaultV1Trap'       0             1       public  noAuthNoPriv nonVolatile    active
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpTargetAddrTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetAddrTable
# 
#           index                  TDomain             TAddress Timeout RetryCount TagList        Params StorageType RowStatus
# '127.0.0.1/162' SNMPv2-TM::snmpUDPDomain "7F 00 00 01 00 A2 "    1500          3  v1trap defaultV1Trap nonVolatile    active
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyTable
# 
#           index    Tag Type StorageType RowStatus
# 'defaultV1Trap' v1trap trap nonVolatile    active
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterProfileTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileTable
# 
#           index                Name    StorType RowStatus
# 'defaultV2Trap' 6.65.66.67.68.69.70 nonVolatile    active
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 snmpNotifyFilterTable
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyFilterTable
# 
#                                       index   Mask     Type StorageType RowStatus
# "ABCDEF".1.3.6.1.4.1.59999.33.1.3.1.2.1.2.2 "fffb" excluded nonVolatile    active
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmStatsLogTable
# NOTIFICATION-LOG-MIB::nlmStatsLogTable: No entries
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogTable
# 
#         index         Time              DateAndTime                                        EngineID       EngineTAddress            EngineTDomain                                 ContextEngineID ContextName                                    NotificationID
# "testGroup".1 0:0:00:00.10 2023-4-4,15:33:39.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "                   "" SNMPv2-TM::snmpUDPDomain "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "                                         SNMPv2-MIB::coldStart
# "testGroup".2 0:0:00:00.10 2023-4-4,15:33:39.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C " "7F 00 00 01 00 A2 " SNMPv2-TM::snmpUDPDomain "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "                                         SNMPv2-MIB::coldStart
# "testGroup".3 0:0:00:03.70 2023-4-4,15:33:39.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "                   "" SNMPv2-TM::snmpUDPDomain "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "             AGENTPP-NOTIFYTEST-MIB::agentppNotifyTestAllTypes
# "testGroup".4 0:0:00:03.70 2023-4-4,15:33:39.0,+2:0 "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C " "7F 00 00 01 00 A2 " SNMPv2-TM::snmpUDPDomain "80 00 13 70 05 57 44 30 30 33 35 32 30 12 5C "             AGENTPP-NOTIFYTEST-MIB::agentppNotifyTestAllTypes
# + snmptable -Cib -v2c -c public -r 0 localhost:4700 nlmLogVariableTable
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogVariableTable
# 
#           index                                                      ID   ValueType Counter32Val Unsigned32Val  TimeTicksVal Integer32Val OctetStringVal    IpAddressVal                        OidVal        Counter64Val OpaqueVal
# "testGroup".3.0   NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val."".1   counter32    719885386             ?             ?            ?              ?               ?                             ?                   ?         ?
# "testGroup".3.1  NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val."".1  unsigned32            ?    1649760492             ?            ?              ?               ?                             ?                   ?         ?
# "testGroup".3.2   NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal."".1   timeTicks            ?             ? 69:0:59:26.49            ?              ?               ?                             ?                   ?         ?
# "testGroup".3.3   NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val."".1   integer32            ?             ?             ?   1189641421              ?               ?                             ?                   ?         ?
# "testGroup".3.4 NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal."".1 octetString            ?             ?             ?            ?       "90 6E "               ?                             ?                   ?         ?
# "testGroup".3.5   NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal."".1   ipAddress            ?             ?             ?            ?              ? 162.234.243.131                             ?                   ?         ?
# "testGroup".3.6         NOTIFICATION-LOG-MIB::nlmLogVariableOidVal."".1    objectId            ?             ?             ?            ?              ?               ? SNMPv2-SMI::mib-2.340775556.0                   ?         ?
# "testGroup".3.7   NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val."".1   counter64            ?             ?             ?            ?              ?               ?                             ? 1306053050348102338         ?
# "testGroup".3.8      NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal."".1      opaque            ?             ?             ?            ?              ?               ?                             ?                   ?    90 6E
# "testGroup".4.0   NOTIFICATION-LOG-MIB::nlmLogVariableCounter32Val."".1   counter32    719885386             ?             ?            ?              ?               ?                             ?                   ?         ?
# "testGroup".4.1  NOTIFICATION-LOG-MIB::nlmLogVariableUnsigned32Val."".1  unsigned32            ?    1649760492             ?            ?              ?               ?                             ?                   ?         ?
# "testGroup".4.2   NOTIFICATION-LOG-MIB::nlmLogVariableTimeTicksVal."".1   timeTicks            ?             ? 69:0:59:26.49            ?              ?               ?                             ?                   ?         ?
# "testGroup".4.3   NOTIFICATION-LOG-MIB::nlmLogVariableInteger32Val."".1   integer32            ?             ?             ?   1189641421              ?               ?                             ?                   ?         ?
# "testGroup".4.4 NOTIFICATION-LOG-MIB::nlmLogVariableOctetStringVal."".1 octetString            ?             ?             ?            ?       "90 6E "               ?                             ?                   ?         ?
# "testGroup".4.5   NOTIFICATION-LOG-MIB::nlmLogVariableIpAddressVal."".1   ipAddress            ?             ?             ?            ?              ? 162.234.243.131                             ?                   ?         ?
# "testGroup".4.6         NOTIFICATION-LOG-MIB::nlmLogVariableOidVal."".1    objectId            ?             ?             ?            ?              ?               ? SNMPv2-SMI::mib-2.340775556.0                   ?         ?
# "testGroup".4.7   NOTIFICATION-LOG-MIB::nlmLogVariableCounter64Val."".1   counter64            ?             ?             ?            ?              ?               ?                             ? 1306053050348102338         ?
# "testGroup".4.8      NOTIFICATION-LOG-MIB::nlmLogVariableOpaqueVal."".1      opaque            ?             ?             ?            ?              ?               ?                             ?                   ?    90 6E
# klein_cl@WD003520:~/Workspace/cpp/agent_pp$

snmpwalk ${SNMP_HOST} nlmLogVariableTable
snmpwalk ${SNMP_HOST} nlmStats

