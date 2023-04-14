/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - snmp_target_mib.cpp
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

#include <agent_pp/List.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <libagent.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#ifndef _NO_LOGGING
static const char* loggerModuleName = "agent++.snmp_target_mib";
#endif

/**
 *  snmpTargetAddrTDomain
 *
 */

snmpTargetAddrTDomain::snmpTargetAddrTDomain(const Oidx& id)
    : MibLeaf(id, READCREATE, new Oid(), VMODE_LOCKED)
{ }

snmpTargetAddrTDomain::~snmpTargetAddrTDomain() { }

MibEntryPtr snmpTargetAddrTDomain::clone()
{
    MibEntryPtr other = new snmpTargetAddrTDomain(oid);

    ((snmpTargetAddrTDomain*)other)->replace_value(value->clone());
    ((snmpTargetAddrTDomain*)other)->set_reference_to_table(my_table);
    return other;
}

int snmpTargetAddrTDomain::get_state()
{
    uint32_t const len = ((Oid*)value)->len();

    if ((len != 7) && (len != 9))
    {
        return 0;
    }
    if (len == 7)
    {
        return (*(Oid*)value)[6];
    }
    return 100 + (*(Oid*)value)[8];
}

bool snmpTargetAddrTDomain::value_ok(const Vbx& vb)
{
    Oid val;

    if (vb.get_value(val) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
    LOG("snmpTargetAddrTDomain: checking (domain)");
    LOG(val.get_printable());
    LOG_END;

    // check for "old" TDomain values
    if (val.len() == 7)
    {
        if ((val[6] >= 1L) && (val[6] <= 5))
        {
            return true;
        }
    }
    // check for new (TRANSPORT_ADDRESS_MIB) TDomain values
    if (val.len() == 9)
    {
        if ((val[8] >= 1) && (val[8] <= 16))
        {
            return true;
        }
    }
    return false;
}

/**
 *  snmpTargetAddrTAddress
 *
 */

snmpTargetAddrTAddress::snmpTargetAddrTAddress(const Oidx& id)
    : MibLeaf(id, READCREATE, new OctetStr(""), VMODE_LOCKED)
{ }

snmpTargetAddrTAddress::snmpTargetAddrTAddress(const Oidx& id, mib_access a, OctetStr* s, int m)
    : MibLeaf(id, a, s, m)
{ }

snmpTargetAddrTAddress::~snmpTargetAddrTAddress() { }

MibEntryPtr snmpTargetAddrTAddress::clone()
{
    MibEntryPtr other = new snmpTargetAddrTAddress(oid);

    ((snmpTargetAddrTAddress*)other)->replace_value(value->clone());
    ((snmpTargetAddrTAddress*)other)->set_reference_to_table(my_table);
    return other;
}

int snmpTargetAddrTAddress::prepare_set_request(Request* req, int& ind)
{
    Vbx domain(my_row->get_nth(0)->get_value());

    for (int i = 0; i < req->subrequests(); i++)
    {
        if ((i != ind) && (req->get_oid(i) == my_row->get_nth(0)->get_oid()))
        {
            domain = req->get_value(i);
            break;
        }
    }
    uint32_t  state = 0;
    Vbx const vb(req->get_value(ind));
    OctetStr  val;
    if (vb.get_value(val) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    int const length = val.len();

    Oidx o;
    if (domain.get_value(o) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if ((o.len() != 7) && (o.len() != 9))
    {
        return SNMP_ERROR_INCONSIST_VAL;
    }
    if (o.len() == 7)
    {
        state = o[6];
    }
    else
    {
        state = o[8] + 100;
    }

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
    LOG("snmpTargetAddrTAddress: checking address (len)(type)");
    LOG(length);
    LOG(state);
    LOG_END;

    switch (state)
    {
    case 1: // UDP Address
    case 101: {
        if (length == 6)
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }

    case 102: {
        if (length == 18)
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }

    case 105: {
        if (length == 6)
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }

    case 2:
    case 3: { // OSI Address
        if ((length == 1) || ((length >= 4) && (length <= 85)))
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }

    case 4: {
        if ((length >= 3) && (length <= 99))
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }

    case 5: {
        if (length == 12)
        {
            return SNMP_ERROR_SUCCESS;
        }
        break;
    }
    }
    return SNMP_ERROR_INCONSIST_VAL;
}

UdpAddress* snmpTargetAddrTAddress::getUdpAddress()
{
    int const tdomain = ((snmpTargetAddrTDomain*)my_row->get_nth(0))->get_state();

    switch (tdomain)
    {
    case 1:
    case 101:
    case 102: {
        auto* address = new UdpAddress();
        *address      = (*(OctetStr*)value);
        return address;
    }
    }
    return nullptr;
}

/**
 *  snmpTargetAddrTimeout
 *
 */

/**
 *  snmpTargetAddrRetryCount
 *
 */

/**
 *  snmpTargetAddrParams
 *
 */

snmpTargetAddrParams::snmpTargetAddrParams(const Oidx& id)
    : MibLeaf(id, READCREATE, new OctetStr()) { }

snmpTargetAddrParams::~snmpTargetAddrParams() { }

MibEntryPtr snmpTargetAddrParams::clone()
{
    MibEntryPtr other = new snmpTargetAddrParams(oid);

    ((snmpTargetAddrParams*)other)->replace_value(value->clone());
    ((snmpTargetAddrParams*)other)->set_reference_to_table(my_table);
    return other;
}

int snmpTargetAddrParams::prepare_set_request(Request* req, int& ind)
{
    // place instrumentation code (manipulating "value") here
    int const status = MibLeaf::prepare_set_request(req, ind);

    if (status == SNMP_ERROR_SUCCESS)
    {
        OctetStr  newAdminString;
        Vbx const vb(req->get_value(ind));
        if (vb.get_value(newAdminString) != SNMP_CLASS_SUCCESS)
        {
            return SNMP_ERROR_WRONG_TYPE;
        }
        if (newAdminString.len() == 0)
        {
            return SNMP_ERROR_WRONG_LENGTH;
        }
        if (!snmpTargetParamsEntry::instance->contains(newAdminString))
        {
            return SNMP_ERROR_INCONSIST_VAL;
        }
    }
    return status;
}

bool snmpTargetAddrParams::value_ok(const Vbx& /*vb*/)
{
    // Vbx cvb(vb); // zero length should return wrong length!
    // if (strlen(cvb.get_printable_value()) == 0) return false;
    return true;
}

/**
 *  snmpTargetAddrRowStatus
 *
 */

/**
 *  snmpTargetParamsMPModel
 *
 */

/**
 *  snmpTargetParamsSecurityModel
 *
 */

/**
 *  snmpTargetParamsSecurityName
 *
 */

/**
 *  snmpTargetParamsSecurityLevel
 *
 */

/**
 *  snmpTargetParamsRowStatus
 *
 */

/**
 *  snmpTargetAddrEntry
 *
 */

snmpTargetAddrEntry* snmpTargetAddrEntry::instance = nullptr;

snmpTargetAddrEntry::snmpTargetAddrEntry() : StorageTable(oidSnmpTargetAddrEntry, iSnmpAdminString, 1)
{
    // This table object is a singleton. In order to access it use
    // the static pointer snmpTargetAddrEntry::instance.
    instance = this;

    add_col(new snmpTargetAddrTDomain("2"));
    add_col(new snmpTargetAddrTAddress("3"));
    add_col(new SnmpInt32MinMax("4", READCREATE, 1500, VMODE_DEFAULT | VMODE_LOCKED, 0, 2147483647));
    add_col(new SnmpInt32MinMax("5", READCREATE, 3, VMODE_DEFAULT | VMODE_LOCKED, 0, 255));
    add_col(new SnmpTagList("6"));
    add_col(new snmpTargetAddrParams("7"));
    add_storage_col(new StorageType("8", 3));
    add_col(new snmpRowStatus("9"));
}

snmpTargetAddrEntry::~snmpTargetAddrEntry() { instance = nullptr; }

void snmpTargetAddrEntry::set_row(MibTableRow* r, const Oidx& p0, const OctetStr& p1, int p2, int p3,
    const OctetStr& p4, const OctetStr& p5, int p6, int p7)
{
    r->get_nth(0)->replace_value(new Oidx(p0));
    r->get_nth(1)->replace_value(new OctetStr(p1));
    r->get_nth(2)->replace_value(new SnmpInt32(p2));
    r->get_nth(3)->replace_value(new SnmpInt32(p3));
    r->get_nth(4)->replace_value(new OctetStr(p4));
    r->get_nth(5)->replace_value(new OctetStr(p5));
    r->get_nth(6)->replace_value(new SnmpInt32(p6));
    r->get_nth(7)->replace_value(new SnmpInt32(p7));
}

MibTableRow* snmpTargetAddrEntry::add_entry(const OctetStr& name, const Oidx& tdomain,
    const OctetStr& taddress, const OctetStr& taglist, const OctetStr& params)
{
    Oidx const index = Oidx::from_string(name, false);

    start_synch();
    MibTableRow* r = find_index(index);
    if (r)
    {
        end_synch();
        return nullptr;
    }
    r = add_row(index);
    r->get_nth(0)->replace_value(new Oidx(tdomain));
    r->get_nth(1)->replace_value(new OctetStr(taddress));
    // leave default values untouched (timeout and retry count)
    r->get_nth(4)->replace_value(new OctetStr(taglist));
    r->get_nth(5)->replace_value(new OctetStr(params));
    // leave default values untouched (storage type)
    r->get_nth(7)->replace_value(new SnmpInt32(rowActive));
    end_synch();
    return r;
}

bool snmpTargetAddrEntry::refers_to(OctetStr& searchEntry)
{
    OidListCursor<MibTableRow> cur;

    start_synch();
    for (cur.init(&content); cur.get(); cur.next())
    {
        OctetStr entry;
        cur.get()->get_nth(5)->get_value(entry);

        if (strcmp(entry.get_printable_hex(), searchEntry.get_printable_hex()) == 0)
        {
            end_synch();
            return true;
        }
    }
    end_synch();
    return false;
}

Address* snmpTargetAddrEntry::get_address(MibTableRow* row)
{
    OctetStr addrStr;
    Oidx     domain;

    row->get_nth(0)->get_value(domain);
    row->get_nth(1)->get_value(addrStr);
    uint32_t const targetDomain = domain.last();
    switch (targetDomain)
    {
    case 1:
    case 101:
    case 102: {
        UdpAddress* address = ((snmpTargetAddrTAddress*)row->get_nth(1))->getUdpAddress();
        return address;
        // break;
    }

    default: {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
        LOG("snmpTargetAddrEntry: target (domain) not supported.");
        LOG(domain.get_printable());
        LOG_END;
    }
    }
    return nullptr;
}

#ifdef _SNMPv3
UTarget* snmpTargetAddrEntry::get_target(
    const OctetStr& entry, snmpTargetParamsEntry* paramInfo, int& secLevel)
{
    start_synch();
    MibTableRow* r = find_index(Oidx::from_string(entry, false));

    if ((!r) || (r->get_row_status()->get() != rowActive))
    {
        end_synch();

        LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
        LOG("snmpTargetAddrEntry: target addr (row) not found.");
        LOG(OctetStr(entry).get_printable());
        LOG((r) ? "no active row found" : "missing row");
        LOG_END;
        return nullptr;
    }
    Address* address = get_address(r);
    OctetStr params;
    r->get_nth(5)->get_value(params);
    end_synch();
    if (!address)
    {
        return nullptr;
    }
    auto* target = new UTarget(*address);
    delete address;

    if (!paramInfo->get_target_params(params, *target, secLevel))
    {
        delete target;
        return nullptr;
    }

    return target;
}

#endif

List<MibTableRow>* snmpTargetAddrEntry::get_rows_cloned_for_tag(const OctetStr& tag)
{
    const OctetStr&            myTag(tag);
    OidListCursor<MibTableRow> cur;
    auto*                      list = new List<MibTableRow>();

    start_synch();
    for (cur.init(&content); cur.get(); cur.next())
    {
        snmpRowStatus* status = cur.get()->get_row_status();
        if (((!status) || ((status) && (status->get() == rowActive)))
            && ((((SnmpTagList*)cur.get()->get_nth(4))->contains(myTag.get_printable()))))
        {
            list->add(new MibTableRow(*cur.get()));
        }
    }
    end_synch();
    return list;
}

bool snmpTargetAddrEntry::ready_for_service(Vbx* pvbs, int /*sz*/)
{
    OctetStr params;

    pvbs[5].get_value(params);
    if (params.len() == 0)
    {
        return false;
    }
    if (!snmpTargetParamsEntry::instance->contains(params))
    {
        return false;
    }
    return true;
}

/**
 *  snmpTargetParamsEntry
 *
 */

snmpTargetParamsEntry* snmpTargetParamsEntry::instance = nullptr;

snmpTargetParamsEntry::snmpTargetParamsEntry()
    : StorageTable(oidSnmpTargetParamsEntry, iSnmpAdminString, 1)
{
    // This table object is a singleton. In order to access it use
    // the static pointer snmpTargetParamsEntry::instance.
    instance = this;

    add_col(new SnmpMessageProcessingModel("2", READCREATE, 0, VMODE_DEFAULT | VMODE_LOCKED));
    add_col(new SnmpSecurityModel("3", READCREATE, 1, VMODE_DEFAULT | VMODE_LOCKED));
    add_col(new SnmpAdminString("4", READCREATE, new OctetStr(""), VMODE_LOCKED, 1, 32));
    add_col(new SnmpSecurityLevel("5", READCREATE, 1, VMODE_DEFAULT | VMODE_LOCKED));
    add_storage_col(new StorageType("6", 3));
    add_col(new snmpRowStatus("7"));
}

snmpTargetParamsEntry::~snmpTargetParamsEntry() { instance = nullptr; }

bool snmpTargetParamsEntry::contains(const OctetStr& name)
{
    const OctetStr&            cname(name);
    OidListCursor<MibTableRow> cur;

    for (cur.init(&content); cur.get(); cur.next())
    {
        Oidx const index = cur.get()->get_index();
        // cut length off
        OctetStr const adminString(index.as_string(true)); // TODO(CK): without length?

        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("snmpTargetParamsEntry: contains ");
        LOG(adminString.get_printable_hex());
        LOG(cname.get_printable_hex());
        LOG_END;
        if (strcmp(cname.get_printable_hex(), adminString.get_printable_hex()) == 0)
        {
            return true;
        }
    }
    return false;
}

MibTableRow* snmpTargetParamsEntry::add_entry(const OctetStr& name, const int mpModel,
    const int secModel, const OctetStr& secName, const int secLevel)
{
    Oidx const index = Oidx::from_string(name, false);

    start_synch();
    MibTableRow* r = find_index(index);
    if (r)
    {
        end_synch();
        return nullptr;
    }
    r = add_row(index);
    r->get_nth(0)->replace_value(new SnmpInt32(mpModel));
    r->get_nth(1)->replace_value(new SnmpInt32(secModel));
    r->get_nth(2)->replace_value(new OctetStr(secName));
    r->get_nth(3)->replace_value(new SnmpInt32(secLevel));
    // leave default value untouched (storage type)
    r->get_nth(5)->replace_value(new SnmpInt32(rowActive));
    end_synch();
    return r;
}

#ifdef _SNMPv3
bool snmpTargetParamsEntry::get_target_params(const OctetStr& param, UTarget& target, int& secLevel)
{
    start_synch();
    MibTableRow* paramsRow = find_index(Oidx::from_string(param, false));

    if ((!paramsRow) || (paramsRow->get_row_status()->get() != rowActive))
    {
        end_synch();

        LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
        LOG("snmpTargetParamsEntry: target addr parameter (row) not found.");
        LOG(OctetStr(param).get_printable());
        LOG((paramsRow) ? "no active row found" : "missing row");
        LOG_END;
        return false;
    }

    int      secModel = 0, mpModel = 0;
    OctetStr secName;

    paramsRow->get_nth(0)->get_value(mpModel);
    paramsRow->get_nth(1)->get_value(secModel);
    paramsRow->get_nth(2)->get_value(secName);
    paramsRow->get_nth(3)->get_value(secLevel);

    end_synch();

    switch (mpModel)
    {
    case 0: {
        target.set_version(version1);
        break;
    }

    case 1: {
        target.set_version(version2c);
        break;
    }

    case 2: {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
        LOG("snmpTargetParamsEntry: mpModel SNMPv2u/* not supported");
        LOG_END;

        return false;

        break;
    }

    case 3: {
        target.set_version(version3);
        break;
    }
    }
    target.set_security_model(secModel);
    target.set_security_name(secName);

    return true;
}

#endif

snmp_target_mib::snmp_target_mib() : MibGroup("1.3.6.1.6.3.12", "snmpTargetMIB")
{
    add(new TestAndIncr("1.3.6.1.6.3.12.1.1.0"));
    add(new snmpTargetAddrEntry());
    add(new snmpTargetParamsEntry());
}
