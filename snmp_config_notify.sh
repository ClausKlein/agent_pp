#!/bin/sh
#
# Author:  Stephan Schloesser, Claus Klein
# Date:    $Date: 2005/10/14 15:44:15 $
# Version: $Revision: 1.4 $
#
# Description:
#   Configures a new notification receiver
#
#   The IP address of the notification receiver and other SNMPv3 parameters
#   to use in generating notifications are added to the appropriate MIB
#   tables.
#
#   MIBs used/needed:
#   - SNMP-NOTIFICATION-MIB [RFC 3413]
#   - SNMP-TARGET-MIB [RFC 3413]
#

: ${HOST:="$1"}
: ${HOST:="space_target"}
: ${HOST:="localhost:4700"}

## SNMP_PORT=4700
## SNMP_HOST="-r0 ${HOST}:${SNMP_PORT}"

test "${HOST}" != "space_target"  && : ${SNMP_VERSION:=2c -c public }
: ${SNMP_VERSION:="3 -u unsec -l noAuthNoPriv"}
## : ${SNMP_VERSION:="3 -u v3rwPriv -l authPriv -a MD5 -A snmpPriv -x DES -X snmpPriv"}
SNMP_HOST="-v${SNMP_VERSION} -r0 ${HOST}"

: ${TRAP_HOST:="snmpd"}
: ${TRAP_COMMUNITY:="trapuser"}


set -e  # exit on error
set -u  # exit on undefind vars

# GetHostByName and append udp port number:
###XXX### ping -c 1 ${TRAP_HOST}
#XXX UDPDomain=$(nslookup ${TRAP_HOST} | awk 'BEGIN { Address = 0 }; /Address:/ { Address = $2 }; END { print Address }' | awk -F . ' {printf "%02x%02x%02x%02x%04x\n", $1, $2, $3, $4, 162 }')
# DomainUdpIpv6=$(nslookup ${TRAP_HOST} | awk 'BEGIN { Address = 0 }; /Address:/ { Address = $2 }; END { print Address }' | awk -F . ' {printf "00000000000000000000FFFF%02x%02x%02x%02x%04x\n", $1, $2, $3, $4, 162 }')
#
# NOTE: host is not available on AXR! ck
### UDPDomain=$(host ${TRAP_HOST} | awk '{print $3 }' | awk -F . ' {printf "%02x%02x%02x%02x%04x\n", $1, $2, $3, $4, 162 }')

# DomainUdpIpv6='2::1:2945'
# DomainUdpIpv6='02 00 00 00 00 00 00 00 00 00 00 00 00 01 41 69  00 A2'
# DomainUdpIpv6='0002000000000000000000000001416900A2'

# OTAX_MANAGEMENT_IP_ADDRESS="0002:0000:0000:0000:0000:0000:0000:0102"
DomainUdpIpv6='0002000000000000000000000001010200A2'

set -x  # be verbose

###############################
# check the connectivity first:
###############################
### snmpget ${SNMP_HOST} SNMPv2-MIB::sysName.0
### snmpget -r 0 -v 2c -c ${TRAP_COMMUNITY} ${TRAP_HOST} SNMPv2-MIB::sysName.0 || echo "WARNING: notification receiver (snmpd) may not ready?"

# 1. SNMP-NOTIFICATION-MIB::snmpNotifyTable
#
if snmpget ${SNMP_HOST} SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus."'${TRAP_HOST}'" | egrep "No Such Instance" ; then
   snmpset ${SNMP_HOST} SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus."'${TRAP_HOST}'" = createAndWait
else
   snmpset ${SNMP_HOST} SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus."'${TRAP_HOST}'" = notInService || echo ignored
fi
snmpset ${SNMP_HOST} \
    SNMP-NOTIFICATION-MIB::snmpNotifyTag."'${TRAP_HOST}'" = "${TRAP_HOST}" \
    SNMP-NOTIFICATION-MIB::snmpNotifyType."'${TRAP_HOST}'" = trap
snmpset ${SNMP_HOST} \
    SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus."'${TRAP_HOST}'" = active

# 2. SNMP-TARGET-MIB::snmpTargetParamsTable
#
if snmpget ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetParamsRowStatus."'${TRAP_HOST}'" | egrep "No Such Instance" ; then
   snmpset ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetParamsRowStatus."'${TRAP_HOST}'" = createAndWait
else
   snmpset ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetParamsRowStatus."'${TRAP_HOST}'" = notInService || echo ignored
fi

# setup MPModel=SNMPv3 and SecurityModel=SNMPv3
snmpset ${SNMP_HOST} \
    SNMP-TARGET-MIB::snmpTargetParamsMPModel."'${TRAP_HOST}'" = 3 \
    SNMP-TARGET-MIB::snmpTargetParamsSecurityModel."'${TRAP_HOST}'" = 3 \
    SNMP-TARGET-MIB::snmpTargetParamsSecurityName."'${TRAP_HOST}'" = ${TRAP_COMMUNITY} \
    SNMP-TARGET-MIB::snmpTargetParamsSecurityLevel."'${TRAP_HOST}'" = noAuthNoPriv
snmpset ${SNMP_HOST} \
    SNMP-TARGET-MIB::snmpTargetParamsRowStatus."'${TRAP_HOST}'" = active

# 3. snmpTargetAddrTable
#
if snmpget ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetAddrRowStatus."'${TRAP_HOST}'" | egrep "No Such Instance" ; then
   snmpset ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetAddrRowStatus."'${TRAP_HOST}'" = createAndWait
else
   snmpset ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetAddrRowStatus."'${TRAP_HOST}'" = notInService || echo ignored
fi

#XXX snmpset ${SNMP_HOST} \
#XXX   SNMP-TARGET-MIB::snmpTargetAddrTDomain.\'${TRAP_HOST}\' = SNMPv2-SMI::snmpDomains.1 \
#XXX   SNMP-TARGET-MIB::snmpTargetAddrTAddress.\'${TRAP_HOST}\' x ${UDPDomain} \

snmpset ${SNMP_HOST} \
    SNMP-TARGET-MIB::snmpTargetAddrTDomain."'${TRAP_HOST}'" = TRANSPORT-ADDRESS-MIB::transportDomainUdpIpv6 \
    SNMP-TARGET-MIB::snmpTargetAddrTAddress."'${TRAP_HOST}'" x ${DomainUdpIpv6} \
    SNMP-TARGET-MIB::snmpTargetAddrTagList."'${TRAP_HOST}'" = "${TRAP_HOST}" \
    SNMP-TARGET-MIB::snmpTargetAddrParams."'${TRAP_HOST}'" = "${TRAP_HOST}" \
    SNMP-TARGET-MIB::snmpTargetAddrRowStatus."'${TRAP_HOST}'" = active

# 4. Show the related tables now
#
### set -x
# snmptable ${SNMP_HOST} SNMP-NOTIFICATION-MIB::snmpNotifyTable
# snmptable ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetAddrTable
# snmptable ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetParamsTable

# 5. Set to 1 to force the agent to save it's persistent data immediately.
#
set +x
echo "save all this setting:"
snmpset ${SNMP_HOST} SNMPv2-MIB::snmpEnableAuthenTraps.0 = enabled || echo ignored

echo "To see all info, do:"
echo "  snmpwalk ${SNMP_HOST} snmpv2"

snmpget ${SNMP_HOST} SNMPv2-MIB::snmpInBadCommunityNames.0 \
    SNMPv2-MIB::snmpOutTraps.0 SNMPv2-MIB::snmpEnableAuthenTraps.0
# SNMPv2-MIB::snmpInBadCommunityNames.0 = Counter32: 36
# SNMPv2-MIB::snmpOutTraps.0 = Counter32: 0
# SNMPv2-MIB::snmpEnableAuthenTraps.0 = INTEGER: enabled(1)
echo

MIBTABLES="snmpTargetAddrTable snmpTargetParamsTable snmpNotifyTable snmpNotifyFilterProfileTable snmpNotifyFilterTable"
for Table in $MIBTABLES; do
    snmptable -C biw 120 ${SNMP_HOST} $Table;
    printf '\n--------------------------\n\n'; done 2>&1 # |
    # perl -n -e 'if (/(^SNMP table.*$)/) {print "$1::\n";} else {if (length() > 1) {print "\t$_";} else {print;} }'

#####
exit
#####

MIBTABLES="nlmConfigLogTable nlmStatsLogTable nlmLogTable nlmLogVariableTable"
for Table in $MIBTABLES; do
    snmptable -n "snmptrapd" -C biw 120 ${SNMP_HOST} $Table;
    printf '\n--------------------------\n\n'; done 2>&1 # |
    # perl -n -e 'if (/(^SNMP table.*$)/) {print "$1\"snmptrapd\"::\n";} else {if (length() > 1) {print "\t$_";} else {print;} }'

snmpwalk -n "snmptrapd" ${SNMP_HOST} nlmconf
snmpwalk -n "snmptrapd" ${SNMP_HOST} nlmstat

# vim:tabstop=4 shiftwidth=4 expandtab

# ++ awk '/TTL/ {print $3 }'
# ++ awk -F . ' {printf "%02x%02x%02x%02x%04x\n", $1, $2, $3, $4, 162 }'
# + UDPDomain=c0a81ded00a2
# + snmpget -r0 -t3 space_target:161 SNMPv2-MIB::sysName.0
# SNMPv2-MIB::sysName.0 = STRING: WFEP_IFM_44_CCP
# + snmpget -r 0 -v 2c -c trapuser NMS SNMPv2-MIB::sysName.0
# Timeout: No Response from NMS.
# + echo 'WARNING: notification receiver (snmpd) may not ready?'
# WARNING: notification receiver (snmpd) may not ready?
# + set -x
# + snmpget -r0 -t3 space_target:161 'SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus.'\''NMS'\'''
# + egrep 'No Such Instance'
# + snmpset -r0 -t3 space_target:161 'SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus.'\''NMS'\''' = notInService
# SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus.\'NMS\' = INTEGER: notInService(2)
# + snmpset -r0 -t3 space_target:161 'SNMP-NOTIFICATION-MIB::snmpNotifyTag.'\''NMS'\''' = NMS 'SNMP-NOTIFICATION-MIB::snmpNotifyType.'\''NMS'\''' = trap
# SNMP-NOTIFICATION-MIB::snmpNotifyTag.\'NMS\' = STRING: NMS
# SNMP-NOTIFICATION-MIB::snmpNotifyType.\'NMS\' = INTEGER: trap(1)
# + snmpset -r0 -t3 space_target:161 'SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus.'\''NMS'\''' = active
# SNMP-NOTIFICATION-MIB::snmpNotifyRowStatus.\'NMS\' = INTEGER: active(1)
# + snmpget -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetAddrRowStatus.'\''NMS'\'''
# + egrep 'No Such Instance'
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetAddrRowStatus.'\''NMS'\''' = notInService
# SNMP-TARGET-MIB::snmpTargetAddrRowStatus.\'NMS\' = INTEGER: notInService(2)
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetAddrTDomain.'\''NMS'\''' = SNMPv2-SMI::snmpDomains.1 'SNMP-TARGET-MIB::snmpTargetAddrTAddress.'\''NMS'\''' x c0a81ded00a2 'SNMP-TARGET-MIB::snmpTargetAddrTagList.'\''NMS'\''' = NMS 'SNMP-TARGET-MIB::snmpTargetAddrParams.'\''NMS'\''' = NMS
# SNMP-TARGET-MIB::snmpTargetAddrTDomain.\'NMS\' = OID: SNMPv2-TM::snmpUDPDomain
# SNMP-TARGET-MIB::snmpTargetAddrTAddress.\'NMS\' = Hex-STRING: C0 A8 1D ED 00 A2
# SNMP-TARGET-MIB::snmpTargetAddrTagList.\'NMS\' = STRING: NMS
# SNMP-TARGET-MIB::snmpTargetAddrParams.\'NMS\' = STRING: NMS
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetAddrRowStatus.'\''NMS'\''' = active
# SNMP-TARGET-MIB::snmpTargetAddrRowStatus.\'NMS\' = INTEGER: active(1)
# + snmpget -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetParamsRowStatus.'\''NMS'\'''
# + egrep 'No Such Instance'
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetParamsRowStatus.'\''NMS'\''' i notInService
# SNMP-TARGET-MIB::snmpTargetParamsRowStatus.\'NMS\' = INTEGER: notInService(2)
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetParamsMPModel.'\''NMS'\''' = 3 'SNMP-TARGET-MIB::snmpTargetParamsSecurityModel.'\''NMS'\''' = 3 'SNMP-TARGET-MIB::snmpTargetParamsSecurityName.'\''NMS'\''' = trapuser 'SNMP-TARGET-MIB::snmpTargetParamsSecurityLevel.'\''NMS'\''' = noAuthNoPriv
# SNMP-TARGET-MIB::snmpTargetParamsMPModel.\'NMS\' = INTEGER: 3
# SNMP-TARGET-MIB::snmpTargetParamsSecurityModel.\'NMS\' = INTEGER: 3
# SNMP-TARGET-MIB::snmpTargetParamsSecurityName.\'NMS\' = STRING: trapuser
# SNMP-TARGET-MIB::snmpTargetParamsSecurityLevel.\'NMS\' = INTEGER: noAuthNoPriv(1)
# + snmpset -r0 -t3 space_target:161 'SNMP-TARGET-MIB::snmpTargetParamsRowStatus.'\''NMS'\''' = active
# SNMP-TARGET-MIB::snmpTargetParamsRowStatus.\'NMS\' = INTEGER: active(1)
# + set +x
# SNMPv2-MIB::snmpEnableAuthenTraps.0 = INTEGER: enabled(1)
# NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = Gauge32: 1
# Timeout: No Response from space_target:161.
# ignored
# To see all info, do:
#   snmpwalk -r0 -t3 space_target:161 .1.3.6.1.6.3
# SNMPv2-MIB::snmpInBadCommunityNames.0 = Counter32: 6
# SNMPv2-MIB::snmpOutTraps.0 = Counter32: 6
# SNMPv2-MIB::snmpEnableAuthenTraps.0 = INTEGER: enabled(1)
# 
# SNMP table: SNMP-TARGET-MIB::snmpTargetAddrTable::
# 
#           index                  TDomain             TAddress Timeout RetryCount TagList Params StorageType RowStatus
#         \'NMS\' SNMPv2-TM::snmpUDPDomain "C0 A8 1D ED 00 A2 "    1500          3     NMS    NMS nonVolatile    active
# 
#         --------------------------
# 
# SNMP table: SNMP-TARGET-MIB::snmpTargetParamsTable::
# 
#           index MPModel SecurityModel SecurityName SecurityLevel StorageType RowStatus
#         \'NMS\'       3             3     trapuser  noAuthNoPriv nonVolatile    active
# 
#         --------------------------
# 
# SNMP table: SNMP-NOTIFICATION-MIB::snmpNotifyTable::
# 
#           index Tag Type StorageType RowStatus
#         \'NMS\' NMS trap nonVolatile    active
# 
#         --------------------------
# 
#         SNMP-NOTIFICATION-MIB::snmpNotifyFilterProfileTable: No entries
# 
#         --------------------------
# 
#         SNMP-NOTIFICATION-MIB::snmpNotifyFilterTable: No entries
# 
#         --------------------------
# 
#         NOTIFICATION-LOG-MIB::nlmConfigLogTable: No entries
# 
#         --------------------------
# 
#         NOTIFICATION-LOG-MIB::nlmStatsLogTable: No entries
# 
#         --------------------------
# 
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogTable"snmptrapd"::
# 
#                 index         Time        DateAndTime EngineID EngineTAddress EngineTDomain ContextEngineID ContextName
#         \"default\".6 0:1:31:26.92 1970-1-1,1:31:28.0       ""              ?             ?              ""
# 
# SNMP table NOTIFICATION-LOG-MIB::nlmLogTable, part 2"snmptrapd"::
# 
#                 index                    NotificationID
#         \"default\".6 SNMPv2-MIB::authenticationFailure
# 
#         --------------------------
# 
# SNMP table: NOTIFICATION-LOG-MIB::nlmLogVariableTable"snmptrapd"::
# 
#                   index                                  ID ValueType Counter32Val Unsigned32Val TimeTicksVal Integer32Val
#         \"default\".6.1 DISMAN-EVENT-MIB::sysUpTimeInstance timeTicks            ?             ? 0:1:31:26.92            ?
#         \"default\".6.2    SNMPv2-MIB::snmpTrapEnterprise.0  objectId            ?             ?            ?            ?
# 
# SNMP table NOTIFICATION-LOG-MIB::nlmLogVariableTable, part 2"snmptrapd"::
# 
#                   index OctetStringVal IpAddressVal               OidVal Counter64Val OpaqueVal
#         \"default\".6.1              ?            ?                    ?            ?         ?
#         \"default\".6.2              ?            ? NET-SNMP-TC::unknown            ?         ?
# 
#         --------------------------
# 
# NOTIFICATION-LOG-MIB::nlmConfigGlobalEntryLimit.0 = Gauge32: 1
# NOTIFICATION-LOG-MIB::nlmConfigGlobalAgeOut.0 = Gauge32: 1440 minutes
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsLogged.0 = Counter32: 7 notifications
# NOTIFICATION-LOG-MIB::nlmStatsGlobalNotificationsBumped.0 = Counter32: 6 notifications
# 

# $ snmptable -Cib space_target snmpTargetAddrTable
# SNMP table: SNMP-TARGET-MIB::snmpTargetAddrTable
# 
#         index                                       TDomain                                                  TAddress Timeout RetryCount   TagList    Params StorageType RowStatus
#       \'NMS\'                      SNMPv2-TM::snmpUDPDomain                                      "7F 00 00 01 00 A2 "    1500          3       NMS       NMS nonVolatile    active
# \'internal0\' TRANSPORT-ADDRESS-MIB::transportDomainUdpIpv6 "00 00 00 00 00 00 00 00 00 00 FF FF 7F 00 00 01  A2 00 "    1000          5 internal0 internal0    readOnly    active
# 

