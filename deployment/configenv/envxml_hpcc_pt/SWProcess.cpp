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
#include "SWProcess.hpp"
#include "deployutils.hpp"
#include "configenvhelper.hpp"
#include "buildset.hpp"

namespace ech
{

SWProcess::SWProcess(const char* name, EnvHelper * envHelper)
{
  this->name = name;
  this->envHelper = envHelper;
}

int SWProcess::create(IPropertyTree *params, StringBuffer& errMsg)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
  
  StringBuffer xpath;
  xpath.clear().appendf("./%s/%s/%s/[@name=\"%s\"]", XML_TAG_PROGRAMS, XML_TAG_BUILD, XML_TAG_BUILDSET, name);
  const IPropertyTree * buildSetTree = envHelper->getBuildSet();
  IPropertyTree * pBuildSet = buildSetTree->queryPropTree(xpath.str());

  StringBuffer buildSetPath, sbl, sbNewName;
  const char* buildSetName = name;
  const char* xsdFileName = pBuildSet->queryProp(XML_ATTR_SCHEMA);
  const char* processName = pBuildSet->queryProp(XML_ATTR_PROCESS_NAME);
  StringBuffer deployable = pBuildSet->queryProp("@" TAG_DEPLOYABLE);

  printf("test 3000\n");
  if (processName && *processName && buildSetName && * buildSetName && xsdFileName && *xsdFileName)
  {
    Owned<IPropertyTree> pSchema = loadSchema(
      buildSetTree->queryPropTree("./" XML_TAG_PROGRAMS "/" XML_TAG_BUILD "[1]"), pBuildSet, buildSetPath, NULL);
    sbNewName.clear();
    if (strstr(buildSetName ,"my") == NULL && (strcmp(buildSetName, "topology") != 0))
      sbNewName.append("my");

    IPropertyTree* pCompTree = createPTree(processName);

 
  printf("test 6000\n");
    CConfigEnvHelper configEnv(envTree);
  printf("test 7000 processName=%s, buildSetName=%s\n", processName, buildSetName);
    configEnv.addComponent(buildSetName, sbNewName, pCompTree);
   

  printf("test 8000\n");
 
  }


  return CE_OK;
}


int SWProcess::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate)
{
   return CE_OK;
}
int SWProcess::addNode(IPropertyTree *params, const char* xpath, StringBuffer& errMsg, bool merge)
{
   return CE_OK;
}

int SWProcess::modify(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

int SWProcess::remove(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

IConfigComp* SWProcess::getNodes(StringArray& ipList, const char* clusterName)
{
  return (IConfigComp*)this;
}

int SWProcess::getNumOfNodes(const char* clusterName)
{
  return 1;
}

int SWProcess::addInstance(IPropertyTree *params, StringBuffer& errMsg)
{
  return CE_OK;
}

}
