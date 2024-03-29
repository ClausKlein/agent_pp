/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - agentpp_config_mib.h
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
//--AgentGen END

#ifndef _agentpp_config_mib_h
#define _agentpp_config_mib_h

#include <agent_pp/mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_textual_conventions.h>

// Scalars
#define oidAgentppCfgLogLevelError        "1.3.6.1.4.1.4976.3.3.1.1.1.0"
#define oidAgentppCfgLogLevelWarning      "1.3.6.1.4.1.4976.3.3.1.1.2.0"
#define oidAgentppCfgLogLevelEvent        "1.3.6.1.4.1.4976.3.3.1.1.3.0"
#define oidAgentppCfgLogLevelInfo         "1.3.6.1.4.1.4976.3.3.1.1.4.0"
#define oidAgentppCfgLogLevelDebug        "1.3.6.1.4.1.4976.3.3.1.1.5.0"
#define oidAgentppCfgSecSrcAddrValidation "1.3.6.1.4.1.4976.3.3.1.2.1.0"

// Columns
#define oidAgentppCfgStoragePath        "1.3.6.1.4.1.4976.3.3.1.3.1.1.2"
#define colAgentppCfgStoragePath        "2"
#define oidAgentppCfgStorageFormat      "1.3.6.1.4.1.4976.3.3.1.3.1.1.3"
#define colAgentppCfgStorageFormat      "3"
#define oidAgentppCfgStorageLastStore   "1.3.6.1.4.1.4976.3.3.1.3.1.1.4"
#define colAgentppCfgStorageLastStore   "4"
#define oidAgentppCfgStorageLastRestore "1.3.6.1.4.1.4976.3.3.1.3.1.1.5"
#define colAgentppCfgStorageLastRestore "5"
#define oidAgentppCfgStorageOperation   "1.3.6.1.4.1.4976.3.3.1.3.1.1.6"
#define colAgentppCfgStorageOperation   "6"
#define oidAgentppCfgStorageStorageType "1.3.6.1.4.1.4976.3.3.1.3.1.1.7"
#define colAgentppCfgStorageStorageType "7"
#define oidAgentppCfgStorageStatus      "1.3.6.1.4.1.4976.3.3.1.3.1.1.8"
#define colAgentppCfgStorageStatus      "8"

// Tables
#define oidAgentppCfgStorageEntry     "1.3.6.1.4.1.4976.3.3.1.3.1.1"
#define nAgentppCfgStoragePath        0
#define cAgentppCfgStoragePath        2
#define nAgentppCfgStorageFormat      1
#define cAgentppCfgStorageFormat      3
#define nAgentppCfgStorageLastStore   2
#define cAgentppCfgStorageLastStore   4
#define nAgentppCfgStorageLastRestore 3
#define cAgentppCfgStorageLastRestore 5
#define nAgentppCfgStorageOperation   4
#define cAgentppCfgStorageOperation   6
#define nAgentppCfgStorageStorageType 5
#define cAgentppCfgStorageStorageType 7
#define nAgentppCfgStorageStatus      6
#define cAgentppCfgStorageStatus      8

// Notifications

//--AgentGen BEGIN=_INCLUDE
#ifdef _SNMPv3
//--AgentGen END

#    ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
using namespace Snmp_pp;
#    endif

//--AgentGen BEGIN=_UTIL_CLASSES

class AGENTPP_DECL agentppCfgStorageOperation;

#    ifndef _NO_THREADS
class AGENTPP_DECL OperationTask : public Thread {
public:
    OperationTask(int op, agentppCfgStorageOperation* source)
    {
        operation = op;
        initiator = source;
    }

    ~OperationTask() override { }

    void run() override;

private:
    int                         operation;
    agentppCfgStorageOperation* initiator;
};
#    endif

//--AgentGen END

// Scalar Objects

/**
 *  agentppCfgSecSrcAddrValidation
 *
 * "Specifies whether SNMPv1/v2c source address
 * validation via the snmpTargetAddrExtTable and
 * the snmpCommunityTable is enabled or disabled.
 *
 * If the value of this object is notAvailable(3), then at
 * least one of the necessary MIB modules are not
 * implemented for this agent instance and an attempt
 * to set this object's value to enabled(1) or disabled(2)
 * will result in a wrongValue error."
 */

class AGENTPP_DECL agentppCfgSecSrcAddrValidation : public MibLeaf {
public:
    /**
     * Default constructor.
     */
    agentppCfgSecSrcAddrValidation();

    /**
     * Construct with Mib reference to avoid Mib::instance usage to modify
     * source address validation in Mib.
     *
     * @param mibRef a pointer to the Mib instance.
     * @since 4.3.0.
     */
    agentppCfgSecSrcAddrValidation(Mib* mibRef) : agentppCfgSecSrcAddrValidation()
    {
        mibReference = mibRef;
    }

    ~agentppCfgSecSrcAddrValidation() override;

    static agentppCfgSecSrcAddrValidation* instance;

    void            get_request(Request* /*unused*/, int /*unused*/) override;
    virtual int32_t get_state();
    virtual void    set_state(int32_t);
    int             set(const Vbx& /*vb*/) override;
    bool            value_ok(const Vbx& /*unused*/) override;

    enum labels { e_enabled = 1, e_disabled = 2, e_notAvailable = 3 };

    //--AgentGen BEGIN=agentppCfgSecSrcAddrValidation
protected:
    Mib* mibReference;

    //--AgentGen END
};

// Columnar Objects

// Tables

// Notifications
#    ifdef _SNMPv3
#    endif

// Group

class AGENTPP_DECL agentpp_config_mib : public MibGroup {
public:
    /**
     * Default constructor without agentppCfgStorageEntry.
     */
    agentpp_config_mib();
    ~agentpp_config_mib() override { }

//--AgentGen BEGIN=agentpp_config_mib
#    ifndef _NO_THREADS
    /**
     * Constructor with agentppCfgStorageEntry that allows to
     * store and restore the agent persistent configuration at
     * runtime.
     */
    agentpp_config_mib(Mib*);
#    endif
    //--AgentGen END
};

//--AgentGen BEGIN=_CLASSES
class AGENTPP_DECL agentppCfgLogLevel : public MibLeaf {
public:
    agentppCfgLogLevel(int, const Oidx&);
    ~agentppCfgLogLevel() override;

    virtual int32_t get_state();
    virtual void    set_state(int32_t);
    void            get_request(Request* /*unused*/, int /*unused*/) override;
    bool            value_ok(const Vbx& /*unused*/) override;
    int             commit_set_request(Request* /*unused*/, int /*unused*/) override;
    int             undo_set_request(Request* /*unused*/, int& /*unused*/) override;

protected:
    int logClass;
};
//--AgentGen END

// Columnar Objects

/**
 *  agentppCfgStoragePath
 *
 * "The path to the configuration on the agent's file
 * system. Depending on the persistent storage type,
 * the path can be a directory or a file."
 */

class AGENTPP_DECL agentppCfgStoragePath : public SnmpDisplayString {
public:
    agentppCfgStoragePath(const Oidx&);
    ~agentppCfgStoragePath() override;

    [[nodiscard]] MibEntryPtr clone() const override;
    void                      get_request(Request* /*unused*/, int /*unused*/) override;
    virtual NS_SNMP OctetStr  get_state();
    virtual void              set_state(const NS_SNMP OctetStr&);
    int                       prepare_set_request(Request* /*unused*/, int& /*unused*/) override;

    //--AgentGen BEGIN=agentppCfgStoragePath
    int commit_set_request(Request* /*unused*/, int /*unused*/) override;

    //--AgentGen END
};

/**
 *  agentppCfgStorageFormat
 *
 * "The storage format specifies the format of the persistent
 * configuration storage associated with this row.
 * Currently only AGENT++'s BER encoded MIB object
 * serialization 'agentppBER(1)' is supported."
 */

class AGENTPP_DECL agentppCfgStorageFormat : public MibLeaf {
public:
    agentppCfgStorageFormat(const Oidx&);
    ~agentppCfgStorageFormat() override;

    [[nodiscard]] MibEntryPtr clone() const override;
    void                      get_request(Request* /*unused*/, int /*unused*/) override;
    virtual int32_t           get_state();
    virtual void              set_state(int32_t);
    int                       prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool                      value_ok(const Vbx& /*unused*/) override;

    enum labels { e_agentppBER = 1 };

    //--AgentGen BEGIN=agentppCfgStorageFormat
    //--AgentGen END
};

/**
 *  agentppCfgStorageOperation
 *
 * "The states 'idle(1)' and 'inProgress(2)'  can only be
 * read, whereas the states 'store(3)' and 'restore(4)' can
 * only be written.
 *
 * Setting this object to 'store(3)' will save the agent's
 * configuration to the location identified by
 * agentppCfgStoragePath. Setting this object to
 * 'restore(4)' resets the agent to the configuration
 * read from location agentppCfgStoragePath.
 * While the two operations above are in progress,
 * this object returns 'inProgress(2)' on get requests.
 * Otherwise 'idle(1)' is returned on get requests.
 *
 * While its state is 'inProgress' any set request returns
 * a 'resourceUnavailable(13)' error."
 */

class AGENTPP_DECL agentppCfgStorageOperation : public MibLeaf {
    friend class OperationTask;

public:
    agentppCfgStorageOperation(const Oidx&);
    ~agentppCfgStorageOperation() override;

    [[nodiscard]] MibEntryPtr clone() const override;
    void                      get_request(Request* /*unused*/, int /*unused*/) override;
    virtual int32_t           get_state();
    virtual void              set_state(int32_t);
    int                       set(const Vbx& /*vb*/) override;
    int                       prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    bool                      value_ok(const Vbx& /*unused*/) override;

    enum labels { e_idle = 1, e_inProgress = 2, e_store = 3, e_restore = 4 };

    //--AgentGen BEGIN=agentppCfgStorageOperation
    bool is_volatile() override { return true; }

#    ifndef _NO_THREADS
private:
    OperationTask* operationTask;
#    endif
    //--AgentGen END
};

/**
 *  agentppCfgStorageStorageType
 *
 * "The storage type of the entry in the AGENT++ persistent
 * storage table."
 */

class AGENTPP_DECL agentppCfgStorageStorageType : public StorageType {
public:
    agentppCfgStorageStorageType(const Oidx&);
    ~agentppCfgStorageStorageType() override;

    [[nodiscard]] MibEntryPtr clone() const override;
    void                      get_request(Request* /*unused*/, int /*unused*/) override;
    int32_t                   get_state() const override;
    void                      set_state(int32_t /*unused*/) override;
    int                       set(const Vbx& /*vb*/) override;
    int                       prepare_set_request(Request* /*unused*/, int& /*unused*/) override;

    enum labels { e_other = 1, e_volatile = 2, e_nonVolatile = 3, e_permanent = 4, e_readOnly = 5 };

    //--AgentGen BEGIN=agentppCfgStorageStorageType
    //--AgentGen END
};

/**
 *  agentppCfgStorageStatus
 *
 * "Control for creating and deleting entries.  Entries may
 * not be modified while active."
 */

class AGENTPP_DECL agentppCfgStorageStatus : public snmpRowStatus {
public:
    agentppCfgStorageStatus(const Oidx&);
    ~agentppCfgStorageStatus() override;

    [[nodiscard]] MibEntryPtr clone() const override;
    virtual int32_t           get_state();
    virtual void              set_state(int32_t);
    int                       set(const Vbx& /*unused*/) override;
    int                       prepare_set_request(Request* /*unused*/, int& /*unused*/) override;

    enum labels {
        e_active        = 1,
        e_notInService  = 2,
        e_notReady      = 3,
        e_createAndGo   = 4,
        e_createAndWait = 5,
        e_destroy       = 6
    };

    //--AgentGen BEGIN=agentppCfgStorageStatus
    //--AgentGen END
};

// Tables

/**
 *  agentppCfgStorageEntry
 *
 * "A row specifying the location and storage format
 * of an AGENT++ agent configuration."
 */

class AGENTPP_DECL agentppCfgStorageEntry : public StorageTable {
public:
    agentppCfgStorageEntry();
    ~agentppCfgStorageEntry() override;

    static agentppCfgStorageEntry* instance;

    void get_request(Request* /*unused*/, int /*unused*/) override;
    int  prepare_set_request(Request* /*unused*/, int& /*unused*/) override;
    int  is_transition_ok(MibTable* /*unused*/, MibTableRow* /*unused*/, const Oidx& /*unused*/,
         int /*unused*/, int /*unused*/) override;
    virtual void set_row(MibTableRow* r, const NS_SNMP OctetStr& p1, int32_t p2, uint32_t p3,
        uint32_t p4, int32_t p5, int32_t p6, int32_t p7);

    //--AgentGen BEGIN=agentppCfgStorageEntry
    void set_mib(Mib* m);

    Mib* get_mib() { return mib; }

    /**
     * Do not remove any rows when resetted, because a restore operation
     * may need a row.
     */
    void reset() override { }

    /**
     * If set to true, absolute and relative paths containing dots
     * are not allowed. true is the default value.
     * @param securePaths
     *    if true, only secure paths are allowed for persistent storage.
     */
    void set_secure_paths(bool b) { securePaths = b; }

    /**
     * Returns true if only secure paths can be configured via SNMP.
     * @return
     *    true if only secure paths are allowed, false if any path is
     *    allowed.
     */
    bool is_secure_paths() const { return securePaths; }

private:
    bool securePaths;
    Mib* mib;
    //--AgentGen END
};

#    ifdef AGENTPP_NAMESPACE
}
#    endif

//--AgentGen BEGIN=_END
#endif
//--AgentGen END

#endif
