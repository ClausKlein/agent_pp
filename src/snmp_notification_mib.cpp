/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - snmp_notification_mib.cpp
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

#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <libagent.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
#endif

#ifndef _NO_LOGGING
static const char* loggerModuleName = "agent++.snmp_notification_mib";
#endif

static constexpr bool WITH_LENGTH { true };

/**
 *  snmpNotifyType
 *
 */

/**
 *  snmpNotifyRowStatus
 *
 */

/**
 *  snmpNotifyFilterProfileName
 *
 */

/**
 *  snmpNotifyFilterProfileRowStatus
 *
 */

/**
 *  snmpNotifyFilterMask
 *
 */

/**
 *  snmpNotifyFilterType
 *
 */

/**
 *  snmpNotifyFilterRowStatus
 *
 */

/**
 *  snmpNotifyEntry
 *
 */

snmpNotifyEntry* snmpNotifyEntry::instance = nullptr;

snmpNotifyEntry::snmpNotifyEntry() : StorageTable(oidSnmpNotifyEntry, iSnmpAdminString, 1)
{
    // This table object is a singleton. In order to access it use
    // the static pointer snmpNotifyEntry::instance.
    instance = this;

    add_col(new SnmpTagValue("2"));
    add_col(new SnmpInt32MinMax("3", READCREATE, 1, VMODE_DEFAULT, 1, 2));
    add_storage_col(new StorageType("4", 3));
    add_col(new snmpRowStatus("5"));
}

snmpNotifyEntry::~snmpNotifyEntry() { }

MibTableRow* snmpNotifyEntry::add_entry(const OctetStr& name, const OctetStr& tag, const int type)
{
    start_synch();
    Oidx const   index = Oidx::from_string(name, !WITH_LENGTH); // withoutLength
    MibTableRow* r     = find_index(index);
    if (r)
    {
        end_synch();
        return nullptr;
    }
    r = add_row(index);
    r->get_nth(0)->replace_value(new OctetStr(tag));
    r->get_nth(1)->replace_value(new SnmpInt32(type));
    // leave default value untouched (storage type)
    r->get_nth(3)->replace_value(new SnmpInt32(rowActive));
    end_synch();
    return r;
}

/**
 *  snmpNotifyFilterProfileEntry
 *
 */

snmpNotifyFilterProfileEntry* snmpNotifyFilterProfileEntry::instance = nullptr;

const index_info indSnmpNotifyFilterProfileEntry[1] = { { sNMP_SYNTAX_OCTETS, true, 1, 32 } };

snmpNotifyFilterProfileEntry::snmpNotifyFilterProfileEntry()
    : StorageTable("1.3.6.1.6.3.13.1.2.1", indSnmpNotifyFilterProfileEntry, 1)
{
    // This table object is a singleton. In order to access it use
    // the static pointer snmpNotifyFilterProfileEntry::instance.
    instance = this;

    add_col(new SnmpAdminString("1", READCREATE, new OctetStr(""), VMODE_NONE, 1, 32));
    add_storage_col(new StorageType("2", 3));
    add_col(new snmpRowStatus("3"));
}

snmpNotifyFilterProfileEntry::~snmpNotifyFilterProfileEntry() { }

/**
 *  snmpNotifyFilterEntry
 *
 */

snmpNotifyFilterEntry* snmpNotifyFilterEntry::instance = nullptr;

const index_info iSnmpNotifyFilterEntry[2] = { { sNMP_SYNTAX_OCTETS, false, 0, 32 },
    { sNMP_SYNTAX_OID, true, 1, 95 } };

snmpNotifyFilterEntry::snmpNotifyFilterEntry()
    : snmpNotifyFilterEntry(snmpNotifyFilterProfileEntry::instance)
{ }

snmpNotifyFilterEntry::snmpNotifyFilterEntry(snmpNotifyFilterProfileEntry* profileEntry)
    : StorageTable("1.3.6.1.6.3.13.1.3.1", iSnmpNotifyFilterEntry, 2),
      _snmpNotifyFilterProfileEntry(profileEntry)
{
    // This table object is a singleton. In order to access it use
    // the static pointer snmpNotifyFilterEntry::instance.
    instance = this;

    add_col(new OctetStrMinMax("2", READCREATE, new OctetStr(""), VMODE_DEFAULT, 0, 16));
    add_col(new SnmpInt32MinMax("3", READCREATE, 1, VMODE_DEFAULT, 1, 2));
    add_storage_col(new StorageType("4", 3));
    add_col(new snmpRowStatus("5"));
}

snmpNotifyFilterEntry::~snmpNotifyFilterEntry() { }

bool snmpNotifyFilterEntry::passes_filter(
    const Oidx& target, const Oidx& _oid, const Vbx* vbs, unsigned int vb_count)
{
    _snmpNotifyFilterProfileEntry->start_synch();
    MibTableRow* found = _snmpNotifyFilterProfileEntry->find_index(target);

    // FIXME: no profile -> passes filter
    if (!found)
    {
        _snmpNotifyFilterProfileEntry->end_synch();
        return true;
    }
    OctetStr profileName;
    found->first()->get_value(profileName);
    _snmpNotifyFilterProfileEntry->end_synch();

    Oidx const profileOid = Oidx::from_string(profileName, WITH_LENGTH);

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
    LOG("NotificationOriginator: filter: using (profile) (as oid)");
    LOG(profileName.get_printable_hex());
    LOG(profileOid.get_printable());
    LOG_END;

    List<MibTableRow>*      list = get_rows_cloned(&profileOid, rowActive);
    ListCursor<MibTableRow> cur;

    // FIXME: no filter -> passes filter
    if (list->size() == 0)
    {
        delete list;
        return true;
    }

    OidList<MibStaticEntry> matches;
    auto*                   oidmatches = new OidList<MibStaticEntry>[vb_count];
    for (cur.init(list); cur.get(); cur.next())
    {
        Oidx subtree = cur.get()->get_index();
        // no need to check: if (subtree.len()<3) continue;
        subtree = subtree.cut_left(subtree[0] + 1);

        OctetStr  filterMask;
        SnmpInt32 filterType = 0;

        cur.get()->first()->get_value(filterMask);
        cur.get()->get_nth(1)->get_value(filterType);

        // check if oid is in the filter specified by filterMask
        // and subtree.
        if (subtree.compare(_oid, filterMask) >= 0)
        {
            Oidx sid;
            sid += subtree.len();
            sid += subtree;
            auto* match = new MibStaticEntry(sid, filterType);
            matches.add(match);

            LOG_BEGIN(loggerModuleName, INFO_LOG | 4);
            LOG("NotificationOriginator: filter: "
                "(trapoid)(subtree)(filterMask)(filterType)(match)");
            LOG(Oidx(oid).get_printable());
            LOG(subtree.get_printable());
            LOG(filterMask.get_printable_hex());
            LOG(filterType);
            LOG(sid.get_printable());
            LOG_END;
        }
        else
        {
            LOG_BEGIN(loggerModuleName, INFO_LOG | 5);
            LOG("NotificationOriginator: filter: "
                "(trapoid)(subtree)(filterMask)(filterType)(match)");
            LOG(Oidx(oid).get_printable());
            LOG(subtree.get_printable());
            LOG(filterMask.get_printable_hex());
            LOG(filterType);
            LOG("no match");
            LOG_END;
        }
        for (unsigned int i = 0; i < vb_count; i++)
        {
            if (subtree.compare(vbs[i].get_oid(), filterMask) >= 0)
            {
                Oidx sid;
                sid += subtree.len();
                sid += subtree;
                auto* match = new MibStaticEntry(sid, filterType);
                oidmatches[i].add(match);
            }
        }
    }
    delete list;
    if (matches.size() == 0)
    {
        delete[] oidmatches;
        return false;
    }

    SnmpInt32 pass = 0;
    matches.last()->get_value(pass);
    if (pass == 1)
    {
        for (unsigned int i = 0; i < vb_count; i++)
        {
            if (oidmatches[i].size() > 0)
            {
                oidmatches[i].last()->get_value(pass);
                if (pass == 2)
                {
                    delete[] oidmatches;
                    return false;
                }
            }
        }
    }
    delete[] oidmatches;
    return pass == 1;
}

snmp_notification_mib::snmp_notification_mib() : MibGroup("1.3.6.1.6.3.13.1", "snmpNotificationMIB")
{
    auto* notifyEntry              = new snmpNotifyEntry();
    auto* notifyFilterProfileEntry = new snmpNotifyFilterProfileEntry();
    auto* notifyFilterEntry        = new snmpNotifyFilterEntry(notifyFilterProfileEntry);

    add(notifyEntry);
    add(notifyFilterProfileEntry);
    add(notifyFilterEntry);
}

#ifdef AGENTPP_NAMESPACE
}
#endif
