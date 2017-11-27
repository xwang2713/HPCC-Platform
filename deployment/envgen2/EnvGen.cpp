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

#include "XMLTags.h"
#include "jliball.hpp"
#include "EnvGen.hpp"


bool CEnvGen::parseArgs(int argc, char** argv)
{

/*
  IPropertyTree* pBuildSet;
  pBuildSet = createPTree(XML_TAG_BUILDSET);
  pBuildSet->addProp(XML_ATTR_NAME, "mycomp");
  StringBuffer sss;
  toXML(pBuildSet, sss);
  fprintf(stdout, "%s\n", sss.str());
   exit(0);
*/

   int i = 1;

   //params.setown(createPTree());
   params = createPTree("Env");
   //Owned<IPropertyTree> config = createPTree("Config");
   IPropertyTree * config = createPTree("Config");
   /*
   StringBuffer s;
   if (config)
   {
      fprintf(stdout, "test ... \n");
      //toXML(config, s);
      //toJSON(config, s);
   }
   */

   params->addPropTree("Config", config);

   while (i < argc)
   {
     if (stricmp(argv[i], "-help") == 0 || stricmp(argv[i], "-?") == 0)
     {
       usage();
       return false;
     }
     else if (stricmp(argv[i], "-env") == 0)
     {
       i++;
       config->addProp("@env", argv[i++]);
     }
     else if (stricmp(argv[i], "-ip") == 0)
     {
       i++;
       config->addProp("ipAddrs", argv[i++]);
     }
   }

   if (!config->queryProp("@env"))
   {
      fprintf(stderr, "\nMissing -env\n");
      usage();
      return false;
   }
   
   if (!config->queryProp("@action"))
       config->addProp("@action", "create");


   iConfigEnv =  ConfigEnvFactory::getIConfigEnv(config);

   return true;
}

bool CEnvGen::create()
{
  
   iConfigEnv->create(params->queryPropTree("Config"));
   return true;
}

void CEnvGen::usage()
{
  const char* version = "0.1";
  printf("\nHPCC Systems® environment generator. version %s. Usage:\n", version);
  puts("   envgen -env <environment file> -ip <ip addr> [options]");
  puts("");
  puts("options: ");
  puts("   -env : Full path of the environment file that will be generated.");
  puts("          If a file with the same name exists, and no \"-update\" provided"); 
  puts("          a new name with _xxx will be generated ");
  puts("   -help: print out this usage.");
}


int main(int argc, char** argv)
{

   InitModuleObjects();

   CEnvGen * envGen = new CEnvGen();
   if (!envGen->parseArgs(argc, argv)) 
     return 1;

   envGen->create();

   return 0;
}
