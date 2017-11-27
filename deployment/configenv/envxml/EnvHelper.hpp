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
#ifndef _ENVHELPER_HPP_
#define _ENVHELPER_HPP_

#include "jiface.hpp"
#include "jliball.hpp"
#include "XMLTags.h"
#include "IConfigEnv.hpp"

//interface IConfigComp;
//interface IConfigEnv;


class EnvConfigOptions
{
public:
   EnvConfigOptions(const char* filename) { loadFile(filename); }
   void loadFile(const char* filename);

private:
   IProperties * options;
};

class GenEnvRules
{
public:
   GenEnvRules(const char* filename) { loadFile(filename); }
   void loadFile(const char* filename);

private:
   IProperties * rules;
};


class EnvHelper 
{
public: 
   EnvHelper(IPropertyTree * config);
   const EnvConfigOptions& getEnvConfigOptions() const { return *envCfgOptions; }
   const GenEnvRules& getGenEnvRules() const { return *genEnvRules; }
   const IPropertyTree * getBuildSet() const { return buildSet;}
   IPropertyTree * getEnvTree() { return envTree; }
   IConfigComp* getEnvComp(const char * compName);

private:
   void init(IPropertyTree * config);

   IPropertyTree * envTree;
   IPropertyTree * buildSet;
   EnvConfigOptions *envCfgOptions;
   GenEnvRules *genEnvRules;

   IConfigComp * hardware;
   IConfigComp * software;
   IConfigComp * settings;
   IConfigComp * programs;
};

#endif
