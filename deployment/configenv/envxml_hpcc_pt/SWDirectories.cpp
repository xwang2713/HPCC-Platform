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

#include "SWDirectories.hpp"
#include "deployutils.hpp"
#include "configenvhelper.hpp"
#include "buildset.hpp"

namespace ech
{

SWDirectories::SWDirectories(const char* name, EnvHelper * envHelper)
{
  this->name = name;
  this->envHelper = envHelper;
}

int SWDirectories::create(IPropertyTree *params, StringBuffer& errMsg)
{
  return CE_OK;
}


int SWDirectories::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate)
{
   return CE_OK;
}


int SWDirectories::addNode(IPropertyTree *node, const char* xpath, StringBuffer& errMsg, bool merge)
{
   return CE_OK;
}

int SWDirectories::modify(IPropertyTree *params, StringBuffer& errMsg)
{
  synchronized block(mutex);

  IPropertyTree * envTree = envHelper->getEnvTree();
  StringBuffer xpath;

  IPropertyTree * pAttrsTree = params->queryPropTree("Attributes");

  Owned<IPropertyTreeIterator> attrsIter = pAttrsTree->getElements("Attribute");

  ForEach(*attrsIter)
  {
    IPropertyTree* pAttrTree = &attrsIter->query();
    const char* dirName = pAttrTree->queryProp("@name");
    const char* dirPath = pAttrTree->queryProp("@value");

    xpath.clear().appendf(XML_TAG_SOFTWARE"/Directories/Category[@name='%s']", dirName); 

    IPropertyTree* pDir = envTree->queryPropTree(xpath.str());
    if (pDir)
      pDir->setProp("@dir", dirPath);
    else
    {
      pDir = envTree->queryPropTree(XML_TAG_SOFTWARE"/Directories/")->addPropTree("Category", createPTree());
      pDir->setProp(XML_ATTR_NAME, dirName);
      pDir->setProp("@dir", dirPath);
    }
  }

  return CE_OK;
}

int SWDirectories::remove(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

}
