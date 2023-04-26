#!/bin/bash
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
#XXX SNMP_HOST='-r0 -v3 -u clausklein -l noAuthNoPriv --defSecurityModel=usm 'tcp6:[::1]:161''

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
# snmptable -Cib ${SNMP_HOST} SNMP-NOTIFICATION-MIB::snmpNotifyTable
# snmptable -Cib ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetAddrTable
# snmptable -Cib ${SNMP_HOST} SNMP-TARGET-MIB::snmpTargetParamsTable

# 5. Set to 1 to force the agent to save it's persistent data immediately.
#
set +x
echo "save all this setting:"
snmpset ${SNMP_HOST} SNMPv2-MIB::snmpEnableAuthenTraps.0 = enabled || echo ignored
snmpset ${SNMP_HOST}  UCD-SNMP-MIB::versionSavePersistentData.0 = 1 || echo ignored

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
    snmptable -Cibw 135 ${SNMP_HOST} $Table;
    printf '\n--------------------------\n\n'; done 2>&1 # |
    # perl -n -e 'if (/(^SNMP table.*$)/) {print "$1::\n";} else {if (length() > 1) {print "\t$_";} else {print;} }'

#####
exit
#####

MIBTABLES="nlmConfigLogTable nlmStatsLogTable nlmLogTable nlmLogVariableTable"
for Table in $MIBTABLES; do
    snmptable -n "snmptrapd" -Cibw 135 ${SNMP_HOST} $Table;
    printf '\n--------------------------\n\n'; done 2>&1 # |
    # perl -n -e 'if (/(^SNMP table.*$)/) {print "$1\"snmptrapd\"::\n";} else {if (length() > 1) {print "\t$_";} else {print;} }'

snmpwalk -n "snmptrapd" ${SNMP_HOST} nlmconf
snmpwalk -n "snmptrapd" ${SNMP_HOST} nlmstat

# vim:tabstop=4 shiftwidth=4 expandtab

