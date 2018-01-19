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

#include "Programs.hpp"
#include "deployutils.hpp"

namespace ech
{

Programs::Programs(EnvHelper * envHelper)
{
  this->envHelper = envHelper;
}

int Programs::create(IPropertyTree *params, StringBuffer& errMsg)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
   
  Owned<IPropertyTree> pProgramTree = createPTreeFromIPT(envHelper->getBuildSet());
  envTree->addPropTree(XML_TAG_PROGRAMS, createPTreeFromIPT(pProgramTree->queryPropTree("./" XML_TAG_PROGRAMS)));

  return CE_OK;
}


int Programs::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate)
{
   return CE_OK;
}

int Programs::addNode(IPropertyTree *node, const char* xpath,  StringBuffer& errMsg, bool mer)
{
   return CE_OK;
}

int Programs::modify(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

int Programs::remove(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

}
