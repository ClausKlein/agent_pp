/*_############################################################################
  _##
  _##  AGENT++ 4.5 - notification_originator.cpp
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

#include <agent_pp/notification_log_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/snmp_request.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/system_group.h>
#include <agent_pp/vacm.h>
#include <libagent.h>
#include <snmp_pp/log.h>

#define NO_TRAP 0
#define TRAP    1
#define INFORM  2

#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
#endif

#ifndef _NO_LOGGING
static const char* loggerModuleName = "agent++.notification_originator";
#endif
/*------------------ class NotificationOriginator -----------------------*/

NotificationOriginator::NotificationOriginator()
{
#ifdef _SNMPv3
    localEngineID  = nullptr;
    communityEntry = nullptr;
    _nlmLogEntry   = nullptr;
    v3mp           = nullptr;
#endif

    targetAddrEntry   = nullptr;
    targetParamsEntry = nullptr;
    notifyEntry       = nullptr;
    notifyFilterEntry = nullptr;
    mib               = Mib::instance;
}

NotificationOriginator::~NotificationOriginator()
{
#ifdef _SNMPv3
    if (localEngineID) delete localEngineID;
#endif
    mib = nullptr;
}

void NotificationOriginator::generate(
    Vbx* vbs, int size, const Oidx& id, const Oidx& enterprise, const OctetStr& contextName)
{
    generate(vbs, size, id, sysUpTime::get(), enterprise, contextName);
}

void NotificationOriginator::generate(
    Vbx* vbs, int size, const Oidx& id, unsigned int timestamp, const OctetStr& contextName)
{
    generate(vbs, size, id, timestamp, "", contextName);
}

int NotificationOriginator::notify(
    const OctetStr& context, const Oidx& oid, Vbx* vbs, int sz, unsigned int timestamp)
{
    if (timestamp == 0) timestamp = sysUpTime::get();
    return generate(vbs, sz, oid, timestamp, Oidx(), context);
}

int NotificationOriginator::generate(Vbx* vbs, int size, const Oidx& id, unsigned int timestamp,
    const Oidx& enterprise, const OctetStr& contextName)
{
    // We have to be careful here about synchronization because,
    // we may be called after an interrupt
    // Therefore synch everything or use synch methods like
    // MibTable::get_rows_cloned();
    List<MibTableRow>*      typeList = get_snmp_notify_entry()->get_rows_cloned();
    ListCursor<MibTableRow> typeCur;

    List<MibTableRow>*      list = get_snmp_target_addr_entry()->get_rows_cloned();
    ListCursor<MibTableRow> cur;

#ifdef _SNMPv3
    if (!localEngineID)
    {
        v3MP* mp = get_v3mp();
        if (!mp)
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
            LOG("NotificationOriginator: v3MP has to be initialized before "
                "notifications can be sent");
            LOG_END;

            return SNMP_ERROR_RESOURCE_UNAVAIL; // TODO: is this the right
                                                // status value? CK
        }
        localEngineID = new OctetStr(mp->get_local_engine_id());
    }
#endif

    int totalStatus = SNMP_ERROR_SUCCESS;

    NotificationOriginatorParams nop(vbs, size, id, timestamp, enterprise, contextName);
#ifdef _SNMPv3
    nlmLogEntry* logEntry = get_nlm_log_entry();
    if (logEntry)
    {
        logEntry->add_notification(
            nullptr, id, vbs, size, contextName, *localEngineID, *localEngineID);
    }
#endif
    for (cur.init(list); cur.get(); cur.next())
    {

        int notify = NO_TRAP;

        // look for tags that identify notifications
        for (typeCur.init(typeList); typeCur.get(); typeCur.next())
        {

            OctetStr tag;
            typeCur.get()->first()->get_value(tag);
            char* tagstr = new char[tag.len() + 1];
            strlcpy(tagstr, (char*)tag.data(), tag.len());
            tagstr[tag.len()] = 0; // OK, CK

            if (((SnmpTagList*)cur.get()->get_nth(4))->contains(tagstr))
            {
                // determine notification type
                typeCur.get()->get_nth(1)->get_value(notify);

                nop.target = nullptr;
                if (check_access(cur, nop))
                {
                    int const status = send_notify(cur, nop, notify);
                    if (status != SNMP_ERROR_SUCCESS) totalStatus = status;
                    delete nop.target;
                }
            }
            delete[] tagstr;
        }
        if (notify == NO_TRAP)
        {
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
            LOG("NotificationOriginator: generate: could not find valid tag");
            LOG_END;
            continue;
        }
    }

    typeList->clearAll();
    delete typeList;
    list->clearAll();
    delete list;

    return totalStatus;
}

snmpTargetAddrEntry* NotificationOriginator::get_snmp_target_addr_entry()
{
    if (mib || targetAddrEntry)
    {
        if (!targetAddrEntry)
        {
            targetAddrEntry = (snmpTargetAddrEntry*)mib->get(oidSnmpTargetAddrEntry);
        }
        return targetAddrEntry;
    }
    return snmpTargetAddrEntry::instance;
}

snmpTargetParamsEntry* NotificationOriginator::get_snmp_target_params_entry()
{
    if (mib || targetParamsEntry)
    {
        if (!targetParamsEntry)
        {
            targetParamsEntry = (snmpTargetParamsEntry*)mib->get(oidSnmpTargetParamsEntry);
        }
        return targetParamsEntry;
    }
    return snmpTargetParamsEntry::instance;
}

snmpNotifyEntry* NotificationOriginator::get_snmp_notify_entry()
{
    if (mib || notifyEntry)
    {
        if (!notifyEntry) { notifyEntry = (snmpNotifyEntry*)mib->get(oidSnmpNotifyEntry); }
        return notifyEntry;
    }
    return snmpNotifyEntry::instance;
}

snmpNotifyFilterEntry* NotificationOriginator::get_snmp_notify_filter_entry()
{
    if (mib || notifyFilterEntry)
    {
        if (!notifyFilterEntry)
        {
            notifyFilterEntry = (snmpNotifyFilterEntry*)mib->get(oidSnmpNotifyFilterEntry);
        }
        return notifyFilterEntry;
    }
    return snmpNotifyFilterEntry::instance;
}

#ifdef _SNMPv3
snmpCommunityEntry* NotificationOriginator::get_snmp_community_entry()
{
    if (mib || communityEntry)
    {
        if (!communityEntry) { communityEntry = (snmpCommunityEntry*)mib->get(oidSnmpCommunityEntry); }
        return communityEntry;
    }
    return snmpCommunityEntry::instance;
}

nlmLogEntry* NotificationOriginator::get_nlm_log_entry()
{
    if (mib || _nlmLogEntry)
    {
        if (!_nlmLogEntry) { _nlmLogEntry = (nlmLogEntry*)mib->get(oidNlmLogEntry); }
        return _nlmLogEntry;
    }
    return nlmLogEntry::instance;
}

v3MP* NotificationOriginator::get_v3mp()
{
    if (mib) { return mib->get_request_list()->get_v3mp(); }
    else { return v3MP::instance; }
}
#endif

bool NotificationOriginator::check_access(
    ListCursor<MibTableRow>& cur, NotificationOriginatorParams& nop)
{
    Vbx*&       vbs           = nop.vbs;
    int const&  size          = nop.size;
    const Oidx& id            = nop.id;
    OctetStr&   securityName  = nop.securityName;
    int&        securityModel = nop.securityModel;
    int&        securityLevel = nop.securityLevel;
    int&        mpModel       = nop.mpModel;
#ifdef _SNMPv3
    const OctetStr& contextName = nop.contextName;
    UTarget*&       target      = nop.target;
#else
    CTarget*& target = nop.target;
#endif

    auto*    paramsPtr = (snmpTargetAddrParams*)cur.get()->get_nth(5);
    OctetStr paramsStr;
    paramsPtr->get_value(paramsStr);

    // Check whether trap oid passes filter
    Oidx const             targetOid(Oidx::from_string(paramsStr, false));
    snmpNotifyFilterEntry* notifyFilterEntry = get_snmp_notify_filter_entry();
    if (!notifyFilterEntry || !notifyFilterEntry->passes_filter(targetOid, id, vbs, size))
    {

        LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
        LOG("NotificationOriginator: generate: event did not pass "
            "notification filter (trapoid)(filter)");
        LOG(Oidx(id).get_printable());
        LOG(paramsStr.get_printable());
        LOG_END;
        return false;
    }

    OctetStr targetAddress;
    cur.get()->get_nth(1)->get_value(targetAddress);

    snmpTargetParamsEntry* targetParamsEntry = get_snmp_target_params_entry();
    if (!targetParamsEntry) { return false; }
    targetParamsEntry->start_synch();
    MibTableRow* paramsRow = targetParamsEntry->find_index(Oidx::from_string(paramsStr, false));

    if ((!paramsRow) || (paramsRow->get_row_status()->get() != rowActive))
    {

        targetParamsEntry->end_synch();
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
        LOG("NotificationOriginator: generate: target addr parameter row not "
            "found.");
        LOG(paramsStr.get_printable());
        LOG((paramsRow) ? "no active row found" : "missing row");
        LOG_END;
        return false;
    }

    paramsRow->get_nth(0)->get_value(mpModel);
    paramsRow->get_nth(2)->get_value(securityName);
    paramsRow->get_nth(1)->get_value(securityModel);
    paramsRow->get_nth(3)->get_value(securityLevel);

    targetParamsEntry->end_synch();

    bool accessAllowed = true;
#ifdef _SNMPv3
    if ((!mib) || (!mib->get_request_list()) || (!mib->get_request_list()->get_vacm()))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
        LOG("NotificationOriginator: Mib or its requestList are not "
            "initialized, aborting!");
        LOG_END;
        return false;
    }
    Vacm* vacm = mib->get_request_list()->get_vacm();

    for (int i = 0; i < size; i++)
    {

        if (vacm->isAccessAllowed(securityModel, securityName, securityLevel, mibView_notify,
                contextName, vbs[i].get_oid())
            != VACM_accessAllowed)
        {
            accessAllowed = false;
            break;
        }
    }

    if (accessAllowed)
    {
        accessAllowed = (vacm->isAccessAllowed(securityModel, securityName, securityLevel,
                             mibView_notify, contextName, id)
            == VACM_accessAllowed);
    }
#endif
    if (!accessAllowed)
    {
        LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
        LOG("Notification not sent (reason) (addr) (params)");
        LOG("no access");
        LOG(targetAddress.get_printable());
        LOG(paramsStr.get_printable());
        LOG_END;

        return false;
    }

    target                               = nullptr;
    snmpTargetAddrEntry* targetAddrEntry = get_snmp_target_addr_entry();
    if (targetAddrEntry)
    {
        Address* address = targetAddrEntry->get_address(cur.get());
        if (address)
        {
#ifdef _SNMPv3
            target = new UTarget(*address, securityName, securityModel);
#else
            target = new CTarget(*address, securityName, securityName);
#endif
            delete address;
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

int NotificationOriginator::send_notify(
    ListCursor<MibTableRow>& cur, NotificationOriginatorParams& nop, int notify)
{
    Vbx*&       vbs          = nop.vbs;
    int const&  size         = nop.size;
    const Oidx& id           = nop.id;
    const Oidx& enterprise   = nop.enterprise;
    OctetStr&   securityName = nop.securityName;
    int const&  mpModel      = nop.mpModel;
#ifdef _SNMPv3
    int const&          securityLevel = nop.securityLevel;
    const OctetStr&     contextName   = nop.contextName;
    unsigned int const& timestamp     = nop.timestamp;
    UTarget*&           target        = nop.target;
#else
    CTarget*& target = nop.target;
#endif

#ifndef _NO_LOGGING
    long targetDomain;
    targetDomain = ((snmpTargetAddrTDomain*)cur.get()->first())->get_state();
#endif

    const Oidx& trapoid(id);

#ifdef _SNMPv3
    Pdux pdu;
    pdu.set_type((notify == TRAP) ? sNMP_PDU_TRAP : sNMP_PDU_INFORM);
    pdu.set_vblist(vbs, size);
    pdu.set_notify_id(id);
    pdu.set_notify_enterprise(enterprise);
    pdu.set_notify_timestamp(timestamp);
    pdu.set_security_level(securityLevel);
    pdu.set_context_name(contextName);

    // get community info from SNMP-COMMUNITY-MIB
    if (((mpModel == mpV1) || (mpModel == mpV2c)) && (communityEntry))
    {
        if (communityEntry->get_community(securityName, *localEngineID, contextName))
        {
            target->set_security_name(securityName);
        }
        else
        {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
            LOG("NotificationOriginator: community lookup failed for "
                "(securityName) (context)");
            LOG(securityName.get_printable());
            LOG(OctetStr(contextName).get_printable());
            LOG_END;
        }
    }
#endif
    int status = SNMP_ERROR_SUCCESS;

    if (mpModel == mpV1)
    {

#ifdef _SNMPv3
        pdu.set_type(sNMP_PDU_V1TRAP);
#endif
        target->set_version(version1);

#ifdef _SNMPv3
        status                = SnmpRequestV3::send(*target, pdu);
        nlmLogEntry* logEntry = get_nlm_log_entry();
        if (logEntry)
        {
            logEntry->add_notification(
                target, id, vbs, size, contextName, *localEngineID, *localEngineID);
        }
#else
        status = SnmpRequest::process_trap(*target, vbs, size, id, enterprise, (notify != TRAP));
#endif

        GenAddress address;
        target->get_address(address);

        LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
        LOG("NotificationGenerator: sent v1 trap "
            "(id)(tdomain)(addr)(vbs)(community)");
        LOG(trapoid.get_printable());
        LOG(targetDomain);
        LOG(address.get_printable());
        LOG(size);
        LOG(securityName.get_printable());
        LOG_END;
    }
    else
    {
#ifdef _SNMPv3
        if (mpModel == mpV3)
            target->set_version(version3);
        else
#endif
            target->set_version(version2c);

        if (notify != TRAP)
        {
            target->set_retry(((SnmpInt32MinMax*)cur.get()->get_nth(3))->get_state());
            target->set_timeout(((SnmpInt32MinMax*)cur.get()->get_nth(2))->get_state());
        }

#ifdef _SNMPv3
        status                = SnmpRequestV3::send(*target, pdu);
        nlmLogEntry* logEntry = get_nlm_log_entry();
        if (logEntry)
        {
            OctetStr const ceid;
            logEntry->add_notification(target, trapoid, vbs, size, contextName, ceid, *localEngineID);
        }
#else
        status = SnmpRequest::process_trap(*target, vbs, size, id, enterprise, (notify != TRAP));
#endif

        GenAddress address;
        target->get_address(address);

        LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
        if (notify == TRAP)
            LOG("NotificationGenerator: sent trap "
                "(vers)(id)(tdomain)(addr)(vbs)(community/secName)(status)");
        else
            LOG("NotificationGenerator: sent inform "
                "(vers)(id)(tdomain)(addr)(vbs)(community/secName)(status)");
        LOG(mpModel);
        LOG(trapoid.get_printable());
        LOG(targetDomain);
        LOG(address.get_printable());
        LOG(size);
        LOG(securityName.get_printable());
        LOG(status);
        LOG_END;
    }
    return status;
}

bool NotificationOriginator::add_v1_trap_destination(
    const UdpAddress& addr, const OctetStr& name, const OctetStr& tag, const OctetStr& community)
{
    OctetStr  address;
    IpAddress ip(addr);
    for (int i = 0; i < addr.get_length() - 2; i++) { address += (unsigned char)ip[i]; }
    address += (addr.get_port() >> 8);
    address += (addr.get_port() & 0x00FF);

    if (!get_snmp_target_addr_entry() || !get_snmp_target_params_entry()) { return false; }
    if (get_snmp_target_params_entry()->add_entry(name,                           // row index
            mpV1,                                                                 // mpModel
            SNMP_SECURITY_MODEL_V1,
            community,                                                            // secName
            1))
    {                                                                             // secLevel
        get_snmp_notify_entry()->add_entry(name,                                  // row index
            tag,                                                                  // tag
            TRAP);                                                                // type (trap)
    }
    if (get_snmp_target_addr_entry()->add_entry(UdpAddress(addr).get_printable(), // row index
            Oidx("1.3.6.1.6.1.1"),                                                // UDP domain
            address,                                                              // target address
            tag,                                                                  // tag
            name))
    {                                                                             // params entry
        return true;
    }
    return false;
}

bool NotificationOriginator::add_v2_trap_destination(
    const UdpAddress& addr, const OctetStr& name, const OctetStr& tag, const OctetStr& community)
{
    OctetStr  address;
    IpAddress ip(addr);
    for (int i = 0; i < addr.get_length() - 2; i++) { address += (unsigned char)ip[i]; }
    address += (addr.get_port() >> 8);
    address += (addr.get_port() & 0x00FF);

    if (!get_snmp_target_addr_entry() || !get_snmp_target_params_entry()) { return false; }
    if (get_snmp_target_params_entry()->add_entry(name,                           // row index
            mpV2c,                                                                // mpModel
            SNMP_SECURITY_MODEL_V2,
            community,                                                            // secName
            1))
    {                                                                             // secLevel
        get_snmp_notify_entry()->add_entry(name,                                  // row index
            tag,                                                                  // tag
            TRAP);                                                                // type (trap)
    }
    if (get_snmp_target_addr_entry()->add_entry(UdpAddress(addr).get_printable(), // row index
            Oidx("1.3.6.1.6.1.1"),                                                // UDP domain
            address,                                                              // target address
            tag,                                                                  // tag
            name))                                                                // params entry
        return true;
    return false;
}

bool NotificationOriginator::add_v3_trap_destination(const UdpAddress& addr, const OctetStr& name,
    const OctetStr& tag, const OctetStr& secName, const int secLevel)
{
    OctetStr  address;
    IpAddress ip(addr);
    for (int i = 0; i < addr.get_length() - 2; i++) { address += (unsigned char)ip[i]; }
    address += (addr.get_port() >> 8);
    address += (addr.get_port() & 0x00FF);

    if (!get_snmp_target_addr_entry() || !get_snmp_target_params_entry() || !get_snmp_notify_entry())
        return false;

    if (get_snmp_target_params_entry()->add_entry(name, // row index
            mpV3,                                       // mpModel
            SNMP_SECURITY_MODEL_USM, secName, secLevel))
    {
        notifyEntry->add_entry(name,                                 // row index
            tag,                                                     // tag
            TRAP);                                                   // type (trap)
    }
    if (targetAddrEntry->add_entry(UdpAddress(addr).get_printable(), // row index
            Oidx("1.3.6.1.6.1.1"),                                   // UDP domain
            address,                                                 // target address
            tag,                                                     // tag
            name))                                                   // params entry
        return true;
    return false;
}

#ifdef AGENTPP_NAMESPACE
}
#endif
