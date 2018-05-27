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
#include "ComponentBase.hpp"

namespace ech
{

class Hardware : public ComponentBase
{
public:
   static const char* TYPE;
   static const char* MAKER;
   static const char* SPEED;
   static const char* DOMAIN;
   static const char* OS;

   Hardware(EnvHelper * envHelper);

   virtual void create(IPropertyTree *params);
   virtual int add(IPropertyTree *params);
   //virtual int addNode(IPropertyTree *node, const char* xpath, bool merge);
   //virtual int modify(IPropertyTree *params);
   //virtual void remove(IPropertyTree *params);

   const char* getComputerName(const char* netAddress);
   const char* getComputerNetAddress(const char* name);
   IPropertyTree* addComputer(IPropertyTree *params);
   IPropertyTree* addComputerType(IPropertyTree *params);
   IPropertyTree* addDomain(IPropertyTree *params);
   //IPropertyTree* addSwitch(IPropertyTree *params);

private:
   StringArray notifyUpdateList;
   StringArray notifyAddList;
   
};

}

#endif
