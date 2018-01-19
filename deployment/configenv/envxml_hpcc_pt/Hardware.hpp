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
#ifndef _HARDWARE_HPP_
#define _HARDWARE_HPP_

#include "EnvHelper.hpp"

namespace ech
{

class Hardware : public CInterface, implements IConfigComp
{
public:
   Hardware(EnvHelper * envHelper);

   IMPLEMENT_IINTERFACE;

   virtual int create(IPropertyTree *params, StringBuffer& errMsg);
   virtual int add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate);
   virtual int addNode(IPropertyTree *node, const char* xpath, StringBuffer& errMsg, bool merge);
   virtual int modify(IPropertyTree *params, StringBuffer& errMsg);
   virtual int remove(IPropertyTree *params, StringBuffer& errMsg);

   IPropertyTree* addComputer(const char* ip, const char* namePrefix,
              const char* name, const char* type, const char* domain);
   const char* getComputerName(const char* netAddress);
   const char* getComputerNetAddress(const char* name);
   IPropertyTree* addComputerType(const char* name, const char * type="linuxmachine",
                  const char* manufacturer="unknown", const char* speed="1000", const char* os="linux" );

private:
   Mutex mutex;
   EnvHelper * envHelper;
   
};

}

#endif
