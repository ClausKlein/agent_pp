/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - snmp_counters.cpp
 * _##
 * _##  Copyright (C) 2000-2021  Frank Fock and Jochen Katz (agentpp.com)
 * _##
 * _##  Licensed under the Apache License, Version 2.0 (the "License");
 * _##  you may not use this file except in compliance with the License.
 * _##  You may obtain a copy of the License at
 * _##
 * _##      http://www.apache.org/licenses/LICENSE-2.0
 * _##
 * _##  Unless required by applicable law or agreed to in writing, software
 * _##  distributed under the License is distributed on an "AS IS" BASIS,
 * _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * _##  See the License for the specific language governing permissions and
 * _##  limitations under the License.
 * _##
 * _##########################################################################*/

#include <agent_pp/snmp_counters.h>
#include <libagent.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

uint32_t MibIIsnmpCounters::counter_snmp[SNMP_COUNTERS];

MibIIsnmpCounters::MibIIsnmpCounters() { reset(); }

void MibIIsnmpCounters::reset()
{
    for (unsigned int& i : counter_snmp) { i = 0; }
}
