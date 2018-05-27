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

#include "Software.hpp"
#include "SWProcess.hpp"
#include "SWThorCluster.hpp"
#include "SWRoxieCluster.hpp"
#include "SWEspProcess.hpp"
#include "SWDirectories.hpp"
#include "deployutils.hpp"

namespace ech
{

Software::Software(EnvHelper * envHelper)
{
  this->envHelper = envHelper;
}

void Software::create(IPropertyTree *params)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
   
  const IPropertyTree * buildSetTree = envHelper->getBuildSetTree();
  envTree->addPropTree(XML_TAG_SOFTWARE, createPTreeFromIPT(
    buildSetTree->queryPropTree("./" XML_TAG_SOFTWARE)));

  getSWComp("esp")->create(params);

  StringBuffer xpath;
  xpath.clear().appendf("./%s/%s/%s", XML_TAG_PROGRAMS, XML_TAG_BUILD, XML_TAG_BUILDSET);
    Owned<IPropertyTreeIterator> buildSetInsts = buildSetTree->getElements(xpath.str());

  const GenEnvRules& rules = envHelper->getGenEnvRules();
  
  ForEach(*buildSetInsts)
  {
    IPropertyTree* pBuildSet = &buildSetInsts->query();
    const char* buildSetName = pBuildSet->queryProp(XML_ATTR_NAME);
    if (stricmp(buildSetName, "esp") == 0) continue;
    if (rules.foundInProp("do_not_generate", buildSetName)) continue;

    getSWComp(buildSetName)->create(params);
  }

}


int Software::add(IPropertyTree *params)
{
  const char *comp    = params->queryProp("@component");
  return getSWComp(comp)->add(params);
}

//int Software::addNode(IPropertyTree *node, const char *xpath, bool merge)
//{
//   return 0;
//}


/*
// This should go to SWProcess class
int Software::addInstance(IPropertyTree *params)
{
   IPropertyTree *pAttrsTree = params->queryPropTree("Attributes");
   assert(pAttrsTree); 

   const char* ipInfo;
   bool isFile = false; 
   StringBuffer xpath;
   xpath.clear().append("./Attrubte/@ip");
   if (pAttrsTree->hasProp(xpath))
   {
     ipInfo = pAttrsTree->queryProp(xpath);
   }
   else
   {
     xpath.clear().append("./Attrubte/@ipFile");
     ipInfo = pAttrsTree->queryProp(xpath);
     isFile = true;
   }

   if (!ipInfo || !(*ipInfo))
   {
      throw MakeStringException(CfgEnvErrorCode::NoIPAddress, "No ip address provided for addInstance"); 
   }

   StringArray ipArray;
   if (envHelper->processNodeAddress(ipInfo, ipArray, isFile) == 0) 
   {
      throw MakeStringException(CfgEnvErrorCode::NoIPAddress, "No ip address provided for addInstance"); 
   }

   //add computer and get computer name
   //add instance for on all nodes
   

   //const char *target  = params->queryProp("@component");

   //query id 
   return 0;
}
*/

int Software::modify(IPropertyTree *params)
{
   //IPropertyTree * envTree = envHelper->getEnvTree();
   const char *comp = params->queryProp("@component");
   return getSWComp(comp)->modify(params);
}


void Software::remove(IPropertyTree *params)
{
   const char *comp = params->queryProp("@component");
   return getSWComp(comp)->remove(params);
}

IConfigComp* Software::getSWComp(const char *compName)
{
   //should call envHelper->getXMLTagName(compName)
   //const char *compNameLC =  envHelper->getXMLTagName(compName);

   const char * compNameLC = (StringBuffer(compName).toLowerCase()).str();

   IConfigComp ** ppComp = swCompMap.getValue(compNameLC);
   if (ppComp && *ppComp) return *ppComp;

   IConfigComp *pComp = NULL;

   
   /*
      directories

comp: dafilesrv
comp: dali
comp: dfuserver
comp: DropZone
comp: eclagent
comp: eclminus
comp: eclccserver
comp: eclscheduler
comp: esp
comp: espsmc
comp: ws_ecl
comp: dynamic_esdl
comp: ftslave
comp: hqltest
comp: roxie
comp: sasha
comp: thor
comp: topology
   */

   if (!stricmp(compNameLC, "directories")  || !stricmp(compNameLC, "dirs"))
   {
      pComp = (IConfigComp*) new SWDirectories("directories", envHelper);
   }
   else if (!stricmp(compNameLC, "roxie") || !stricmp(compNameLC, "RoxieCluster")) 
   {
      pComp = (IConfigComp*) new SWRoxieCluster("roxie", envHelper);
   }
   else if (!stricmp(compNameLC, "thor") || !stricmp(compNameLC, "ThorCluster")) 
   {
      pComp = (IConfigComp*) new SWThorCluster("thor", envHelper);
   }
   else if (!stricmp(compNameLC, "esp") || !stricmp(compNameLC, "EspProcess")) 
   {
      pComp = (IConfigComp*) new SWEspProcess("esp", envHelper);
   }
   else if (!stricmp(compNameLC, "srv") || !stricmp(compNameLC, "EspService") ||
            !stricmp(compNameLC, "elcwatch") || !stricmp(compNameLC, "espsmc")) 
   {
      pComp = (IConfigComp*) new SWEspProcess("espsmc", envHelper);
   }
   else
   {
      pComp = (IConfigComp*) new SWProcess(compNameLC, envHelper);
   }

   if (pComp != NULL)
   {
      swCompMap.setValue(compName,  pComp);
   }

  
   return pComp;
}

}
