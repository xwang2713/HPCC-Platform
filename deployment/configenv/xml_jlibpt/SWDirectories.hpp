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
#ifndef _SWDIRECTORIES_HPP_
#define _SWDIRECTORIES_HPP_

#include "EnvHelper.hpp"

namespace ech
{

class SWDirectories : public CInterface, implements IConfigComp
{
public:
   SWDirectories(const char* name, EnvHelper * envHelper);

   IMPLEMENT_IINTERFACE;

   virtual void create(IPropertyTree *params);
   virtual int add(IPropertyTree *params);
   //virtual int addNode(IPropertyTree *params, const char* xpath, bool merge);
   virtual int modify(IPropertyTree *params);
   virtual void remove(IPropertyTree *params);


private:
   Mutex mutex;
   EnvHelper * envHelper;
   const char* name;
   
};

}

#endif
