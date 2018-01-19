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

#include "EnvHelper.hpp"
#include "ConfigEnv.hpp"
#include "deployutils.hpp"
#include "build-config.h"


namespace ech
{

ConfigEnv::ConfigEnv(IPropertyTree *config)
{
   envHelper = new EnvHelper(config);
}

int ConfigEnv::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicdate)
{
   return CE_OK;
}

int ConfigEnv::addNode(IPropertyTree *node, const char* xpath, StringBuffer& errMsg, bool merge)
{
   return CE_OK;
}

int ConfigEnv::modify(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

int ConfigEnv::create(IPropertyTree *params, StringBuffer& errMsg) 
{

   StringBuffer xpath, sbTemp;
   fprintf(stdout, "create environment.xml ...\n");
   //Process ips
   envHelper->processNodeAddress(params);
   int rc;

   if (USE_WIZARD)
   {
     StringBuffer optionsXml, envXml;
     const char* pServiceName = "WsDeploy_wsdeploy_esp";
     int roxieNodes=1, thorNodes=1, slavesPerNode=1, supportNodes=1, espNodes=1;
     bool roxieOnDemand = true;
     MapStringTo<StringBuffer> dirMap;
     Owned<IPropertyTree> pCfg = createPTreeFromXMLFile(ESP_CONFIG_PATH);
     StringArray arrAssignIPRanges;
     StringArray arrBuildSetWithAssignedIPs;

     if (params->hasProp("roxieNodes")) 
       roxieNodes = params->getPropInt("roxieNodes", 1);

     if (params->hasProp("thorNodes")) 
       thorNodes = params->getPropInt("thorNodes", 1);

     if (params->hasProp("slavesPerNode")) 
       slavesPerNode = params->getPropInt("slavesPerNode", 1);

     if (params->hasProp("supportNodes")) 
       supportNodes = params->getPropInt("supportNodes", 1);

     if (params->hasProp("espNodes")) 
       espNodes = params->getPropInt("espNodes", 1);

     if (params->hasProp("roxieOnDemand")) 
       roxieOnDemand = params->getPropBool("roxieOnDemand", true);

     StringBuffer ipAddr;
     const StringArray& ipList = envHelper->getNodeList();

     ipAddr.clear();
     ForEachItemIn(ip, ipList)
     {
       if (ipAddr.length() > 0)
         ipAddr.append(";");

       ipAddr.append(ip);
     }
     optionsXml.appendf("<XmlArgs supportNodes=\"%d\" roxieNodes=\"%d\" thorNodes=\"%d\" espNodes=\"%d\" slavesPerNode=\"%d\" roxieOnDemand=\"%s\" ipList=\"%s\"/>", supportNodes, roxieNodes,
      thorNodes, espNodes, slavesPerNode, roxieOnDemand?"true":"false", ipAddr.str());

     buildEnvFromWizard(optionsXml, pServiceName, pCfg, envXml, arrBuildSetWithAssignedIPs, arrAssignIPRanges, &dirMap);

     envHelper->setEnvTree(envXml);

   }
   else
   { 

     //Hardware
     rc = envHelper->getEnvComp("Hardware")->create(params, errMsg);
     if (rc != CE_OK) return rc; 
   
     //Programs
     rc = envHelper->getEnvComp("Programs")->create(params, errMsg);
     if (rc != CE_OK) return rc; 

     //EnvSettings
     rc = envHelper->getEnvComp("EnvSettings")->create(params, errMsg);
     if (rc != CE_OK) return rc; 

     //Software
     rc = envHelper->getEnvComp("Software")->create(params, errMsg);
     if (rc != CE_OK) return rc; 
   }

   rc = dispatchUpdateTasks(params, errMsg);
   if (rc != CE_OK) return rc; 


   const IPropertyTree * envTree = envHelper->getEnvTree();


   //output
   StringBuffer env, envXml; 
   toXML(envTree, envXml, 0, XML_SortTags | XML_Format);
   env.clear().appendf("<" XML_HEADER ">\n");
   env.append(envXml);

   Owned<IFile> pFile;
   const char* envFile = params->queryProp("@env");

   printf("output envxml to file %s\n", envFile);

   pFile.setown(createIFile(envFile));

   Owned<IFileIO> pFileIO;
   pFileIO.setown(pFile->open(IFOcreaterw));
   pFileIO->write(0, env.length(), env.str());

   return CE_OK;
}


int ConfigEnv::dispatchUpdateTasks(IPropertyTree *params, StringBuffer& errMsg)
{
   Owned<IPropertyTreeIterator> iter = params->getElements("Update");

   if (!iter) return CE_OK;

   int rc = CE_OK;

   ForEach(*iter)
   {
     IPropertyTree* updateTree = &iter->query();
     const char * action = updateTree->queryProp("@action"); 
     const char * category = updateTree->queryProp("@category"); 

     IConfigComp *comp =  envHelper->getEnvComp(category);

     if (!comp)
     {
        errMsg.appendf( "Cannot create component %s", category);
        return CEERR_CannotCreateCompoent;
     }

     rc = CE_OK;
     if (!stricmp(action, "add"))
     { 
        StringBuffer s;
        rc = comp->add(updateTree, errMsg, s, false);
     }
     else if (!stricmp(action, "modify"))
        rc = comp->modify(updateTree, errMsg);
     else if (!stricmp(action, "remove"))
        rc = comp->remove(updateTree, errMsg);
     else
       errMsg.appendf( "Unknown action %s on compoent %s", action, category);

   }

   return rc;
}

int ConfigEnv::remove(IPropertyTree *params, StringBuffer& errMsg)
{
  return CE_OK;
}

const char * ConfigEnv::queryAttribute(const char *xpath)
{
   return NULL;
}

IPropertyTree * ConfigEnv::getNode(const char *xpath)
{
   return NULL;
}


int ConfigEnv::getContent(StringBuffer& out, int format)
{
  return CE_OK;
}

}
