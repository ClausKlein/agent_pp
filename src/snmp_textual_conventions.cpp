/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - snmp_textual_conventions.cpp
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

#include "agent_pp/vacm.h"

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/system_group.h>
#include <algorithm> // std::min
#include <libagent.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#ifndef _NO_LOGGING
static const char* loggerModuleName = "agent++.snmp_textual_conventions";
#endif

/*--------------------- class snmpDisplayString -------------------------*/

SnmpDisplayString::SnmpDisplayString(const Oidx& id, mib_access a, OctetStr* s) : MibLeaf(id, a, s)
{
    min_size = 0;
    max_size = 255;
}

SnmpDisplayString::SnmpDisplayString(const Oidx& id, mib_access a, OctetStr* s, bool d)
    : MibLeaf(id, a, s, (d ? VMODE_DEFAULT : VMODE_LOCKED))
{
    min_size = 0;
    max_size = 255;
}

SnmpDisplayString::SnmpDisplayString(
    const Oidx& id, mib_access a, OctetStr* s, bool d, int min_sz, int max_sz)
    : MibLeaf(id, a, s, (d ? VMODE_DEFAULT : VMODE_LOCKED))
{
    min_size = min_sz;
    max_size = max_sz;
}

SnmpDisplayString::~SnmpDisplayString() { }

MibEntryPtr SnmpDisplayString::clone()
{
    MibEntryPtr other = new SnmpDisplayString(
        oid, access, dynamic_cast<OctetStr*>(value->clone()), (value_mode == VMODE_DEFAULT));

    (dynamic_cast<SnmpDisplayString*>(other))->set_reference_to_table(my_table);
    return other;
}

int SnmpDisplayString::prepare_set_request(Request* req, int& ind)
{
    int const s = MibLeaf::prepare_set_request(req, ind);

    if (s != SNMP_ERROR_SUCCESS)
    {
        return s;
    }
    OctetStr  ostr;
    Vbx const vb(req->get_value(ind));
    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if ((ostr.len() < min_size) || (ostr.len() > max_size))
    {
        return SNMP_ERROR_WRONG_LENGTH;
    }
    return SNMP_ERROR_SUCCESS;
}

bool SnmpDisplayString::value_ok(const Vbx& vb)
{
    OctetStr ostr;

    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }
    for (unsigned int i = 0; i < ostr.len(); i++)
    {
        if (ostr[i] > 127u)
        {
            return false;
        }
        // check for CR NULL or CR LF
        if (ostr[i] == '\r')
        {
            if (i + 1 == ostr.len())
            {
                return false;
            }
            if ((ostr[i + 1] != 0) && (ostr[i + 1] != '\n'))
            {
                return false;
            }
        }
    }
    return true;
}

/*--------------------- class SnmpAdminString -------------------------*/

SnmpAdminString::SnmpAdminString(const Oidx& id, mib_access a, OctetStr* s)
    : OctetStrMinMax(id, a, s, VMODE_NONE, 0, 255)
{ }

SnmpAdminString::SnmpAdminString(const Oidx& id, mib_access a, OctetStr* s, int m)
    : OctetStrMinMax(id, a, s, m, 0, 255)
{ }

SnmpAdminString::SnmpAdminString(
    const Oidx& id, mib_access a, OctetStr* s, int m, int min_sz, int max_sz)
    : OctetStrMinMax(id, a, s, m, min_sz, max_sz)
{ }

SnmpAdminString::~SnmpAdminString() { }

MibEntryPtr SnmpAdminString::clone()
{
    MibEntryPtr other = new SnmpAdminString(
        oid, access, dynamic_cast<OctetStr*>(value->clone()), value_mode, min, max);

    (dynamic_cast<SnmpAdminString*>(other))->set_reference_to_table(my_table);
    return other;
}

OctetStr SnmpAdminString::get() { return *(dynamic_cast<OctetStr*>(value)); }

/*--------------------- class SnmpEngineID -------------------------*/

SnmpEngineID::SnmpEngineID(const Oidx& id, mib_access a, OctetStr* s) : MibLeaf(id, a, s) { }

SnmpEngineID::SnmpEngineID(const Oidx& id, mib_access a, OctetStr* s, int d) : MibLeaf(id, a, s, d) { }

SnmpEngineID::~SnmpEngineID() { }

MibEntryPtr SnmpEngineID::clone()
{
    MibEntryPtr other =
        new SnmpEngineID(oid, access, dynamic_cast<OctetStr*>(value->clone()), value_mode);

    (dynamic_cast<SnmpEngineID*>(other))->set_reference_to_table(my_table);
    return other;
}

int SnmpEngineID::prepare_set_request(Request* req, int& ind)
{
    int const s = MibLeaf::prepare_set_request(req, ind);

    if (s != SNMP_ERROR_SUCCESS)
    {
        return s;
    }

    OctetStr  ostr;
    Vbx const vb(req->get_value(ind));
    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if ((ostr.len() < 5) || (ostr.len() > 32))
    {
        return SNMP_ERROR_WRONG_LENGTH;
    }
    return SNMP_ERROR_SUCCESS;
}

OctetStr SnmpEngineID::create_engine_id(const OctetStr& userText)
{
    // 8 = v3EngineID, 1370h = 4976 = AGENT++ enterprise ID
    OctetStr engineID((const unsigned char*)"\x80\x00\x13\x70\x05", 5);

    engineID += userText;
    return engineID;
}

OctetStr SnmpEngineID::create_engine_id(unsigned short p)
{
    // 8 = v3EngineID, 1370h = 4976 = AGENT++ enterprise ID
    OctetStr      engineID((const unsigned char*)"\x80\x00\x13\x70\x05", 5);
    unsigned char port[3] {};

    port[0] = static_cast<uint8_t>(p / 256);
    port[1] = static_cast<uint8_t>(p % 256);
    port[2] = 0;
    constexpr size_t LEN { 255 };
    constexpr size_t MAX_LEN { 23 };
    char             hname[LEN];
    if (gethostname(hname, LEN) == 0)
    {
        OctetStr const host((const unsigned char*)hname, std::min(strlen(hname), MAX_LEN));
        engineID += OctetStr(host);
        engineID += OctetStr(port, 2);
    }
    else
    {
        time_t const   ct = time(nullptr);
        char*          tp = ctime(&ct); // TODO(CK): use ctime_s()!
        OctetStr const t((const unsigned char*)tp, std::min(strlen(hname), MAX_LEN));
        engineID += t;
        engineID += OctetStr(port, 2);
    }
    return engineID;
}

/*--------------------------- class snmpSpinLock -------------------------*/

/**
 *  SnmpTagValue
 *
 */

SnmpTagValue::SnmpTagValue(const Oidx& id) : MibLeaf(id, READCREATE, new OctetStr(""), VMODE_DEFAULT)
{ }

SnmpTagValue::SnmpTagValue(const Oidx& id, mib_access a, OctetStr* v, int m) : MibLeaf(id, a, v, m) { }

SnmpTagValue::~SnmpTagValue() { }

MibEntryPtr SnmpTagValue::clone()
{
    MibEntryPtr other = new SnmpTagValue(oid);

    (dynamic_cast<SnmpTagValue*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpTagValue*>(other))->set_reference_to_table(my_table);
    return other;
}

bool SnmpTagValue::value_ok(const Vbx& vb)
{
    OctetStr ostr;

    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }
    int const length = ostr.len();
    if (length == 0)
    {
        return true;
    }
    if ((length < 0) || (length > 255))
    {
        return false;
    }

    for (int i = 0; i < length; i++)
    {
        if (is_delimiter(ostr[i]))
        {
            return false;
        }
    }
    return true;
}

bool SnmpTagValue::is_delimiter(char c) { return (c == 32) || (c == 9) || (c == 13) || (c == 11); }

int SnmpTagValue::prepare_set_request(Request* req, int& ind)
{
    int const s = MibLeaf::prepare_set_request(req, ind);

    if (s != SNMP_ERROR_SUCCESS)
    {
        return s;
    }

    OctetStr  ostr;
    Vbx const vb(req->get_value(ind));
    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if (ostr.len() > 255)
    {
        return SNMP_ERROR_WRONG_LENGTH;
    }
    return SNMP_ERROR_SUCCESS;
}

/**
 *  SnmpTagList
 *
 */

SnmpTagList::SnmpTagList(const Oidx& id, mib_access a, OctetStr* v, int m) : MibLeaf(id, a, v, m) { }

SnmpTagList::SnmpTagList(const Oidx& id) : MibLeaf(id, READCREATE, new OctetStr(""), VMODE_DEFAULT) { }

SnmpTagList::~SnmpTagList() { }

MibEntryPtr SnmpTagList::clone()
{
    MibEntryPtr other = new SnmpTagList(oid, access, nullptr, get_value_mode());

    (dynamic_cast<SnmpTagList*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpTagList*>(other))->set_reference_to_table(my_table);
    return other;
}

bool SnmpTagList::value_ok(const Vbx& vb)
{
    OctetStr ostr;

    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }

    // pointer into ostr!
    char* s = (char*)ostr.data();
    if (s)
    {
        int const length = ostr.len();
        if (length > 255)
        {
            return false;
        }

        if ((length > 0) && (SnmpTagValue::is_delimiter(s[0])))
        {
            return false;
        }
        if ((length > 0) && (SnmpTagValue::is_delimiter(s[length - 1])))
        {
            return false;
        }
        for (int i = 0; i < length; i++)
        {
            if ((SnmpTagValue::is_delimiter(s[i]))
                && ((i + 1 < length) && (SnmpTagValue::is_delimiter(s[i + 1]))))
            {
                return false;
            }
        }
    }
    return true;
}

bool SnmpTagList::contains(const char* tag)
{
    if (!tag)
    {
        return false;
    }

    int const len = (dynamic_cast<OctetStr*>(value))->len(); // NOTE: without \0! CK
    char*     l   = new char[len + 1];                       // TODO(CK): use std::array<char>
    memcpy(l, (char*)(dynamic_cast<OctetStr*>(value))->data(), len);
    l[len] = 0;                                              // OK, CK

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 10);
    LOG("SnmpTagList: contains: (taglist)(tag)");
    LOG(l);
    LOG(tag);
    LOG_END;

    char* start = l;
    while ((l + strlen(l) - start >= (int)strlen(tag)) && (start = strstr(start, tag)))
    {
        if (((start == l) || (SnmpTagValue::is_delimiter(*(start - 1))))
            && ((l + strlen(l) - start == (int)strlen(tag))
                || (SnmpTagValue::is_delimiter(*(start + strlen(tag))))))
        {
            delete[] l;
            return true;
        }
        start++;
    }
    delete[] l;
    return false;
}

/**
 *  TestAndIncr
 *
 */

TestAndIncr::TestAndIncr(const Oidx& o) : MibLeaf(o, READWRITE, new SnmpInt32(0)) { }

TestAndIncr::~TestAndIncr() { }

int32_t TestAndIncr::get_state() { return (int32_t) * (dynamic_cast<SnmpInt32*>(value)); }

void TestAndIncr::set_state(int32_t l) { *(dynamic_cast<SnmpInt32*>(value)) = l; }

int TestAndIncr::set(const Vbx& vb)
{
    // place code for handling operations triggered
    // by this set here
    int const status = MibLeaf::set(vb);

    if (get_state() == 2147483647)
    {
        set_state(0);
    }
    else
    {
        set_state(get_state() + 1);
    }
    return status;
}

int TestAndIncr::prepare_set_request(Request* req, int& reqind)
{
    int32_t v = 0;

    if (req->get_value(reqind).get_value(v) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if (v != get_state())
    {
        return SNMP_ERROR_INCONSIST_VAL;
    }
    return SNMP_ERROR_SUCCESS;
}

bool TestAndIncr::value_ok(const Vbx& vb)
{
    int32_t v = 0;

    if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }
    if ((v < 0) || (v > 2147483647))
    {
        return false;
    }
    return true;
}

/*--------------------------- class storageType -------------------------*/

StorageType::StorageType(const Oidx& o, int def)
    : MibLeaf(o, READCREATE, new SnmpInt32(def), VMODE_DEFAULT)
{ }

MibEntryPtr StorageType::clone()
{
    MibEntryPtr other = new StorageType(oid, get_state());

    (dynamic_cast<StorageType*>(other))->replace_value(value->clone());
    (dynamic_cast<StorageType*>(other))->set_reference_to_table(my_table);
    return other;
}

bool StorageType::value_ok(const Vbx& vb)
{
    int32_t v = 0;

    if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }
    if ((v < 1) || (v > 5))
    {
        return false;
    }
    if ((valid()) && (get_state() < storageType_permanent) && (v >= storageType_permanent))
    {
        return false;
    }
    if ((valid()) && (get_state() >= storageType_readOnly))
    {
        return false;
    }
    return true;
}

bool StorageType::row_is_volatile() { return get_state() <= 2; }

void StorageType::set_state(int32_t state)
{
    if ((state >= 1) && (state <= 5))
    {
        *(dynamic_cast<SnmpInt32*>(value)) = state;
    }
}

int32_t StorageType::get_state() { return (int32_t) * (dynamic_cast<SnmpInt32*>(value)); }

/*--------------------------- class StorageTypeVoter ------------------------*/

int StorageTypeVoter::is_transition_ok(
    MibTable* t, MibTableRow* row, const Oidx& /*oid*/, int curState, int newState)
{
    int const storageType = (dynamic_cast<StorageTable*>(t))->get_storage_type(row);

    if (storageType == 5)
    {
        return SNMP_ERROR_INCONSIST_VAL;
    }
    if (((curState == rowNotInService) || (curState == rowActive)) && (newState == rowDestroy))
    {
        if (storageType == 4)
        {
            return SNMP_ERROR_INCONSIST_VAL;
        }
    }
    return SNMP_ERROR_SUCCESS;
}

/*--------------------------- class StorageTable ------------------------*/

StorageTable::StorageTable(const StorageTable& other) : MibTable(other)
{
    storage_type = other.storage_type;
    register_row_status_voting();
}

StorageTable::StorageTable(const Oidx& o) : MibTable(o)
{
    storage_type = 0;
    register_row_status_voting();
}

StorageTable::StorageTable(const Oidx& o, unsigned int ilen) : MibTable(o, ilen)
{
    storage_type = 0;
    register_row_status_voting();
}

StorageTable::StorageTable(const Oidx& o, unsigned int ilen, bool a) : MibTable(o, ilen, a)
{
    storage_type = 0;
    register_row_status_voting();
}

StorageTable::StorageTable(const Oidx& o, const index_info* istruc, unsigned int ilen)
    : MibTable(o, istruc, ilen)
{
    storage_type = 0;
    register_row_status_voting();
}

StorageTable::~StorageTable()
{
    if (storage_type_voter)
    {
        delete storage_type_voter;
    }
}

void StorageTable::register_row_status_voting()
{
    storage_type_voter = new StorageTypeVoter();
    add_voter(storage_type_voter);
}

bool StorageTable::is_persistent(MibTableRow* row)
{
    if (row->get_nth(storage_type))
    {
        if ((dynamic_cast<StorageType*>(row->get_nth(storage_type)))->row_is_volatile())
        {
            return false;
        }
    }
    return true;
}

void StorageTable::add_storage_col(StorageType* col)
{
    storage_type = generator.size();
    MibTable::add_col(col);
}

void StorageTable::set_storage_type(MibTableRow* row, int storageType)
{
    if (row->get_nth(storage_type))
    {
        (dynamic_cast<StorageType*>(row->get_nth(storage_type)))->set_state(storageType);
    }
}

int StorageTable::get_storage_type(MibTableRow* row)
{
    int storageType = 0;

    if (row->get_nth(storage_type))
    {
        storageType = (dynamic_cast<StorageType*>(row->get_nth(storage_type)))->get_state();
    }
    return storageType;
}

void StorageTable::reset()
{
    OidListCursor<MibTableRow> cur;

    for (cur.init(&content); cur.get();)
    {
        long const type = (dynamic_cast<StorageType*>(cur.get()->get_nth(storage_type)))->get_state();
        if ((type != storageType_permanent) && (type != storageType_readOnly))
        {
            MibTableRow* victim = cur.get();
            cur.next();
            delete content.remove(victim);
        }
        else
        {
            cur.next();
        }
    }
}

int StorageTable::prepare_set_request(Request* req, int& ind)
{
    MibLeaf* o = nullptr;

    if ((o = MibTable::find(req->get_oid(ind))) != nullptr)
    {
        int const storageType = get_storage_type(o->get_reference_to_row());
        if (storageType == storageType_readOnly)
        {
            return SNMP_ERROR_INCONSIST_VAL;
        }
    }
    return MibTable::prepare_set_request(req, ind);
}

/*------------------------- class SnmpInt32MinMax ------------------------*/

SnmpInt32MinMax::SnmpInt32MinMax(
    const Oidx& o, mib_access _access, const int def_val, int vmode, int _min, int _max)
    : MibLeaf(o, _access, new SnmpInt32(def_val), vmode)
{
    min = _min;
    max = _max;
}

SnmpInt32MinMax::SnmpInt32MinMax(const Oidx& o, mib_access _access, int _min, int _max)
    : MibLeaf(o, _access, new SnmpInt32(0), VMODE_NONE)
{
    min = _min;
    max = _max;
}

MibEntryPtr SnmpInt32MinMax::clone()
{
    MibEntryPtr other = new SnmpInt32MinMax(oid, access, 0, get_value_mode(), min, max);

    (dynamic_cast<SnmpInt32MinMax*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpInt32MinMax*>(other))->set_reference_to_table(my_table);
    return other;
}

bool SnmpInt32MinMax::value_ok(const Vbx& v)
{
    SnmpInt32 si;

    if (v.get_value(si) != SNMP_CLASS_SUCCESS)
    {
        return false;
    }
    if (((int)si < min) || ((int)si > max))
    {
        return false;
    }
    return true;
}

int SnmpInt32MinMax::get_state() { return *(dynamic_cast<SnmpInt32*>(value)); }

void SnmpInt32MinMax::set_state(int i) { *(dynamic_cast<SnmpInt32*>(value)) = i; }

/*------------------------- class OctetStrMinMax ------------------------*/

OctetStrMinMax::OctetStrMinMax(const Oidx& o, mib_access _access, OctetStr* def_val, int vmode,
    unsigned int _min, unsigned int _max)
    : MibLeaf(o, _access, def_val, vmode)
{
    min = _min;
    max = _max;
}

OctetStrMinMax::OctetStrMinMax(const Oidx& o, mib_access _access, unsigned int _min, unsigned int _max)
    : MibLeaf(o, _access, new OctetStr(""), VMODE_NONE)
{
    min = _min;
    max = _max;
}

MibEntryPtr OctetStrMinMax::clone()
{
    MibEntryPtr other = new OctetStrMinMax(oid, access, nullptr, get_value_mode(), min, max);

    (dynamic_cast<OctetStrMinMax*>(other))->replace_value(value->clone());
    (dynamic_cast<OctetStrMinMax*>(other))->set_reference_to_table(my_table);
    return other;
}

int OctetStrMinMax::prepare_set_request(Request* req, int& ind)
{
    OctetStr  ostr;
    Vbx const vb(req->get_value(ind));

    if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS)
    {
        return SNMP_ERROR_WRONG_TYPE;
    }
    if ((ostr.len() < min) || (ostr.len() > max))
    {
        return SNMP_ERROR_WRONG_LENGTH;
    }
    return MibLeaf::prepare_set_request(req, ind);
}

/*----------------- class SnmpMessageProcessingModel -------------------*/

SnmpMessageProcessingModel::SnmpMessageProcessingModel(const Oidx& id, mib_access a, int i, int m)
    : SnmpInt32MinMax(id, a, i, m, 0, 3)
{ }

SnmpMessageProcessingModel::~SnmpMessageProcessingModel() { }

MibEntryPtr SnmpMessageProcessingModel::clone()
{
    MibEntryPtr other = new SnmpMessageProcessingModel(oid, access, 0, get_value_mode());

    (dynamic_cast<SnmpMessageProcessingModel*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpMessageProcessingModel*>(other))->set_reference_to_table(my_table);
    return other;
}

/*--------------------- class SnmpSecurityLevel ------------------------*/

SnmpSecurityLevel::SnmpSecurityLevel(const Oidx& id, mib_access a, int i, int m)
    : SnmpInt32MinMax(id, a, i, m, 1, 3)
{ }

SnmpSecurityLevel::~SnmpSecurityLevel() { }

MibEntryPtr SnmpSecurityLevel::clone()
{
    MibEntryPtr other = new SnmpSecurityLevel(oid, access, 0, get_value_mode());

    (dynamic_cast<SnmpSecurityLevel*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpSecurityLevel*>(other))->set_reference_to_table(my_table);
    return other;
}

/*--------------------- class SnmpSecurityModel ------------------------*/

SnmpSecurityModel::SnmpSecurityModel(const Oidx& id, mib_access a, int i, int m)
    : SnmpInt32MinMax(id, a, i, m, 0, 3)
{ }

SnmpSecurityModel::~SnmpSecurityModel() { }

MibEntryPtr SnmpSecurityModel::clone()
{
    MibEntryPtr other = new SnmpSecurityModel(oid, access, 0, get_value_mode());

    (dynamic_cast<SnmpSecurityModel*>(other))->replace_value(value->clone());
    (dynamic_cast<SnmpSecurityModel*>(other))->set_reference_to_table(my_table);
    return other;
}

/*------------------------ class TimeStamp -----------------------------*/

TimeStamp::TimeStamp(const Oidx& id, mib_access a, int m) : MibLeaf(id, a, new TimeTicks(0), m) { }

TimeStamp::~TimeStamp() { }

void TimeStamp::update() TS_SYNCHRONIZED({ *dynamic_cast<TimeTicks*>(value) = sysUpTime::get(); })

    MibEntryPtr TimeStamp::clone()
{
    MibEntryPtr other = new TimeStamp(oid, access, value_mode);

    (dynamic_cast<TimeStamp*>(other))->replace_value(value->clone());
    (dynamic_cast<TimeStamp*>(other))->set_reference_to_table(my_table);
    return other;
}

/*----------------------- class TimeStampTable --------------------------*/

TimeStampTable::TimeStampTable(const Oidx& o, const index_info* inf, unsigned int sz, TimeStamp* lc)
    : MibTable(o, inf, sz)
{
    lastChange = lc;
}

TimeStampTable::~TimeStampTable() { lastChange = nullptr; }

void TimeStampTable::row_added(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/)
{
    lastChange->update();
}

void TimeStampTable::row_delete(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/)
{
    lastChange->update();
}

void TimeStampTable::updated() { lastChange->update(); }

/*----------------------- class DateAndTime --------------------------*/

DateAndTime::DateAndTime(const Oidx& id, mib_access a, int mode) : MibLeaf(id, a, new OctetStr(), mode)
{
    update();
}

DateAndTime::~DateAndTime() { }

MibEntryPtr DateAndTime::clone()
{
    MibEntryPtr other = new DateAndTime(oid, access, value_mode);

    (dynamic_cast<DateAndTime*>(other))->replace_value(value->clone());
    (dynamic_cast<DateAndTime*>(other))->set_reference_to_table(my_table);
    return other;
}

OctetStr DateAndTime::get_state() { return *(dynamic_cast<OctetStr*>(value)); }

void DateAndTime::set_state(const OctetStr& s) { *(dynamic_cast<OctetStr*>(value)) = s; }

void DateAndTime::update()
{
    time_t const c = sysUpTime::get_currentTime();
    struct tm    stm { };
    struct tm*   dt = nullptr;

#ifdef HAVE_LOCALTIME_R
    dt = localtime_r(&c, &stm); // TODO: check if gmtime_r() would be better? CK
#else
    dt = localtime(&c); // TODO: use localtime_s()! CK
#endif

    if (!dt)
    {
        return; // TODO: possibly log an error;
    }
    OctetStr val;
    val += (unsigned char)((dt->tm_year + 1900) >> 8) & 0xFF;
    val += (unsigned char)(dt->tm_year + 1900) & 0xFF;
    val += (unsigned char)dt->tm_mon + 1;
    val += (unsigned char)dt->tm_mday;
    val += (unsigned char)dt->tm_hour;
    val += (unsigned char)dt->tm_min;
    val += (unsigned char)dt->tm_sec;
    val += (unsigned char)0;

#if defined __FreeBSD__ || defined __APPLE__
    if (dt->tm_gmtoff >= 0)
    {
        val += '+';
    }
    else
    {
        val += '-';
    }
    auto const tz           = (unsigned int)abs(dt->tm_gmtoff);
    /*long const*/ timezone = dt->tm_gmtoff;
#else
    // initialize timezone needed?
    // tzset();
#    ifdef WIN32
    long timezone = _timezone; // TODO: use _get_timezone! CK
#    endif
    if (timezone < 0)
    {
        val += '+';
    }
    else
    {
        val += '-';
    }
    unsigned int tz = std::abs(timezone);
#endif

    val += (unsigned char)((tz / 3600) + ((dt->tm_isdst > 0) ? ((timezone > 0) ? -1 : 1) : 0));
    val += (unsigned char)((tz % 3600) / 60);
    set_state(val);
}
