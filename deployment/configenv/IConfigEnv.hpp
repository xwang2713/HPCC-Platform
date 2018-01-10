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

#ifdef CONFIGENV_EXPORTS
  #define configenv_decl DECL_EXPORT
#else
  #define configenv_decl DECL_IMPORT
#endif

interface IPropertyTree;
interface IEnvSettings;

interface IConfigComp : public IInterface
{
  virtual IConfigComp* create(IPropertyTree *params) = 0;
  virtual IConfigComp* add(IPropertyTree *params) = 0;
  virtual IConfigComp* modify(IPropertyTree *params) = 0;
  virtual IConfigComp* remove(IPropertyTree *params) = 0;
  
};



//interface configenv_decl IConfigEnv: extends IConfigComp
interface configenv_decl IConfigEnv
//interface IConfigEnv: extends IConfigComp
{
  virtual IConfigEnv* create(IPropertyTree *params) = 0;
  virtual IConfigEnv* add(IPropertyTree *params) = 0;
  virtual IConfigEnv* modify(IPropertyTree *params) = 0;
  virtual IConfigEnv* remove(IPropertyTree *params) = 0;
  virtual StringBuffer * queryAttribute(const char *xpath) = 0;
  virtual IPropertyTree * query(const char *xpath) = 0;
  virtual IConfigEnv* dispatchUpdateTasks(IPropertyTree *params) = 0;
};


class configenv_decl ConfigEnvFactory
{
public:
  static IConfigEnv * getIConfigEnv(IPropertyTree *config);
};

#endif
