/*_############################################################################
  _## 
  _##  AGENT++ 4.5 - WIN32DLL.h  
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
#ifndef win32dll_h_
#define win32dll_h_

#include <agent_pp/List.h>

class MibContext;
class MibTable;
class MibTableRow;
class MibTableVoter;
class MibEntry;
class MibLeaf;
class ProxyForwarder;

AGENTPP_TEMPL template class AGENTPP_DECL ListCursor<MibEntry>;
AGENTPP_TEMPL template class AGENTPP_DECL OrderedList<MibEntry>;
AGENTPP_TEMPL template class AGENTPP_DECL List<MibEntry>;

AGENTPP_TEMPL template class AGENTPP_DECL OidList<MibTableRow>;
AGENTPP_TEMPL template class AGENTPP_DECL OrderedList<MibTableRow>;
AGENTPP_TEMPL template class AGENTPP_DECL OrderedList<MibLeaf>;
AGENTPP_TEMPL template class AGENTPP_DECL	List<MibTableRow>;
AGENTPP_TEMPL template class AGENTPP_DECL	List<MibLeaf>;
AGENTPP_TEMPL template class AGENTPP_DECL	List<MibTable>;
AGENTPP_TEMPL template class AGENTPP_DECL	List<MibTableVoter>;

#ifdef USE_ARRAY_TEMPLATE
	AGENTPP_TEMPL template class AGENTPP_DECL OrderedArray<MibLeaf>;
	AGENTPP_TEMPL template class AGENTPP_DECL Array<MibLeaf>;
#else
	AGENTPP_TEMPL template class AGENTPP_DECL OrderedList<MibLeaf>;
	AGENTPP_TEMPL template class AGENTPP_DECL List<MibLeaf>;
#endif

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER
AGENTPP_TEMPL template class AGENTPP_DECL OidList<MibContext>;
AGENTPP_TEMPL template class AGENTPP_DECL	OidList<ProxyForwarder>;
#endif
#endif

#endif
