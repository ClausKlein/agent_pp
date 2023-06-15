/*_############################################################################
 * _##
 * _##  AGENT++ 4.5 - tools.h
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

#ifndef tools_h_
#define tools_h_

#include <agent_pp/agent++.h>

#include <ctime>
#include <snmp_pp/octet.h>
#include <string>
#include <sys/types.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
using namespace Snmp_pp;
#endif

class AGENTPP_DECL AgentTools {
public:
    /**
     * Return a new string that is the concatenation of two given strings.
     *
     * @param prefix - The prefix string.
     * @param suffix - The suffix string.
     * @return A new string.
     */
    static char* make_concatenation(const char*, const char*);

    /**
     * Return the file size of a given file.
     *
     * @return The file size.
     */
    static long file_size(FILE*);

    /**
     * Create the directories of a path if not present.
     * @param path
     *    the directory path to create if not present.
     * @return
     *    true if the directory path is now present, false otherwise.
     * @since 4.3.0
     */
    static bool make_path(const std::string&);
};

///////////////////////////////////////////////////////////////////////////////
// class Timer
//

class AGENTPP_DECL Timer {
public:
    Timer() : timestamp(0), lifetime(0) { }

    Timer(time_t life) : timestamp(0), lifetime(life) { }

    time_t get_life() const { return lifetime; }

    void set_life(time_t sec) { lifetime = sec; }

    bool   in_time();
    time_t due_time();
    bool   in_time(int frac);

    void set_timestamp() { time(&timestamp); }

    time_t get_timestamp() const { return timestamp; }

protected:
    time_t timestamp;
    time_t lifetime;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
