/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - notification_log_mib.h
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

//--AgentGen BEGIN=_BEGIN
#include <agent_pp/agent++.h>
#ifdef _SNMPv3

#    include <agent_pp/mib_complex_entry.h>
//--AgentGen END

#    ifndef _notification_log_mib_h
#        define _notification_log_mib_h

#        include <agent_pp/mib.h>
#        include <agent_pp/snmp_textual_conventions.h>

#        define oidNlmConfigGlobalEntryLimit         "1.3.6.1.2.1.92.1.1.1.0"
#        define oidNlmConfigGlobalAgeOut             "1.3.6.1.2.1.92.1.1.2.0"
#        define oidNlmConfigLogTable                 "1.3.6.1.2.1.92.1.1.3"
#        define oidNlmConfigLogEntry                 "1.3.6.1.2.1.92.1.1.3.1"
#        define oidNlmLogName                        "1.3.6.1.2.1.92.1.1.3.1.1"
#        define colNlmLogName                        "1"
#        define oidNlmConfigLogFilterName            "1.3.6.1.2.1.92.1.1.3.1.2"
#        define colNlmConfigLogFilterName            "2"
#        define oidNlmConfigLogEntryLimit            "1.3.6.1.2.1.92.1.1.3.1.3"
#        define colNlmConfigLogEntryLimit            "3"
#        define oidNlmConfigLogAdminStatus           "1.3.6.1.2.1.92.1.1.3.1.4"
#        define colNlmConfigLogAdminStatus           "4"
#        define oidNlmConfigLogOperStatus            "1.3.6.1.2.1.92.1.1.3.1.5"
#        define colNlmConfigLogOperStatus            "5"
#        define oidNlmConfigLogStorageType           "1.3.6.1.2.1.92.1.1.3.1.6"
#        define colNlmConfigLogStorageType           "6"
#        define oidNlmConfigLogEntryStatus           "1.3.6.1.2.1.92.1.1.3.1.7"
#        define colNlmConfigLogEntryStatus           "7"
#        define oidNlmStatsGlobalNotificationsLogged "1.3.6.1.2.1.92.1.2.1.0"
#        define oidNlmStatsGlobalNotificationsBumped "1.3.6.1.2.1.92.1.2.2.0"
#        define oidNlmStatsLogTable                  "1.3.6.1.2.1.92.1.2.3"
#        define oidNlmStatsLogEntry                  "1.3.6.1.2.1.92.1.2.3.1"
#        define oidNlmStatsLogNotificationsLogged    "1.3.6.1.2.1.92.1.2.3.1.1"
#        define colNlmStatsLogNotificationsLogged    "1"
#        define oidNlmStatsLogNotificationsBumped    "1.3.6.1.2.1.92.1.2.3.1.2"
#        define colNlmStatsLogNotificationsBumped    "2"
#        define oidNlmLogTable                       "1.3.6.1.2.1.92.1.3.1"
#        define oidNlmLogEntry                       "1.3.6.1.2.1.92.1.3.1.1"
#        define oidNlmLogIndex                       "1.3.6.1.2.1.92.1.3.1.1.1"
#        define colNlmLogIndex                       "1"
#        define oidNlmLogTime                        "1.3.6.1.2.1.92.1.3.1.1.2"
#        define colNlmLogTime                        "2"
#        define oidNlmLogDateAndTime                 "1.3.6.1.2.1.92.1.3.1.1.3"
#        define colNlmLogDateAndTime                 "3"
#        define oidNlmLogEngineID                    "1.3.6.1.2.1.92.1.3.1.1.4"
#        define colNlmLogEngineID                    "4"
#        define oidNlmLogEngineTAddress              "1.3.6.1.2.1.92.1.3.1.1.5"
#        define colNlmLogEngineTAddress              "5"
#        define oidNlmLogEngineTDomain               "1.3.6.1.2.1.92.1.3.1.1.6"
#        define colNlmLogEngineTDomain               "6"
#        define oidNlmLogContextEngineID             "1.3.6.1.2.1.92.1.3.1.1.7"
#        define colNlmLogContextEngineID             "7"
#        define oidNlmLogContextName                 "1.3.6.1.2.1.92.1.3.1.1.8"
#        define colNlmLogContextName                 "8"
#        define oidNlmLogNotificationID              "1.3.6.1.2.1.92.1.3.1.1.9"
#        define colNlmLogNotificationID              "9"
#        define oidNlmLogVariableTable               "1.3.6.1.2.1.92.1.3.2"
#        define oidNlmLogVariableEntry               "1.3.6.1.2.1.92.1.3.2.1"
#        define oidNlmLogVariableIndex               "1.3.6.1.2.1.92.1.3.2.1.1"
#        define colNlmLogVariableIndex               "1"
#        define oidNlmLogVariableID                  "1.3.6.1.2.1.92.1.3.2.1.2"
#        define colNlmLogVariableID                  "2"
#        define oidNlmLogVariableValueType           "1.3.6.1.2.1.92.1.3.2.1.3"
#        define colNlmLogVariableValueType           "3"
#        define oidNlmLogVariableCounter32Val        "1.3.6.1.2.1.92.1.3.2.1.4"
#        define colNlmLogVariableCounter32Val        "4"
#        define oidNlmLogVariableUnsigned32Val       "1.3.6.1.2.1.92.1.3.2.1.5"
#        define colNlmLogVariableUnsigned32Val       "5"
#        define oidNlmLogVariableTimeTicksVal        "1.3.6.1.2.1.92.1.3.2.1.6"
#        define colNlmLogVariableTimeTicksVal        "6"
#        define oidNlmLogVariableInteger32Val        "1.3.6.1.2.1.92.1.3.2.1.7"
#        define colNlmLogVariableInteger32Val        "7"
#        define oidNlmLogVariableOctetStringVal      "1.3.6.1.2.1.92.1.3.2.1.8"
#        define colNlmLogVariableOctetStringVal      "8"
#        define oidNlmLogVariableIpAddressVal        "1.3.6.1.2.1.92.1.3.2.1.9"
#        define colNlmLogVariableIpAddressVal        "9"
#        define oidNlmLogVariableOidVal              "1.3.6.1.2.1.92.1.3.2.1.10"
#        define colNlmLogVariableOidVal              "10"
#        define oidNlmLogVariableCounter64Val        "1.3.6.1.2.1.92.1.3.2.1.11"
#        define colNlmLogVariableCounter64Val        "11"
#        define oidNlmLogVariableOpaqueVal           "1.3.6.1.2.1.92.1.3.2.1.12"
#        define colNlmLogVariableOpaqueVal           "12"

#        define nNlmConfigLogFilterName         0
#        define cNlmConfigLogFilterName         2
#        define nNlmConfigLogEntryLimit         1
#        define cNlmConfigLogEntryLimit         3
#        define nNlmConfigLogAdminStatus        2
#        define cNlmConfigLogAdminStatus        4
#        define nNlmConfigLogOperStatus         3
#        define cNlmConfigLogOperStatus         5
#        define nNlmConfigLogStorageType        4
#        define cNlmConfigLogStorageType        6
#        define nNlmConfigLogEntryStatus        5
#        define cNlmConfigLogEntryStatus        7
#        define nNlmStatsLogNotificationsLogged 0
#        define cNlmStatsLogNotificationsLogged 1
#        define nNlmStatsLogNotificationsBumped 1
#        define cNlmStatsLogNotificationsBumped 2
#        define nNlmLogTime                     0
#        define cNlmLogTime                     2
#        define nNlmLogDateAndTime              1
#        define cNlmLogDateAndTime              3
#        define nNlmLogEngineID                 2
#        define cNlmLogEngineID                 4
#        define nNlmLogEngineTAddress           3
#        define cNlmLogEngineTAddress           5
#        define nNlmLogEngineTDomain            4
#        define cNlmLogEngineTDomain            6
#        define nNlmLogContextEngineID          5
#        define cNlmLogContextEngineID          7
#        define nNlmLogContextName              6
#        define cNlmLogContextName              8
#        define nNlmLogNotificationID           7
#        define cNlmLogNotificationID           9
#        define nNlmLogVariableID               0
#        define cNlmLogVariableID               2
#        define nNlmLogVariableValueType        1
#        define cNlmLogVariableValueType        3
#        define nNlmLogVariableCounter32Val     2
#        define cNlmLogVariableCounter32Val     4
#        define nNlmLogVariableUnsigned32Val    3
#        define cNlmLogVariableUnsigned32Val    5
#        define nNlmLogVariableTimeTicksVal     4
#        define cNlmLogVariableTimeTicksVal     6
#        define nNlmLogVariableInteger32Val     5
#        define cNlmLogVariableInteger32Val     7
#        define nNlmLogVariableOctetStringVal   6
#        define cNlmLogVariableOctetStringVal   8
#        define nNlmLogVariableIpAddressVal     7
#        define cNlmLogVariableIpAddressVal     9
#        define nNlmLogVariableOidVal           8
#        define cNlmLogVariableOidVal           10
#        define nNlmLogVariableCounter64Val     9
#        define cNlmLogVariableCounter64Val     11
#        define nNlmLogVariableOpaqueVal        10
#        define cNlmLogVariableOpaqueVal        12

//--AgentGen BEGIN=_INCLUDE
#        ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
using namespace Snmp_pp;
#        endif

class AGENTPP_DECL nlmConfigLogOperStatus : public MibLeaf {
public:
    nlmConfigLogOperStatus(const Oidx&);
    ~nlmConfigLogOperStatus() override;

    MibEntryPtr clone() override;
    void        get_request(Request* /*unused*/, int /*unused*/) override;
};

//--AgentGen END

/**
 *  nlmConfigGlobalEntryLimit
 *
 * "The maximum number of notification entries that may be held
 * in nlmLogTable for all nlmLogNames added together.  A particular
 * setting does not guarantee that much data can be held.
 *
 * If an application changes the limit while there are
 * Notifications in the log, the oldest Notifications MUST be
 * discarded to bring the log down to the new limit - thus the
 * value of nlmConfigGlobalEntryLimit MUST take precedence over
 * the values of nlmConfigGlobalAgeOut and nlmConfigLogEntryLimit,
 * even if the Notification being discarded has been present for
 * fewer minutes than the value of nlmConfigGlobalAgeOut, or if
 * the named log has fewer entries than that specified in
 * nlmConfigLogEntryLimit.
 *
 * A value of 0 means no limit.
 *
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */

class AGENTPP_DECL nlmConfigGlobalEntryLimit : public MibLeaf {
public:
    nlmConfigGlobalEntryLimit();
    ~nlmConfigGlobalEntryLimit() override;

    static nlmConfigGlobalEntryLimit* instance;

    void             get_request(Request* /*unused*/, int /*unused*/) override;
    virtual uint32_t get_state();
    virtual void     set_state(uint32_t);
    int              set(const Vbx& /*vb*/) override;
    int              prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool             value_ok(const Vbx& /*unused*/) override;

    //--AgentGen BEGIN=nlmConfigGlobalEntryLimit
    //--AgentGen END
};

/**
 *  nlmConfigGlobalAgeOut
 *
 * "The number of minutes a Notification SHOULD be kept in a log
 * before it is automatically removed.
 *
 * If an application changes the value of nlmConfigGlobalAgeOut,
 * Notifications older than the new time MAY be discarded to meet the
 * new time.
 *
 * A value of 0 means no age out.
 *
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */

class AGENTPP_DECL nlmConfigGlobalAgeOut : public MibLeaf {
public:
    nlmConfigGlobalAgeOut();
    ~nlmConfigGlobalAgeOut() override;

    static nlmConfigGlobalAgeOut* instance;

    void             get_request(Request* /*unused*/, int /*unused*/) override;
    virtual uint32_t get_state();
    virtual void     set_state(uint32_t);
    int              set(const Vbx& /*vb*/) override;
    int              prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool             value_ok(const Vbx& /*unused*/) override;

    //--AgentGen BEGIN=nlmConfigGlobalAgeOut
    //--AgentGen END
};

/**
 *  nlmConfigLogFilterName
 *
 * "A value of snmpNotifyFilterProfileName as used as an index
 * into the snmpNotifyFilterTable in the SNMP Notification MIB,
 * specifying the locally or remotely originated Notifications
 * to be filtered out and not logged in this log.
 *
 * A zero-length value or a name that does not identify an
 * existing entry in snmpNotifyFilterTable indicate no
 * Notifications are to be logged in this log."
 */

class AGENTPP_DECL nlmConfigLogFilterName : public MibLeaf {
public:
    nlmConfigLogFilterName(const Oidx&);
    ~nlmConfigLogFilterName() override;

    MibEntryPtr              clone() override;
    void                     get_request(Request* /*unused*/, int /*unused*/) override;
    virtual NS_SNMP OctetStr get_state();
    virtual void             set_state(const NS_SNMP OctetStr&);
    int                      set(const Vbx& /*vb*/) override;
    int                      prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool                     value_ok(const Vbx& /*unused*/) override;

    //--AgentGen BEGIN=nlmConfigLogFilterName
    //--AgentGen END
};

/**
 *  nlmConfigLogEntryLimit
 *
 * "The maximum number of notification entries that can be held in
 * nlmLogTable for this named log.  A particular setting does not
 * guarantee that that much data can be held.
 *
 * If an application changes the limit while there are
 * Notifications in the log, the oldest Notifications are discarded
 * to bring the log down to the new limit.
 * A value of 0 indicates no limit.
 *
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */

class AGENTPP_DECL nlmConfigLogEntryLimit : public MibLeaf {
public:
    nlmConfigLogEntryLimit(const Oidx&);
    ~nlmConfigLogEntryLimit() override;

    MibEntryPtr      clone() override;
    void             get_request(Request* /*unused*/, int /*unused*/) override;
    virtual uint32_t get_state();
    virtual void     set_state(uint32_t);
    int              set(const Vbx& /*vb*/) override;
    int              prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool             value_ok(const Vbx& /*unused*/) override;

    //--AgentGen BEGIN=nlmConfigLogEntryLimit
    //--AgentGen END
};

/**
 *  nlmConfigLogAdminStatus
 *
 * "Control to enable or disable the log without otherwise
 * disturbing the log's entry.
 *
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */

class AGENTPP_DECL nlmConfigLogAdminStatus : public MibLeaf {
public:
    nlmConfigLogAdminStatus(const Oidx&);
    ~nlmConfigLogAdminStatus() override;

    MibEntryPtr     clone() override;
    void            get_request(Request* /*unused*/, int /*unused*/) override;
    virtual int32_t get_state();
    virtual void    set_state(int32_t);
    int             set(const Vbx& /*vb*/) override;
    int             prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool            value_ok(const Vbx& /*unused*/) override;

    enum labels { e_enabled = 1, e_disabled = 2 };

    //--AgentGen BEGIN=nlmConfigLogAdminStatus
    //--AgentGen END
};

/**
 *  nlmConfigLogStorageType
 *
 * "The storage type of this conceptual row."
 */

class AGENTPP_DECL nlmConfigLogStorageType : public StorageType {
public:
    nlmConfigLogStorageType(const Oidx&);
    ~nlmConfigLogStorageType() override;

    MibEntryPtr clone() override;
    void        get_request(Request* /*unused*/, int /*unused*/) override;
    int32_t     get_state() override;
    void        set_state(int32_t /*unused*/) override;
    int         set(const Vbx& /*vb*/) override;
    int         prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool        value_ok(const Vbx& /*unused*/) override;

    enum labels { e_other = 1, e_volatile = 2, e_nonVolatile = 3, e_permanent = 4, e_readOnly = 5 };

    //--AgentGen BEGIN=nlmConfigLogStorageType
    //--AgentGen END
};

/**
 *  nlmConfigLogEntryStatus
 *
 * "Control for creating and deleting entries.  Entries may be
 * modified while active.
 *
 * For non-null-named logs, the managed system records the security
 * credentials from the request that sets nlmConfigLogStatus
 * to 'active' and uses that identity to apply access control to
 * the objects in the Notification to decide if that Notification
 * may be logged."
 */

class AGENTPP_DECL nlmConfigLogEntryStatus : public snmpRowStatus {
public:
    nlmConfigLogEntryStatus(const Oidx&);
    ~nlmConfigLogEntryStatus() override;

    MibEntryPtr     clone() override;
    virtual int32_t get_state();
    virtual void    set_state(int32_t);
    int             set(const Vbx& /*unused*/) override;
    int             prepare_set_request(Request* /*unused*/, int& /*unused*/) override;

    enum labels {
        e_active        = 1,
        e_notInService  = 2,
        e_notReady      = 3,
        e_createAndGo   = 4,
        e_createAndWait = 5,
        e_destroy       = 6
    };

    //--AgentGen BEGIN=nlmConfigLogEntryStatus
    int commit_set_request(Request* /*unused*/, int /*unused*/) override;

    //--AgentGen END
};

/**
 *  nlmStatsGlobalNotificationsLogged
 *
 * "The number of Notifications put into the nlmLogTable.  This
 * counts a Notification once for each log entry, so a Notification
 *  put into multiple logs is counted multiple times."
 */

class AGENTPP_DECL nlmStatsGlobalNotificationsLogged : public Counter32MibLeaf {
public:
    nlmStatsGlobalNotificationsLogged();
    ~nlmStatsGlobalNotificationsLogged() override;

    static nlmStatsGlobalNotificationsLogged* instance;

    void             get_request(Request* /*unused*/, int /*unused*/) override;
    virtual uint32_t get_state();
    virtual void     set_state(uint32_t);
    virtual uint32_t inc();

    //--AgentGen BEGIN=nlmStatsGlobalNotificationsLogged
    //--AgentGen END
};

/**
 *  nlmStatsGlobalNotificationsBumped
 *
 * "The number of log entries discarded to make room for a new entry
 * due to lack of resources or the value of nlmConfigGlobalEntryLimit
 * or nlmConfigLogEntryLimit.  This does not include entries discarded
 * due to the value of nlmConfigGlobalAgeOut."
 */

class AGENTPP_DECL nlmStatsGlobalNotificationsBumped : public Counter32MibLeaf {
public:
    nlmStatsGlobalNotificationsBumped();
    ~nlmStatsGlobalNotificationsBumped() override;

    static nlmStatsGlobalNotificationsBumped* instance;

    void             get_request(Request* /*unused*/, int /*unused*/) override;
    virtual uint32_t get_state();
    virtual void     set_state(uint32_t);
    virtual uint32_t inc();

    //--AgentGen BEGIN=nlmStatsGlobalNotificationsBumped
    //--AgentGen END
};

/**
 *  nlmConfigLogEntry
 *
 * "A logging control entry.  Depending on the entry's storage type
 * entries may be supplied by the system or created and deleted by
 * applications using nlmConfigLogEntryStatus."
 */

class AGENTPP_DECL nlmConfigLogEntry : public StorageTable {
    friend class nlmConfigLogEntryStatus;

public:
    nlmConfigLogEntry(Mib* mib);
    ~nlmConfigLogEntry() override;

    static nlmConfigLogEntry* instance;

    void         row_added(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_delete(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_init(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    virtual void set_row(MibTableRow* r, const NS_SNMP OctetStr& p0, uint32_t p1, int32_t p2,
        int32_t p3, int32_t p4, int32_t p5);

    //--AgentGen BEGIN=nlmConfigLogEntry
protected:
    Mib* mib;
    //--AgentGen END
};

/**
 *  nlmStatsLogEntry
 *
 * "A Notification log statistics entry."
 */

class AGENTPP_DECL nlmStatsLogEntry : public MibTable {
public:
    nlmStatsLogEntry(nlmConfigLogEntry*);
    ~nlmStatsLogEntry() override;

    static nlmStatsLogEntry* instance;

    void         row_added(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_delete(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_init(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    virtual void set_row(MibTableRow* r, uint32_t p0, uint32_t p1);

    //--AgentGen BEGIN=nlmStatsLogEntry
    bool is_volatile() override { return true; }

protected:
    nlmConfigLogEntry* configLogEntry;

    //--AgentGen END
};

/**
 *  nlmLogEntry
 *
 * "A Notification log entry.
 *
 * Entries appear in this table when Notifications occur and pass
 * filtering by nlmConfigLogFilterName and access control.  They are
 * removed to make way for new entries due to lack of resources or
 * the values of nlmConfigGlobalEntryLimit, nlmConfigGlobalAgeOut, or
 * nlmConfigLogEntryLimit.
 *
 * If adding an entry would exceed nlmConfigGlobalEntryLimit or system
 * resources in general, the oldest entry in any log SHOULD be removed
 * to make room for the new one.
 *
 * If adding an entry would exceed nlmConfigLogEntryLimit the oldest
 * entry in that log SHOULD be removed to make room for the new one.
 *
 * Before the managed system puts a locally-generated Notification
 * into a non-null-named log it assures that the creator of the log
 * has access to the information in the Notification.  If not it
 * does not log that Notification in that log."
 */

class nlmLogVariableEntry;

class AGENTPP_DECL nlmLogEntry : public MibTable {
    friend class nlmConfigLogOperStatus;

public:
    nlmLogEntry(Mib*, nlmConfigLogEntry*, nlmStatsLogEntry*, nlmLogVariableEntry*,
        nlmConfigGlobalEntryLimit*, nlmConfigGlobalAgeOut*);
    ~nlmLogEntry() override;

    static nlmLogEntry* instance;

    void         row_added(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_delete(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_init(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    virtual void set_row(MibTableRow* r, uint32_t p0, const NS_SNMP OctetStr& p1,
        const NS_SNMP OctetStr& p2, const NS_SNMP OctetStr& p3, const char* p4,
        const NS_SNMP OctetStr& p5, const NS_SNMP OctetStr& p6, const char* p7);

    //--AgentGen BEGIN=nlmLogEntry
    bool is_volatile() override { return true; }

    bool check_access(const Vbx*, const int, const NS_SNMP Oid&, MibTableRow*);
    void check_limits(List<MibTableRow>*);
    void add_notification(const NS_SNMP SnmpTarget*, const NS_SNMP Oid&, const Vbx*, const int vbcount,
        const NS_SNMP OctetStr&, const NS_SNMP OctetStr&, const NS_SNMP OctetStr&);

protected:
    OidList<MibStaticEntry>    logIndexes;
    List<MibTableRow>          entries;
    MibTable*                  secInfo;
    Mib*                       mib;
    nlmConfigLogEntry*         configLogEntry;
    nlmStatsLogEntry*          statsLogEntry;
    nlmLogVariableEntry*       logVariableEntry;
    nlmConfigGlobalEntryLimit* configGlobalEntryLimit;
    nlmConfigGlobalAgeOut*     configGlobalAgeOut;
    //--AgentGen END
};

/**
 *  nlmLogVariableEntry
 *
 * "A Notification log entry variable.
 *
 * Entries appear in this table when there are variables in
 * the varbind list of a Notification in nlmLogTable."
 */

class AGENTPP_DECL nlmLogVariableEntry : public MibTable {
public:
    nlmLogVariableEntry();
    ~nlmLogVariableEntry() override;

    static nlmLogVariableEntry* instance;

    void         row_added(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_delete(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    void         row_init(MibTableRow* /*unused*/, const Oidx& /*unused*/, MibTable* /*t*/) override;
    virtual void set_row(MibTableRow* r, const char* p0, int32_t p1, uint32_t p2, uint32_t p3,
        uint32_t p4, int32_t p5, const NS_SNMP OctetStr& p6, const char* p7, const char* p8,
        uint32_t p9hi, uint32_t p9lo, const NS_SNMP OctetStr& p10);

    //--AgentGen BEGIN=nlmLogVariableEntry
    bool is_volatile() override { return true; }

    void add_variable(const Oidx&, unsigned int, const Vbx&);

    //--AgentGen END
};

class AGENTPP_DECL notification_log_mib : public MibGroup {
public:
    notification_log_mib();
    notification_log_mib(Mib* mib);
    ~notification_log_mib() override { }

    //--AgentGen BEGIN=notification_log_mib
protected:
    Mib* mib;
    //--AgentGen END
};

//--AgentGen BEGIN=_END
#        ifdef AGENTPP_NAMESPACE
}
#        endif
#    endif
//--AgentGen END

/**
 * notification_log_mib.h generated by AgentGen 1.6.2 for AGENT++v3.4
 * Sun Jun 03 23:58:42 GMT+02:00 2001.
 */

#endif
