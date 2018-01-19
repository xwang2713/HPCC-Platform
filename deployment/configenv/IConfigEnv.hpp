/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */


#ifndef _ICONFIGENV_HPP_
#define _ICONFIGENV_HPP_

#include "ConfigEnvError.hpp"

#ifdef CONFIGENV_EXPORTS
  #define configenv_decl DECL_EXPORT
#else
  #define configenv_decl DECL_IMPORT
#endif

//Output format
//XML_SortTags | XML_Format

#ifndef ATTR_SEP 
#define ATTR_SEP  "^"
#endif

#ifdef ATTR_V_SEP
#define ATTR_V_SEP "|"
#endif


interface IPropertyTree;
interface IEnvSettings;


//interface configenv_decl IConfigEnv: extends IConfigComp

template <class PTYPE, class SB>
interface configenv_decl IConfigEnv
{
  virtual int create(PTYPE *params, SB& errMsg) = 0;
  virtual int add(PTYPE *params, SB& errMsg, SB& name, bool duplicate) = 0;
  virtual int addNode(PTYPE *node, const char *xpath, SB& errMsg, bool merge) = 0;
  virtual int modify(PTYPE *params, SB& errMsg) = 0;
  virtual int remove(PTYPE *params, SB& errMsg) = 0;
  virtual const char* queryAttribute(const char *xpath) = 0;
  virtual PTYPE * getNode(const char *xpath) = 0;
  //virtual bool validate(const char *xpath, const char value) = 0;
  virtual int getContent(SB& out, int format) = 0;
  virtual int dispatchUpdateTasks(PTYPE *params, SB& errMsg) = 0;
  // validate value from env tree or schema, xpath and schema only provide one usually
  //virtual bool isValid(const char* xpath, const char* schema, const char* key, const char* value, bool src);
};


/*
template <class PTYPE, class SB>
class configenv_decl ConfigEnvFactory
{
public:
  static IConfigEnv<PTYPE,SB> * getIConfigEnv(PTYPE *config);
};
*/

#endif
