/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - agent_copy.cpp
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

#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/snmp_request.h>
#include <iostream>
#include <signal.h>
#include <snmp_pp/log.h>
#include <snmp_pp/snmp_pp.h>
#include <snmp_pp/v3.h>
#include <stdlib.h>

using namespace std;

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#define BULK_MAX 10

int set_config_mode(Pdu& pdu, SnmpTarget* target, Snmp& snmp, int mode)
{
    Vb vbConfig("1.3.6.1.4.1.4976.2.1.1.0");

    vbConfig.set_value(SnmpInt32(mode));
    pdu.set_vblist(&vbConfig, 1);

    return snmp.set(pdu, *target);
}

int main(int argc, char** argv)
{
    int requests = 0; // keep track of # of requests
    int objects  = 0;

    //---------[ check the arg count ]----------------------------------------
    if (argc < 3)
    {
        cout << "Usage:\n";
        cout << "agent_copy sourceAddress destAddress [subtreeOid] [options]\n";
        cout << "options: prefix - applies to sourceAddress\n";
        cout << "                + applies to destAddress\n\n";
        cout << "         -x    cross subtree upper bound\n";
        cout << "         -v1 , use SNMPV1, default\n";
        cout << "         -v2 , use SNMPV2\n";
#ifdef _SNMPv3
        cout << "         -v3 , use SNMPV3\n"; // Jo
#endif
        cout << "         -cCommunityName (default = 'public')\n";
        cout << "         -pPort , remote port to use\n";
        cout << "         -rN , retries default is N = 1 retry\n";
        cout << "         -tN , timeout in mili-seconds default is N = 100 = "
                "1 second\n";
#ifdef _SNMPv3
        cout << "         -snSecurityName, " << endl;
        cout << "         -slN , securityLevel to use, default N = 3 = authPriv" << endl;
        cout << "         -smN , securityModel to use, only default N = 3 = "
                "USM possible\n";
        cout << "         -cnContextName, default "
                ""
                ""
             << endl;
        cout << "         -ceContextEngineID, default "
                ""
                ""
             << endl;
        cout << "         -md5 , use MD5 authentication protocol\n";
        cout << "         -sha , use SHA authentication protocol\n";
        cout << "         -des , use DES privacy protocol\n";
        cout << "         -idea, use IDEA privacy protocol\n";
        cout << "         -uaAuthPassword\n";
        cout << "         -upPrivPassword\n";
#endif
        return 0;
    }

    Snmp::socket_startup(); // Initialize socket subsystem

    //---------[ make a GenAddress and Oid object to retrieve ]---------------
    UdpAddress srcAddress(argv[1]); // make a SNMP++ Generic address
    UdpAddress destAddress(argv[2]);
    if (!srcAddress.valid())
    {                               // check validity of address
        cout << "Invalid Address or DNS Name, " << argv[1] << "\n";
        return -1;
    }
    if (!destAddress.valid())
    { // check validity of address
        cout << "Invalid Address or DNS Name, " << argv[2] << "\n";
        return -1;
    }
    Oid oid("1"); // default is beginning of MIB
    if (argc >= 4)
    {             // if 3 args, then use the callers Oid
        if (strstr(argv[3], "-") == 0)
        {
            oid = argv[3];
            if (!oid.valid())
            { // check validity of user oid
                cout << "Invalid Oid, " << argv[3] << "\n";
                return -2;
            }
        }
    }

    //---------[ determine options to use ]-----------------------------------
    snmp_version   srcVersion = version1;   // default is v1
    int            srcRetries = 1;          // default retries is 1
    int            srcTimeout = 100;        // default is 1 second
    unsigned short srcPort    = 161;        // default snmp port is 161
    OctetStr       srcCommunity("public");  // read community

    snmp_version   destVersion = version1;  // default is v1
    int            destRetries = 1;         // default retries is 1
    int            destTimeout = 100;       // default is 1 second
    unsigned short destPort    = 161;       // default snmp port is 161
    OctetStr       destCommunity("public"); // read community
    int            crossSubtree = 0;
#ifdef _SNMPv3
    OctetStr srcPrivPassword("");
    OctetStr srcAuthPassword("");
    OctetStr srcSecurityName("");
    int      srcSecurityModel = SNMP_SECURITY_MODEL_USM;
    int      srcSecurityLevel = SecurityLevel_authPriv;
    OctetStr srcContextName("");
    OctetStr srcContextEngineID("");
    long     srcAuthProtocol = SNMPv3_usmNoAuthProtocol;
    long     srcPrivProtocol = SNMPv3_usmNoPrivProtocol;

    OctetStr destPrivPassword("");
    OctetStr destAuthPassword("");
    OctetStr destSecurityName("");
    int      destSecurityModel = SNMP_SECURITY_MODEL_USM;
    int      destSecurityLevel = SecurityLevel_authPriv;
    OctetStr destContextName("");
    OctetStr destContextEngineID("");
    long     destAuthProtocol = SNMPv3_usmNoAuthProtocol;
    long     destPrivProtocol = SNMPv3_usmNoPrivProtocol;
#endif
    char* ptr;
    for (int x = 1; x < argc; x++)
    { // parse for version
        if (strstr(argv[x], "-x") != 0)
        {
            crossSubtree = 1;
        }
        if (strstr(argv[x], "-v2") != 0)
        {
            srcVersion = version2c;
        }
        if (strstr(argv[x], "+v2") != 0)
        {
            destVersion = version2c;
        }
        if (strstr(argv[x], "-r") != 0)
        { // parse for retries
            ptr = argv[x];
            ptr++;
            ptr++;
            srcRetries = atoi(ptr);
            if ((srcRetries < 1) || (srcRetries > 5))
            {
                srcRetries = 1;
            }
        }
        if (strstr(argv[x], "+r") != 0)
        { // parse for retries
            ptr = argv[x];
            ptr++;
            ptr++;
            destRetries = atoi(ptr);
            if ((destRetries < 1) || (destRetries > 5))
            {
                destRetries = 1;
            }
        }
        if (strstr(argv[x], "-t") != 0)
        { // parse for timeout
            ptr = argv[x];
            ptr++;
            ptr++;
            srcTimeout = atoi(ptr);
            continue;
        }
        if (strstr(argv[x], "+t") != 0)
        { // parse for timeout
            ptr = argv[x];
            ptr++;
            ptr++;
            destTimeout = atoi(ptr);
            continue;
        }
#ifdef _SNMPv3
        if (strstr(argv[x], "-v3") != 0) // Jo
        {
            srcVersion = version3;       // Jo
        }
        if (strstr(argv[x], "+v3") != 0) // Jo
        {
            destVersion = version3;      // Jo
        }
        if (strstr(argv[x], "-idea") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcPrivProtocol = SNMPv3_usmIDEAPrivProtocol;
            continue;
        }
        if (strstr(argv[x], "+idea") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            destPrivProtocol = SNMPv3_usmIDEAPrivProtocol;
            continue;
        }

        if (strstr(argv[x], "-sha") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcAuthProtocol = SNMPv3_usmHMACSHAAuthProtocol;
            continue;
        }
        if (strstr(argv[x], "+sha") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            destAuthProtocol = SNMPv3_usmHMACSHAAuthProtocol;
            continue;
        }

        if (strstr(argv[x], "-des") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcPrivProtocol = SNMPv3_usmDESPrivProtocol;
            continue;
        }
        if (strstr(argv[x], "+des") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            destPrivProtocol = SNMPv3_usmDESPrivProtocol;
            continue;
        }

        if (strstr(argv[x], "-md5") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcAuthProtocol = SNMPv3_usmHMACMD5AuthProtocol;
            continue;
        }
        if (strstr(argv[x], "+md5") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcAuthProtocol = SNMPv3_usmHMACMD5AuthProtocol;
            continue;
        }

        if (strstr(argv[x], "-sn") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcSecurityName = ptr;
            continue;
        }
        if (strstr(argv[x], "+sn") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destSecurityName = ptr;
            continue;
        }

        if (strstr(argv[x], "-sl") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcSecurityLevel = atoi(ptr);
            if ((srcSecurityLevel < SecurityLevel_noAuthNoPriv)
                || (srcSecurityLevel > SecurityLevel_authPriv))
            {
                srcSecurityLevel = SecurityLevel_authPriv;
            }
            continue;
        }
        if (strstr(argv[x], "+sl") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destSecurityLevel = atoi(ptr);
            if ((destSecurityLevel < SecurityLevel_noAuthNoPriv)
                || (destSecurityLevel > SecurityLevel_authPriv))
            {
                destSecurityLevel = SecurityLevel_authPriv;
            }
            continue;
        }

        if (strstr(argv[x], "-sm") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcSecurityModel = atoi(ptr);
            if ((srcSecurityModel < SNMP_SECURITY_MODEL_V1)
                || (srcSecurityModel > SNMP_SECURITY_MODEL_USM))
            {
                srcSecurityModel = SNMP_SECURITY_MODEL_USM;
            }
            continue;
        }
        if (strstr(argv[x], "+sm") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destSecurityModel = atoi(ptr);
            if ((destSecurityModel < SNMP_SECURITY_MODEL_V1)
                || (destSecurityModel > SNMP_SECURITY_MODEL_USM))
            {
                destSecurityModel = SNMP_SECURITY_MODEL_USM;
            }
            continue;
        }

        if (strstr(argv[x], "-cn") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcContextName = ptr;
            continue;
        }
        if (strstr(argv[x], "+cn") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destContextName = ptr;
            continue;
        }

        if (strstr(argv[x], "-ce") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destContextEngineID = ptr;
            continue;
        }
        if (strstr(argv[x], "+ce") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destContextEngineID = ptr;
            continue;
        }

        if (strstr(argv[x], "-ua") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcAuthPassword = ptr;
            continue;
        }
        if (strstr(argv[x], "+ua") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destAuthPassword = ptr;
            continue;
        }

        if (strstr(argv[x], "-up") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            srcPrivPassword = ptr;
            continue;
        }
        if (strstr(argv[x], "+up") != 0)
        {
            ptr = argv[x];
            ptr += 3;
            destPrivPassword = ptr;
            continue;
        }
#endif
        if (strstr(argv[x], "-c") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            srcCommunity = ptr;
            continue;
        }
        if (strstr(argv[x], "+c") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            destCommunity = ptr;
            continue;
        }

        if (strstr(argv[x], "-p") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            sscanf(ptr, "%hu", &srcPort);
            continue;
        }
        if (strstr(argv[x], "+p") != 0)
        {
            ptr = argv[x];
            ptr++;
            ptr++;
            sscanf(ptr, "%hu", &destPort);
            continue;
        }
    }

    srcAddress.set_port(srcPort);
    destAddress.set_port(destPort);

    //----------[ create a SNMP++ session ]-----------------------------------
    int  status;
    Snmp snmp(status); // check construction status
    if (status != SNMP_CLASS_SUCCESS)
    {
        cout << "SNMP++ Session Create Fail, " << snmp.error_msg(status) << "\n";
        return -3;
    }

#ifdef _SNMPv3
    USM*  usm;
    v3MP* v3_MP;
    //---------[ init SnmpV3 ]--------------------------------------------
    if ((srcVersion == version3) || (destVersion == version3))
    {
        const char*  engineId = "agent_copy";
        const char*  filename = "snmpv3_boot_counter";
        unsigned int snmpEngineBoots;
        int          status;

        status = getBootCounter(filename, engineId, snmpEngineBoots);
        if ((status != SNMPv3_OK) && (status < SNMPv3_FILEOPEN_ERROR))
        {
            cout << "Error loading snmpEngineBoots counter: " << status << endl;
            return 1;
        }
        snmpEngineBoots++;
        status = saveBootCounter(filename, engineId, snmpEngineBoots);
        if (status != SNMPv3_OK)
        {
            cout << "Error saving snmpEngineBoots counter: " << status << endl;
            return 1;
        }

        int construct_status;
        v3_MP = new v3MP(engineId, snmpEngineBoots, construct_status);

        usm = v3_MP->get_usm();
    }
    if (srcVersion == version3)
    {
        usm->add_usm_user(srcSecurityName, srcSecurityName, srcAuthProtocol, srcPrivProtocol,
            srcAuthPassword, srcPrivPassword);
    }
    if (destVersion == version3)
    {
        usm->add_usm_user(srcSecurityName, destSecurityName, destAuthProtocol, destPrivProtocol,
            destAuthPassword, destPrivPassword);
    }
#endif
    //--------[ build up SNMP++ object needed ]-------------------------------
    Pdux    srcPdu;                   // construct a Pdu object
    Pdux    destPdu;                  // construct a Pdu object
    CTarget ctargetSrc(srcAddress);   // make a target using the address
    CTarget ctargetDest(destAddress); // make a target using the address
#ifdef _SNMPv3
    UTarget utargetSrc(srcAddress);
    UTarget utargetDest(destAddress); // make a target using the address

    if (srcVersion == version3)
    {
        utargetSrc.set_version(srcVersion); // set the SNMP version SNMPV1 or V2 or V3
        utargetSrc.set_retry(srcRetries);   // set the number of auto retries
        utargetSrc.set_timeout(srcTimeout); // set timeout
        utargetSrc.set_security_model(srcSecurityModel);
        utargetSrc.set_security_name(srcSecurityName);

        srcPdu.set_security_level(destSecurityLevel);
        srcPdu.set_context_name(destContextName);
        srcPdu.set_context_engine_id(destContextEngineID);
    }
    else
    {
#endif
        ctargetSrc.set_version(srcVersion);         // set the SNMP version SNMPV1 or V2 or V3
        ctargetSrc.set_retry(srcRetries);           // set the number of auto retries
        ctargetSrc.set_timeout(srcTimeout);         // set timeout
        ctargetSrc.set_readcommunity(srcCommunity); // set the read community to use
        ctargetSrc.set_writecommunity(srcCommunity);
#ifdef _SNMPv3
    }

    if (destVersion == version3)
    {
        utargetDest.set_version(destVersion); // set the SNMP version SNMPV1 or V2 or V3
        utargetDest.set_retry(destRetries);   // set the number of auto retries
        utargetDest.set_timeout(destTimeout); // set timeout
        utargetDest.set_security_model(destSecurityModel);
        utargetDest.set_security_name(destSecurityName);

        destPdu.set_security_level(destSecurityLevel);
        destPdu.set_context_name(destContextName);
        destPdu.set_context_engine_id(destContextEngineID);
    }
    else
    {
#endif
        ctargetDest.set_version(destVersion);         // set the SNMP version SNMPV1 or V2 or V3
        ctargetDest.set_retry(destRetries);           // set the number of auto retries
        ctargetDest.set_timeout(destTimeout);         // set timeout
        ctargetDest.set_readcommunity(destCommunity); // set the read community to use
        ctargetDest.set_writecommunity(destCommunity);
#ifdef _SNMPv3
    }
#endif
    //-------[ issue the request, blocked mode ]-----------------------------
    cout << "AGENT++ copying from " << argv[1] << " to " << argv[2] << endl;

    SnmpTarget* targetSrc;
    SnmpTarget* targetDest;
#ifdef _SNMPv3
    if (srcVersion == version3)
    {
        targetSrc = &utargetSrc;
    }
    else
#endif
        targetSrc = &ctargetSrc;

#ifdef _SNMPv3
    if (destVersion == version3)
    {
        targetDest = &utargetDest;
    }
    else
#endif
        targetDest = &ctargetDest;

    // set the config mode
    status = set_config_mode(destPdu, targetDest, snmp, 2);

    if (status != SNMP_ERROR_SUCCESS)
    {
        cout << "Target seems not to be an AGENT++ simulation agent:" << endl;
#ifdef _SNMPv3
        if (destPdu.get_type() == REPORT_MSG)
        {
            Vb vbConfig;
            destPdu.get_vb(vbConfig, 0);
            Oid tmp;
            vbConfig.get_oid(tmp);
            cout << "Received a reportPdu: " << snmp.error_msg(tmp) << endl
                 << vbConfig.get_printable_oid() << " = " << vbConfig.get_printable_value() << endl;
            return -5;
        }
#endif
        cout << "SNMP error: " << snmp.error_msg(status) << "\n";
    }

    Vb vb;                     // construct a Vb object
    vb.set_oid(oid);           // set the Oid portion of the Vb
    srcPdu.set_vblist(&vb, 1); // add the vb to the Pdu

    while ((status = snmp.get_bulk(srcPdu, *targetSrc, 0, BULK_MAX)) == SNMP_CLASS_SUCCESS)
    {
        requests++;
#ifdef _SNMPv3
        if (srcPdu.get_type() == REPORT_MSG)
        {
            Oid tmp;
            vb.get_oid(tmp);
            cout << "Received a reportPdu: " << snmp.error_msg(tmp) << endl
                 << vb.get_printable_oid() << " = " << vb.get_printable_value() << endl;
            return -5;
        }
#endif
        for (int z = 0; z < srcPdu.get_vb_count(); z++)
        {
            srcPdu.get_vb(vb, z);
            Oidx coid(vb.get_printable_oid());
            // check for end
            if ((!crossSubtree) && (!coid.in_subtree_of(oid)))
            {
                cout << "End of subtree reached\n";
                cout << "Total # of Requests = " << requests << "\n";
                cout << "Total # of Objects  = " << objects << "\n";
                set_config_mode(destPdu, targetDest, snmp, 1);
                return 0;
            }
            if (coid <= oid)
            {
                cout << "Lexicographic ordering error detected:\n";
                cout << "ERROR: " << vb.get_printable_oid() << " <= ";
                cout << oid.get_printable() << "\n";
                cout << "Total # of Requests = " << requests << "\n";
                cout << "Total # of Objects  = " << objects << "\n";
                set_config_mode(destPdu, targetDest, snmp, 1);
                return 0;
            }
            objects++;
            // look for var bind exception, applies to v2 only
            if (vb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW)
            {
                destPdu.set_vblist(&vb, 1);
                status = snmp.set(destPdu, *targetDest);
                if (status == SNMP_ERROR_SUCCESS)
                {
                    cout << "OK : ";
                }
                else
                {
                    cout << "NOK: ";
                }
                cout << vb.get_printable_oid() << " = ";
                cout << vb.get_printable_value();
                if (status != SNMP_ERROR_SUCCESS)
                {
                    cout << ", reason: " << snmp.error_msg(status);
                }
                cout << endl;
            }
            else
            {
                cout << "End of MIB Reached\n";
                cout << "Total # of Requests = " << requests << "\n";
                cout << "Total # of Objects  = " << objects << "\n";
                set_config_mode(destPdu, targetDest, snmp, 1);
                return -4;
            }
        }
        // last vb becomes seed of next rquest
        srcPdu.set_vblist(&vb, 1);
    }
    set_config_mode(destPdu, targetDest, snmp, 1);

    if (status != SNMP_ERROR_NO_SUCH_NAME)
    {
        cout << "SNMP++ snmpWalk Error, " << snmp.error_msg(status) << "\n";
    }
    cout << "Total # of Requests = " << requests << "\n";
    cout << "Total # of Objects  = " << objects << "\n";

    Snmp::socket_cleanup(); // Shut down socket subsystem
}
