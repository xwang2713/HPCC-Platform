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
#ifndef _EVNGEN2_INCL
#define _ENVGEN2_INCL

//#include <vector>
#include "jliball.hpp"
#include "XMLTags.h"
#include "IConfigEnv.hpp"

using namespace std;

interface IPropertyTree;

//typedef vector<IPropertyTree*> IPropertyTreePtrArray;

class CEnvGen
{

public:
   //CEnvGen(){ iConfigEnv = NULL; };
   bool parseArgs(int argc, char** argv);
   void createUpdateTask(const char* action, IPropertyTree* config, const char* param);
   void usage();
   bool create();

private:

   static map<string, string> envCategoryMap;

   IConfigEnv * iConfigEnv; 
   //Owned<IPropertyTree>  params;
   IPropertyTree *  params;
   
};

#endif
