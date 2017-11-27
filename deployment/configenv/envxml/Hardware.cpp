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

#include "Hardware.hpp"

Hardware::Hardware(EnvHelper * envHelper)
{
  this->envHelper = envHelper;
}

bool Hardware::create(IPropertyTree *params)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
  if (envTree == NULL)
     fprintf(stdout, "envTree is NULL\n");

  if (envTree->queryPropTree("./" XML_TAG_HARDWARE))
    throw MakeStringException(-1, "Cannot create Hardware component which  already exists");

  fprintf(stdout, "create hardware section\n");
   
  StringBuffer xpath, sbdefaultValue("");
  Owned<IPropertyTree> pCompTree = createPTree(XML_TAG_HARDWARE);


  IPropertyTree * pComputerType = pCompTree->addPropTree(XML_TAG_COMPUTERTYPE, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pComputerType->addProp(xpath, "linuxmachine");
  xpath.clear().append(XML_ATTR_MANUFACTURER);
  pComputerType->addProp(xpath, "unknown");
  xpath.clear().append(XML_ATTR_COMPUTERTYPE);
  pComputerType->addProp(xpath, "linuxmachine"); 
  xpath.clear().append(XML_ATTR_OPSYS);
  pComputerType->addProp(xpath, "linux");
  xpath.clear().append(XML_ATTR_NICSPEED);
  pComputerType->addProp(xpath, "1000");


  IPropertyTree* pSwitch = pCompTree->addPropTree(XML_TAG_SWITCH, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pSwitch->addProp(xpath, "Switch") ;

  IPropertyTree* pDomain = pCompTree->addPropTree(XML_TAG_DOMAIN, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pDomain->addProp(xpath, "localdomain");


  envTree->addPropTree(XML_TAG_HARDWARE, createPTreeFromIPT(pCompTree)); 

  return true;
}

bool Hardware::add(IPropertyTree *params)
{
   return true;
}

bool Hardware::modify(IPropertyTree *params)
{
   return true;
}

bool Hardware::remove(IPropertyTree *params)
{
   return true;
}

