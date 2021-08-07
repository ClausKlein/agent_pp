/*_############################################################################
  _##
  _##  AGENT++ 4.5 - snmp_group.cpp
  _##
  _##  Copyright (C) 2000-2021  Frank Fock and Jochen Katz (agentpp.com)
  _##
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##
  _##########################################################################*/

#include <agent_pp/snmp_counters.h>
#include <agent_pp/snmp_group.h>
#include <libagent.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

/**
 *  snmpEnableAuthenTraps
 *
 */

snmpEnableAuthenTraps* snmpEnableAuthenTraps::instance = 0;

snmpEnableAuthenTraps::snmpEnableAuthenTraps()
    : MibLeaf(oidSnmpEnableAuthenTraps, READWRITE, new SnmpInt32(2))
{
    instance = this;
}

snmpEnableAuthenTraps::~snmpEnableAuthenTraps() { }

long snmpEnableAuthenTraps::get_state() { return (long)*((SnmpInt32*)value); }

bool snmpEnableAuthenTraps::value_ok(const Vbx& vb)
{
    int32_t v = 0;
    if (vb.get_value(v) != SNMP_CLASS_SUCCESS) return FALSE;
    if ((v != 1) && (v != 2)) return FALSE;
    return TRUE;
}

/**********************************************************************
 *
 *  class snmpGroup
 *
 **********************************************************************/

snmpGroup::snmpGroup() : MibGroup(oidSnmpGroup, "snmpGroup")
{
    MibIIsnmpCounters::reset();

    add(new snmpInPkts());
    add(new snmpOutPkts());
    add(new snmpInBadVersions());
    add(new snmpInBadCommunityNames());
    add(new snmpInBadCommunityUses());
    add(new snmpInASNParseErrs());
    add(new snmpInTooBigs());
    add(new snmpInNoSuchNames());
    add(new snmpInBadValues());
    add(new snmpInReadOnlys());
    add(new snmpInGenErrs());
    add(new snmpInTotalReqVars());
    add(new snmpInTotalSetVars());
    add(new snmpInGetRequests());
    add(new snmpInGetNexts());
    add(new snmpInSetRequests());
    add(new snmpInGetResponses());
    add(new snmpInTraps());
    add(new snmpOutTooBigs());
    add(new snmpOutNoSuchNames());
    add(new snmpOutBadValues());
    add(new snmpOutGenErrs());
    add(new snmpOutGetRequests());
    add(new snmpOutGetNexts());
    add(new snmpOutSetRequests());
    add(new snmpOutGetResponses());
    add(new snmpOutTraps());
    add(new snmpEnableAuthenTraps());
    add(new snmpSilentDrops());
    add(new snmpProxyDrops());
}
