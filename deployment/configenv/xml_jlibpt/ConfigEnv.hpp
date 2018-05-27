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
#ifndef _CONFIGENV_HPP_
#define _CONFIGENV_HPP_

#include "EnvHelper.hpp"
#include "IConfigEnv.hpp"

namespace ech
{

class ConfigEnv : public CInterface, implements IConfigEnv<IPropertyTree, StringBuffer>
{

public:

   ConfigEnv() {}
   ConfigEnv(IPropertyTree *config);

   IMPLEMENT_IINTERFACE;

   virtual void create(IPropertyTree *params);
   virtual int add(IPropertyTree *params);
   //virtual int addNode(IPropertyTree *node, const char* xpath, bool merge=false);
   virtual int modify(IPropertyTree *params);
   virtual void remove(IPropertyTree *params);
   virtual void dispatchUpdateTasks(IPropertyTree *params);

   virtual const char* queryAttribute(const char *xpath);
   virtual IPropertyTree* getNode(const char *xpath);
   virtual void getContent(const char* xpath, StringBuffer& out, int format=XML_SortTags|XML_Format);
   virtual void addContent(const char* xpath, StringBuffer& in, int type=XML_Format);

   
   
private:
   EnvHelper * envHelper;

};

}

#endif
