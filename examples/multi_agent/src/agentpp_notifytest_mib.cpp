/*_############################################################################
  _##
  _##  AGENT++ 4.5 - agentpp_notifytest_mib.cpp
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

//--AgentGen BEGIN=_BEGIN
//--AgentGen END

#include "agentpp_notifytest_mib.h"

static const char* loggerModuleName = "agent++.notifytest_mib";

/**
 *  generated by AgentGen 1.6.1 for AGENT++v3.4
 * Fri Jul 06 11:45:57 GMT+02:00 2001.
 */

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

//--AgentGen BEGIN=_INCLUDE
#include <stdlib.h>
#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
#endif
//--AgentGen END

/**
 *  agentppNotifyTest
 *
 */

agentppNotifyTest* agentppNotifyTest::instance = 0;

agentppNotifyTest::agentppNotifyTest(Mib* mib)
    : MibLeaf(oidAgentppNotifyTest, READWRITE, new SnmpInt32())
{
    // This leaf object is a singleton. In order to access it use
    // the static pointer agentppNotifyTest::instance.
    instance = this;

    //--AgentGen BEGIN=agentppNotifyTest::agentppNotifyTest
    this->mib = mib;
    //--AgentGen END
}

agentppNotifyTest::~agentppNotifyTest()
{

    //--AgentGen BEGIN=agentppNotifyTest::~agentppNotifyTest
    //--AgentGen END
}

int32_t agentppNotifyTest::get_state() { return (int32_t) * ((SnmpInt32*)value); }

void agentppNotifyTest::set_state(int32_t l)
{

    //--AgentGen BEGIN=agentppNotifyTest::set_state
    //--AgentGen END
    *((SnmpInt32*)value) = l;
}

int agentppNotifyTest::set(const Vbx& vb)
{

    //--AgentGen BEGIN=agentppNotifyTest::set
    int32_t v = 0;
    vb.get_value(v);
    switch (v)
    {
    case e_agentppNotifyTestAllTypes: send_agentppNotifyTestAllTypes(); break;
    }
    //--AgentGen END
    return MibLeaf::set(vb);
}

bool agentppNotifyTest::value_ok(const Vbx& vb)
{
    int32_t v = 0;
    vb.get_value(v);
    if ((v != 1)) return false;

    //--AgentGen BEGIN=agentppNotifyTest::value_ok
    //--AgentGen END
    return true;
}

int agentppNotifyTest::prepare_set_request(Request* req, int& ind)
{
    int status = 0;
    if ((status = MibLeaf::prepare_set_request(req, ind)) != SNMP_ERROR_SUCCESS) return status;

    //--AgentGen BEGIN=agentppNotifyTest::prepare_set_request
    //--AgentGen END
    return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppNotifyTest
void agentppNotifyTest::send_agentppNotifyTestAllTypes()
{
    Vbx* vbs = new Vbx[9];
    int  n   = 0;
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.4.0.1");
    vbs[n++].set_value(Counter32(rand()));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.5.0.1");
    vbs[n++].set_value(Gauge32(rand()));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.6.0.1");
    vbs[n++].set_value(TimeTicks(rand()));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.7.0.1");
    vbs[n++].set_value(SnmpInt32(rand()));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.8.0.1");
    OctetStr s;
    int      length = rand() / (RAND_MAX / 5);
    for (int i = 0; i < length; i++) { s += (unsigned char)(rand() / (RAND_MAX / 128) + 64); }
    vbs[n++].set_value(s);
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.9.0.1");
    char ipaddr[30];
    snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", rand() / (RAND_MAX / 256),
        rand() / (RAND_MAX / 256), rand() / (RAND_MAX / 256), rand() / (RAND_MAX / 256));
    vbs[n++].set_value(IpAddress(ipaddr));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.10.0.1");
    vbs[n++].set_value(Oid("1.3.6.1.2.1.340775556.0"));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.11.0.1");
    uint32_t low  = rand();
    uint32_t high = rand();
    vbs[n++].set_value(Counter64(high, low));
    vbs[n].set_oid("1.3.6.1.2.1.92.1.3.2.1.12.0.1");
    OpaqueStr op(s);
    vbs[n++].set_value(op);
    agentppNotifyTestAllTypes no(mib);
    no.generate(vbs, 9, "");
    for (int j = 0; j < 9; j++)
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
        LOG("Test notification vb sent (#)(oid)(val)(syntax)");
        LOG(j);
        LOG(vbs[j].get_printable_oid());
        LOG(vbs[j].get_printable_value());
        LOG(vbs[j].get_syntax());
        LOG_END;
    }
    delete[] vbs;
}
//--AgentGen END

/**
 *  agentppNotifyTestAllTypes
 *
 */

agentppNotifyTestAllTypes::agentppNotifyTestAllTypes(Mib* mib) : NotificationOriginator(mib)
{

    //--AgentGen BEGIN=agentppNotifyTestAllTypes::agentppNotifyTestAllTypes
    //--AgentGen END
}

agentppNotifyTestAllTypes::~agentppNotifyTestAllTypes()
{

    //--AgentGen BEGIN=agentppNotifyTestAllTypes::~agentppNotifyTestAllTypes
    //--AgentGen END
}

void agentppNotifyTestAllTypes::generate(Vbx* vbs, int sz, const OctetStr& context)
{

    //--AgentGen BEGIN=agentppNotifyTestAllTypes::generate
    //--AgentGen END
    if (sz < 9)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: too few var binds (given) (expected)");
        LOG(sz);
        LOG(9);
        LOG_END;
        return;
    }
    if (!(vbs[0].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.4")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(1l);
        LOG(vbs[0].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.4");
        LOG_END;
        return;
    }
    if (!(vbs[1].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.5")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(2l);
        LOG(vbs[1].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.5");
        LOG_END;
        return;
    }
    if (!(vbs[2].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.6")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(3l);
        LOG(vbs[2].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.6");
        LOG_END;
        return;
    }
    if (!(vbs[3].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.7")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(4l);
        LOG(vbs[3].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.7");
        LOG_END;
        return;
    }
    if (!(vbs[4].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.8")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(5l);
        LOG(vbs[4].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.8");
        LOG_END;
        return;
    }
    if (!(vbs[5].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.9")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(6l);
        LOG(vbs[5].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.9");
        LOG_END;
        return;
    }
    if (!(vbs[6].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.10")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(7l);
        LOG(vbs[6].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.10");
        LOG_END;
        return;
    }
    if (!(vbs[7].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.11")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(8l);
        LOG(vbs[7].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.11");
        LOG_END;
        return;
    }
    if (!(vbs[8].get_oid().in_subtree_of("1.3.6.1.2.1.92.1.3.2.1.12")))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("agentppNotifyTestAllTypes: wrong var bind (no.) (given) "
            "(expected)");
        LOG(9l);
        LOG(vbs[8].get_printable_oid());
        LOG("1.3.6.1.2.1.92.1.3.2.1.12");
        LOG_END;
        return;
    }
    mib->notify(context, oidAgentppNotifyTestAllTypes, vbs, sz);
}

//--AgentGen BEGIN=agentppNotifyTestAllTypes
//--AgentGen END

agentpp_notifytest_mib::agentpp_notifytest_mib(Mib* mib)
    : MibGroup("1.3.6.1.4.1.4976.6.2", "agentppNotifyTestMIB")
{

    //--AgentGen BEGIN=agentpp_notifytest_mib::agentpp_notifytest_mib
    //--AgentGen END
    add(new agentppNotifyTest(mib));

    //--AgentGen BEGIN=agentpp_notifytest_mib::agentpp_notifytest_mib:post
    //--AgentGen END
}

//--AgentGen BEGIN=agentpp_notifytest_mib
//--AgentGen END

//--AgentGen BEGIN=_END
#ifdef AGENTPP_NAMESPACE
}
#endif

//--AgentGen END
