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
#ifndef _SWPROCESS_HPP_
#define _SWPROCESS_HPP_

#include "EnvHelper.hpp"

namespace ech
{

class SWProcess : public CInterface, implements IConfigComp
{
public:
   SWProcess(const char* name, EnvHelper * envHelper);

   IMPLEMENT_IINTERFACE;

   virtual int create(IPropertyTree *params, StringBuffer& errMsg);
   virtual int add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate);
   virtual int addNode(IPropertyTree *params, const char* xpath, StringBuffer& errMsg, bool merge);
   virtual int modify(IPropertyTree *params, StringBuffer& errMsg);
   virtual int remove(IPropertyTree *params, StringBuffer& errMsg);
   virtual int addInstance(IPropertyTree *params, StringBuffer& errMsg);

   IConfigComp* getNodes(StringArray& ipList, const char* clusterName=NULL);
   int getNumOfNodes(const char* clusterName=NULL);

private:
   Mutex mutex;
   EnvHelper * envHelper;
   const char* name;
   
};

}
#endif
