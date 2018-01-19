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
#include "SWDirectories.hpp"
#include "deployutils.hpp"

namespace ech
{

Software::Software(EnvHelper * envHelper)
{
  this->envHelper = envHelper;
}

int Software::create(IPropertyTree *params, StringBuffer& errMsg)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
   
  const IPropertyTree * buildSetTree = envHelper->getBuildSet();
  envTree->addPropTree(XML_TAG_SOFTWARE, createPTreeFromIPT(
    buildSetTree->queryPropTree("./" XML_TAG_SOFTWARE)));

  int rc;
  rc = getSWComp("esp")->create(params, errMsg);
  if (rc != CE_OK) return rc;

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

    rc = getSWComp(buildSetName)->create(params, errMsg);
    if (rc != CE_OK) return rc;

  }

  return CE_OK;
}


int Software::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate)
{
    //if add instance
	//  add to Hardware computer
	//  add to component needed on all nodes
	//endif

	return CE_OK;
}

int Software::addNode(IPropertyTree *node, const char *xpath, StringBuffer& errMsg, bool merge)
{
   return CE_OK;
}


int Software::addInstance(IPropertyTree *params, StringBuffer& errMsg)
{
   IPropertyTree *pAttrsTree = params->queryPropTree("Attributes");
   if (pAttrsTree == NULL) 
   {
     errMsg.append("Params property tree of  addInstance is null.");
     return CEERR_NullPointer;
   }

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
      errMsg.append("No ip address provided for addInstance"); 
      return CEERR_NoIPAddress;
   }

   StringArray ipArray;
   if (envHelper->processNodeAddress(ipInfo, ipArray, isFile) == 0) 
   {
      errMsg.append("No ip address provided for addInstance"); 
      return CEERR_NoIPAddress;
   }

   

     //add computer and get computer name
     //add instance for on all nodes
   

   //const char *target  = params->queryProp("@component");
   return CE_OK;
}



int Software::modify(IPropertyTree *params, StringBuffer& errMsg)
{
  IPropertyTree * envTree = envHelper->getEnvTree();

  //const char *action  = params->queryProp("@action");  
  const char *comp    = params->queryProp("@target");
  const char *xpath   = params->queryProp("@xpath");


  if (stricmp(xpath, "instance") == 0)
  {
     return ((SWProcess*)getSWComp(comp))->addInstance(params, errMsg);
  }
  else
  {
    const char *target  = params->queryProp("@component");
    return getSWComp(comp)->modify(params, errMsg);
  }
}


int Software::remove(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

IConfigComp* Software::getSWComp(const char *compName)
{
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
comp: ftslave
comp: hqltest
comp: roxie
comp: sasha
comp: thor
comp: topology
comp: ws_ecl
   */

   if ((stricmp(compNameLC, "Directories") == 0) || (stricmp(compNameLC, "dirs") == 0))
   {
      pComp = (IConfigComp*) new SWDirectories(compName, envHelper);
   }
   else if (stricmp(compNameLC, "yyyy") == 0)
   {
      pComp = (IConfigComp*) new SWProcess(compName, envHelper);
   }
   else
   {
      pComp = (IConfigComp*) new SWProcess(compName, envHelper);
   }

   if (pComp != NULL)
   {
      swCompMap.setValue(compName,  pComp);
   }


   return pComp;
}

}
