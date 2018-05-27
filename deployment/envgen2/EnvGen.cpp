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

#include <map>
#include <string>
#include "XMLTags.h"
#include "jliball.hpp"
#include "EnvGen.hpp"


map<string, string> CEnvGen::envCategoryMap =
{
  {""  , "Software"}, 
  {"sw", "Software"}, 
  {"hd", "Hardware"} 
};
  

bool CEnvGen::parseArgs(int argc, char** argv)
{

/*
  IPropertyTree* pBuildSet;
  pBuildSet = createPTree(XML_TAG_BUILDSET);
  pBuildSet->addProp(XML_ATTR_NAME, "mycomp");
  StringBuffer sss;
  toXML(pBuildSet, sss);
  printf("%s\n", sss.str());
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
      toXML(config, s);
      //toJSON(config, s);
      fprintf(stdout, "%s\n",s.str());
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
       config->addProp("@iplist", argv[i++]);
     }
     else if (stricmp(argv[i], "-supportnodes") == 0)
     {
       i++;
       config->addProp("@supportnodes", argv[i++]);
     }
     else if (stricmp(argv[i], "-espnodes") == 0)
     {
       i++;
       config->addProp("@espnodes", argv[i++]);
     }
     else if (stricmp(argv[i], "-roxienodes") == 0)
     {
       i++;
       config->addProp("@roxienodes", argv[i++]);
     }
     else if (stricmp(argv[i], "-thornodes") == 0)
     {
       i++;
       config->addProp("@thornodes", argv[i++]);
     }
     else if (stricmp(argv[i], "-add") == 0)
     {
       i++;
       createUpdateTask("add", config, argv[i++]);
     }
     else if (stricmp(argv[i], "-mod") == 0)
     {
       i++;
       createUpdateTask("modify", config, argv[i++]);
     }
     else if (stricmp(argv[i], "-rmv") == 0)
     {
       i++;
       createUpdateTask("remove", config, argv[i++]);
     }
     else
     {
       printf("\nUnknown option %s\n", argv[i]);
       usage();
       return false;
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

   StringBuffer cfgXML;
   toXML(config, cfgXML.clear());
   printf("%s\n",cfgXML.str());

   return true;
}

void CEnvGen::createUpdateTask(const char* action, IPropertyTree * config, const char* param)
{
   if (!param || !(*param)) return;

   StringArray items;
   items.appendList(param, ":");
   if (items.ordinality() < 3) return;

   IPropertyTree * updateTree =  createPTree("Task");

   updateTree->addProp("@action", action);
   updateTree->addProp("@category", (envCategoryMap.at(items.item(0))).c_str());
   //has @key
   StringArray compAndKey;
   compAndKey.appendList(items[1], "@");
   updateTree->addProp("@component", compAndKey[0]);
   if (compAndKey.ordinality() > 1) 
     updateTree->addProp("@key", compAndKey[1]);
   
   int index = 2;
   if (!stricmp(action, "remove"))
   {
      if (*(items[2])) updateTree->addProp("@target", items[2]);
      index = 3;
   }

   if (items.ordinality() == index) return;

   String s(items[index]);

   if (s.indexOf('=') < 0) 
   {
      updateTree->addProp("@selector", items[index]);
      index++;
   }


   if (items.ordinality() == index) return;

   StringArray attrs;
   attrs.appendList(items[index], ATTR_SEP);
   printf("attribute: %s\n",items[index]);


   IPropertyTree *pAttrs = updateTree->addPropTree("Attributes", createPTree("Attributes"));
   for ( unsigned i = 0; i < attrs.ordinality() ; i++)
   {
     IPropertyTree *pAttr = pAttrs->addPropTree("Attribute", createPTree("Attribute"));

     StringArray keyValues;
     keyValues.appendList(attrs[i], "=");
     pAttr->addProp("@name", keyValues[0]);

     StringArray newOldValues;
     newOldValues.appendList(keyValues[1], ATTR_V_SEP);
     pAttr->addProp("@value", newOldValues[0]);
     if (newOldValues.ordinality() > 1) pAttr->addProp("@oldValue", newOldValues[1]);
   }


   config->addPropTree("Task", updateTree);

   /*
   StringBuffer cfgXML;
   toXML(config, cfgXML.clear());
   printf("%s\n",cfgXML.str());
   */
   
}

bool CEnvGen::create()
{
  
   IPropertyTree* config = params->queryPropTree("Config");
   const char* action = config->queryProp("@action");
   if (stricmp(action, "create") ) return true;
   iConfigEnv->create(config);
   return true;
}

bool CEnvGen::update()
{
   IPropertyTree* config = params->queryPropTree("Config");
   if (stricmp(config->queryProp("@action"), "update")) return true;
   iConfigEnv->dispatchUpdateTasks(config);
   
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
  puts("   -ip  : Ip addresses that should be used for environment generation");
  puts("          Allowed formats are ");
  puts("          X.X.X.X;");
  puts("          X.X.X.X-XXX;");
  puts("   -ipfile: name of the file that contains Ip addresses");
  puts("          Allowed formats are ");
  puts("          X.X.X.X;");
  puts("          X.X.X.X-XXX;");
  puts("   -supportnodes <number of support nodes>: Number of nodes to be used");
  puts("           for non-Thor and non-Roxie components. If not specified or ");
  puts("           specified as 0, thor and roxie nodes may overlap with support");
  puts("           nodes. If an invalid value is provided, support nodes are ");
  puts("           treated to be 0");
  puts("   -roxienodes <number of roxie nodes>: Number of nodes to be generated ");
  puts("          for roxie. If not specified or specified as 0, no roxie nodes");
  puts("          are generated");
  puts("   -thornodes <number of thor nodes>: Number of nodes to be generated ");
  puts("          for thor slaves. A node for thor master is automatically added. ");
  puts("          If not specified or specified as 0, no thor nodes");

  //new options
  puts("   -add category(hd:sw:pg):comp@name(esp|computer):selector(instance|dir|<relative xpath>):attr=value");
  puts("   -mod : Modify an entry. Format: ");
  puts("          category(hd:sw:pg):comp(esp|computer)@(cluster|service):(instance|dir|<xpath>):attr1=value|old");
  puts("          sw:Directories:dir:data=/snap/hpcc/data");
  puts("-rmv category(hd:sw:pg):comp@name(service@ws_ecl|computer@localhost):target<xpath to delete>:selector<condition>(instance|dir|<relative xpath>):attr=value|old");
  puts("   -help: print out this usage.");
}


int main(int argc, char** argv)
{

   InitModuleObjects();

   CEnvGen * envGen = new CEnvGen();
   if (!envGen->parseArgs(argc, argv)) 
     return 1;


   try {
      envGen->create();
   }
   catch (IException* e)
   {
     int errCode = e->errorCode();
     StringBuffer errMsg;
     e->errorMessage(errMsg);
     printf("Error: %d, %s\n", errCode, errMsg.str()); 
     e->Release();
   }
   return 0;
}
