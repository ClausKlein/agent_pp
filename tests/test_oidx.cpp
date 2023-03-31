#include <agent_pp/mib.h>
#include <cassert>
#include <iostream>
#include <string>

static constexpr unsigned int    INDEX_LEN { 6 };
static const Agentpp::index_info index_struct[INDEX_LEN] = {
    // NOTE: { type,    implied,    min,    max }
    { sNMP_SYNTAX_OCTETS, false, 3, 32 }, // NOTE: subindex with min length set to > 1!
    { sNMP_SYNTAX_INT, false, 1, 1 },
    { sNMP_SYNTAX_OCTETS, false, 0, 2 },  // NOTE: subindex withLength!
    { sNMP_SYNTAX_INT, false, 1, 1 }, { sNMP_SYNTAX_IPADDR, false, 4, 4 }, // NOTE: fix length is 4
    { sNMP_SYNTAX_OCTETS, true, 1, 32 } // XXX "1.3.6.1.6.3.13.1.2.1"
    // NOTE: implied set! no subindex length used, only as last subindex usable!
};

// ----------------------------------------
// example for implied index length
// ----------------------------------------
// agent++/src/snmp_notification_mib.cpp:
static const Agentpp::index_info SnmpNotifyFilterEntry[2] = { { sNMP_SYNTAX_OCTETS, false, 0, 32 },
    { sNMP_SYNTAX_OID, true, 1, 95 } };
// ----------------------------------------
// example: \"test\".1.3.6.1.4.1.9999
// ----------------------------------------

// --------------------
// ./src/ip_forward_mib.cpp:
static const Agentpp::index_info ind_IP_FORWARD_MIB_IpForwardEntry[4] = {
    { sNMP_SYNTAX_IPADDR, false, 4, 4 }, { sNMP_SYNTAX_INT, false, 1, 1 },
    { sNMP_SYNTAX_INT, false, 1, 1 }, { sNMP_SYNTAX_IPADDR, false, 4, 4 }
};
// --------------------

/**
 * Return the next available index value for the receiver table,
 * that can be used by a manager to create a new row.
 * @return The next available index value.
 * @note Works best if the table's index is a single scalar sub-identifier.
 */
Agentpp::Oidx
/*MibTable::*/
get_next_avail_index()
{
    static Agentpp::Oidx* my_index = nullptr; // XXX simple HACK to emulate Agentpp::MibTable handling!

    Agentpp::Oidx retval;

    // XXX if(content.empty())
    if (!my_index)
    {
        for (const auto& i : index_struct)
        {
            if (i.implied) // NOTE: means we have no length
            {
                retval += 1;
            }
            else
            {
                if (i.type == sNMP_SYNTAX_OCTETS)
                {
                    retval += i.min; // NOTE: set length to min len!
                }
                else
                {
                    retval += 1;
                }
                for (unsigned int j = 1; j < i.max; j++)
                {
                    if ((i.type == sNMP_SYNTAX_OCTETS) && (j > i.min))
                    {
                        break; // NOTE: I want to have a valid subindex with min len
                    }
                    retval += j;
                }
            }
        }

        my_index = new Agentpp::Oidx(retval); // XXX
        return retval;
    }

    // XXX  retval = content.last()->get_index();
    retval                   = *my_index; // XXX
    retval[retval.len() - 1] = retval[retval.len() - 1] + 1;

    delete my_index;                      // XXX
    my_index = new Agentpp::Oidx(retval); // XXX

    return retval;
}

/**
 * Return the all sub index values for the current table row index given.
 *
 * @note Return all sub indices as a list of pointers to the
 *      corresponding Oidx values of the current MibTableRow index.
 *
 * @param index - the MibTableRow index (only needed for demo).
 * @return a pointer to a cloned array of the rows indices.
 **/
Agentpp::Array<Agentpp::Oidx>*
/*MibTable::*/
getRowIndicesCloned(const Agentpp::Oidx& index)
{
    static const bool trace = true;

    auto* idx = new Agentpp::Array<Agentpp::Oidx>();

    unsigned int pos = 0;
    for (unsigned int i = 0; i < INDEX_LEN; i++)
    {
        assert(pos < index.len());

        auto* cur = new Agentpp::Oidx;

        if (trace)
        {
            std::cout << "at " << i << " [" << pos << ":";
        }

        if (index_struct[i].implied)      // NOTE: subindex without length
        {
            assert(i == (INDEX_LEN - 1)); // IMPLIED only alowed for last subindex!

            *cur = index.cut_left(pos);   // last part
            pos  = index.len();           // after the END!
        }
        else                              // NOTE: withLength
        {
            if (index_struct[i].type == sNMP_SYNTAX_IPADDR)
            {
                *cur = index.cut_left(pos).cut_right(index.len() - (pos + 4));
                pos += 4;
            }
            else if (index_struct[i].type == sNMP_SYNTAX_OCTETS)
            {
                // NOTE: the first subid is the strlen!
                unsigned long strlen = index[pos];
                *cur                 = index.cut_left(pos).cut_right(index.len() - (pos + strlen + 1));
                pos += (strlen + 1); // pos after this string
            }
            else
            {
                *cur = index.cut_left(pos).cut_right(index.len() - (pos + 1));
                pos += 1;
            }
        }

        if (trace)
        {
            std::cout << pos - 1 << "] = " << cur->get_printable() << std::endl;
        }

        idx->addLast(cur);
    }

    return idx;
}

/************************************************************************
 * DEMO only (context should be the MibTable and the current row of interest)
 ************************************************************************/
int main()
{
    {
        std::cout << get_next_avail_index().get_printable() << std::endl;
        std::cout << get_next_avail_index().get_printable() << std::endl;

        Agentpp::Oidx defaults = get_next_avail_index();
        std::cout << defaults.get_printable() << std::endl;

        Agentpp::Array<Agentpp::Oidx>* defaults_indices = getRowIndicesCloned(defaults);
        Agentpp::Oidx* third = defaults_indices->getNth(2); // I want to get only the third subindex
        assert(third->len() == 1);                          // subindex min len is 0
        assert(third->last() == 0);                         // strlen is 0 too in this case
        assert(std::string("")
            == third->as_string(true).get_printable());     // get empty string without length!
        delete defaults_indices;
    }

    // XXX Agentpp::Oidx StringIndex("7.99.111.110.116.101.120.116");
    const char*   firstTestString   = "context";
    const char*   testOidWithoutLen = "1.2.3.4";
    const char*   testOidWithLen    = "4.1.2.3.4";
    const char*   testOidString     = "1.3.6.1.6.3.13.1.2.1";
    const bool    withLength        = true;
    Agentpp::Oidx IpaddressIndex("127.0.0.1");
    Agentpp::Oidx oidWithoutLen(testOidWithoutLen);

    Agentpp::Oidx StringIndex = Agentpp::Oidx::from_string(firstTestString, withLength); // add the len

    assert(std::string(firstTestString)
        == StringIndex.as_string(true).get_printable());         // get back without length!

    Agentpp::Oidx ImpliedStringIndex =
        Agentpp::Oidx::from_string(firstTestString, false);      // without Length, implied!
    assert(std::string(firstTestString)
        == ImpliedStringIndex.as_string(false).get_printable()); // NOTE: the symmetry!

    Agentpp::Oidx OidIndex(testOidString);
    assert(std::string(testOidString) == OidIndex.get_printable());

    // Oidx index = cur.get()->get_index();             // something like [a.b.c.d.e.f]
    Agentpp::Oidx index;
    index += Agentpp::Oidx::from_string(firstTestString, withLength); // index a: variable len string
    index += 2;                                                       // index b
    // XXX index += Agentpp::Oidx(testOidWithLen);       // index c: variable len oid
    index += oidWithoutLen.len(); // index c len has to be explicit set!
    index += oidWithoutLen;       // index c: variable len oid
    index += 4;                   // index d
    // XXX index += Agentpp::Oidx::from_string("1234", false); // index e: without len!
    index += IpaddressIndex;                         // index e
    index += OidIndex;                               // index f: vaiable len oid without len: IMPLIED

    std::cout << index.get_printable() << std::endl; // 7.99.111.110.116.101.120.116.2.2.88.89.4...

    Agentpp::Array<Agentpp::Oidx>* indices = getRowIndicesCloned(index);

    Agentpp::Oidx* first = indices->getNth(0);           // I want to get only the first subindex

    Agentpp::Oidx* second = indices->getNth(1);          // I want to get only the second subindex

    Agentpp::Oidx* third = indices->getNth(2);           // I want to get only the third. subindex

    Agentpp::Oidx* fourth = indices->getNth(3);          // I want to get only the fourth subindex

    Agentpp::Oidx* ipaddress = indices->getNth(4);       // I want to get only the fourth subindex

    Agentpp::Oidx* oid = indices->getNth(INDEX_LEN - 1); // I want to get only the last subindex

    std::cout << "Oidx::get_printable():" << std::endl;
    std::cout << first->get_printable() << '\n'
              << second->get_printable() << '\n'
              << third->get_printable() << '\n'
              << fourth->get_printable() << '\n'
              << ipaddress->get_printable() << '\n'
              << oid->get_printable() << std::endl;

    /**
     * handling with variable string subindex with explicit len at oid[0]
     **/
    std::cout << "Oidx::as_string().get_printable_clear():" << std::endl;
    std::cout << first->as_string().get_printable_clear() << std::endl;   // ".context"; withLength
    assert(StringIndex.len() == (StringIndex[0] + 1));
    assert(strlen(firstTestString)
        == strlen(first->cut_left(1).as_string().get_printable_clear())); // FIXME get without the
                                                                          // first subid: the len
    assert(strlen(firstTestString)
        == strlen(first->as_string(true).get_printable_clear()));         // NOTE: without len
    assert(StringIndex == *first);

    std::cout << second->as_string().get_printable_clear() << std::endl;
    assert(2 == (*second)[0]);

    /**
     * handling with variable oid subindex with explicit len at oid[0]
     **/
    std::cout << third->as_string().get_printable_clear() << std::endl;
    assert(third->len() == ((*third)[0] + 1));
    assert(std::string(testOidWithLen) == third->get_printable());
    assert((strlen(testOidWithoutLen) / 2 + 2) == third->len());
    assert(std::string(testOidWithoutLen)
        == third->cut_left(1).get_printable()); // FIXME get without the first subid: the len

    std::cout << fourth->as_string().get_printable_clear() << std::endl;
    assert(4 == (*fourth)[0]);

    std::cout << ipaddress->as_string().get_printable_clear() << std::endl;
    assert(4 == ipaddress->len());
    assert(IpaddressIndex == *ipaddress);

    {
        // IPv6 address as index samples
        Agentpp::Oidx IpV6addressIndex("253.0.0.0.0.0.0.0.12.150.48.164.1.194.15.47");
        // XXX Agentpp::Oidx IpV6addressIndex("09.08.07.06.05.04.03.02.01.00.255.255.127.0.0.1");
        assert(16 == IpV6addressIndex.len());

        NS_SNMP OctetStr  address = IpV6addressIndex.as_string();
        NS_SNMP IpAddress ipaddr("fd00::c96:30a4:1c2:f2f");
        // XXX NS_SNMP IpAddress ipaddr;
        ipaddr = address;

        void*   addr = &address[0];
        char    ipstr[INET6_ADDRSTRLEN];
        uint8_t buf[sizeof(struct in6_addr)];
        if (ipaddr.valid() && ipaddr.get_inet_address_type() == NS_SNMP Address::e_ipv6)
        {
            std::cout << address.get_printable_hex() << "  ->\t  " << ipaddr.get_printable()
                      << std::endl;
        }

        // InetNtop(address_family, IP_address_in_network_byte_to_convert_to_a_string,
        //         buffer_to_store_the_IP_address_string, the_IP_string_length_in_character);
        if (inet_ntop(AF_INET6, addr, ipstr, sizeof(ipstr)))
        {
            std::cout << IpV6addressIndex.get_printable() << " -> " << ipstr << std::endl;
        }
        else
        {
            perror("inet_ntop()");
        }
    }

    /**
     * handling with variable oid subindex with IMPLIED len
     **/
    std::cout << oid->as_string().get_printable_clear() << std::endl;
    assert(strlen(testOidString) / 2 == oid->len());
    assert(std::string(testOidString) == oid->get_printable());
    assert(OidIndex == *oid);
    assert(index.last() == oid->last()); // IMPLIED: only as last subindex allowed!
    assert(nullptr == indices->getNth(INDEX_LEN));

    delete indices;
    return 0;
}

// Output:

/*************************************
 *  3.1.2.3.1.0.1.1.1.2.3.1
 *  3.1.2.3.1.0.1.1.1.2.3.2
 *  3.1.2.3.1.0.1.1.1.2.3.3
 *  at 0 [0:3] = 3.1.2.3
 *  at 1 [4:4] = 1
 *  at 2 [5:5] = 0
 *  at 3 [6:6] = 1
 *  at 4 [7:10] = 1.1.2.3
 *  at 5 [11:11] = 3
 *  7.99.111.110.116.101.120.116.2.4.1.2.3.4.4.127.0.0.1.1.3.6.1.6.3.13.1.2.1
 *  at 0 [0:7] = 7.99.111.110.116.101.120.116
 *  at 1 [8:8] = 2
 *  at 2 [9:13] = 4.1.2.3.4
 *  at 3 [14:14] = 4
 *  at 4 [15:18] = 127.0.0.1
 *  at 5 [19:28] = 1.3.6.1.6.3.13.1.2.1
 *  Oidx::get_printable():
 *  7.99.111.110.116.101.120.116
 *  2
 *  4.1.2.3.4
 *  4
 *  127.0.0.1
 *  1.3.6.1.6.3.13.1.2.1
 *  Oidx::as_string().get_printable_clear():
 *  .context
 *  .
 *  .....
 *  .
 *  ....
 *  ..........
 *************************************/
