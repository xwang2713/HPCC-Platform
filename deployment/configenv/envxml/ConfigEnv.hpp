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



class ConfigEnv : public CInterface, implements IConfigEnv
{

public:

   ConfigEnv() {}
   ConfigEnv(IPropertyTree *config);

   IMPLEMENT_IINTERFACE;

   virtual bool create(IPropertyTree *params);
   virtual bool add(IPropertyTree *params);
   virtual bool modify(IPropertyTree *params);
   virtual bool remove(IPropertyTree *params);

   virtual StringBuffer * queryAttribute(StringBuffer *xpath);
   virtual IPropertyTree * query(StringBuffer *xpath);
   
   
private:
   EnvHelper * envHelper;

};

#endif
