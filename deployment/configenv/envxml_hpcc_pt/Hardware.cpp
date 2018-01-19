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
#include "deployutils.hpp"

namespace ech
{

Hardware::Hardware(EnvHelper * envHelper)
{
  this->envHelper = envHelper;
}

int Hardware::create(IPropertyTree *params, StringBuffer& errMsg)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
  if (envTree == NULL)
    throw MakeStringException(-1, "Environment tree is NULL");

  if (envTree->queryPropTree("./" XML_TAG_HARDWARE))
  {
     errMsg.append("Cannot create Hardware component which  already exists");
     return CEWRN_ComponentExists;
  }

  fprintf(stdout, "create hardware section\n");
   
  StringBuffer xpath, sbdefaultValue("");
  Owned<IPropertyTree> pCompTree = createPTree(XML_TAG_HARDWARE);

  IPropertyTree* pSwitch = pCompTree->addPropTree(XML_TAG_SWITCH, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pSwitch->addProp(xpath, "Switch") ;

  IPropertyTree* pDomain = pCompTree->addPropTree(XML_TAG_DOMAIN, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pDomain->addProp(xpath, "localdomain");

  envTree->addPropTree(XML_TAG_HARDWARE, createPTreeFromIPT(pCompTree)); 

  addComputerType("linuxmachine");

  const StringArray& ipArray = envHelper->getNodeList(); 
  for (unsigned i = 0; i < ipArray.ordinality(); i++)
  {
     addComputer(ipArray.item(i), NULL,  NULL, NULL, NULL);
  }


  return CE_OK;
}

IPropertyTree* Hardware::addComputer(const char* ip, const char* namePrefix, 
      const char* name, const char* type, const char* domain)
{

  StringBuffer sbIp, sbType, sbDomain, sbName;
  IpAddress ipAddr; 

  sbIp.clear();
  if ((ip == NULL) || (ip == "")) 
  {
    if ((name == NULL) || (name == ""))
      throw MakeStringException( -1 , "Cannot add a computer without both ip and name.");
    else
    {
      ipAddr.ipset(name);
      ipAddr.getIpText(sbIp);
    }
  }
  else
    sbIp.append(ip);

  sbType.clear();
  ((type == NULL) || (type == ""))? sbType.append("linuxmachine"):sbType.append(type);

  sbDomain.clear();
  ((domain == NULL) || (domain == ""))? sbDomain.append("localdomain"):sbType.append(domain);


  IPropertyTree* envTree = envHelper->getEnvTree();

  StringBuffer xpath;

  synchronized block(mutex);

  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  assert(pHardwareTree != NULL);

  xpath.clear().appendf(XML_TAG_COMPUTER"[@netAddress=\"%s\"]", sbIp.str()); 
  IPropertyTree* pComputer = pHardwareTree->queryPropTree(xpath);
  if (pComputer)
    return pComputer;

  sbName.clear();
  if ((name == NULL) || (name == ""))
  {
    if (ip && (ip == "."))
       sbName.append("localhost");
    else
    {
      if ((namePrefix == NULL) || (namePrefix == ""))
      {
        unsigned x;
        ipAddr.ipset(sbIp.str());
        ipAddr.getNetAddress(sizeof(x), &x);
        sbName.appendf("node%03d%03d", (x >> 16) & 0xFF, (x >> 24) & 0xFF);
      }
      else
        sbName.append(namePrefix);

      getUniqueName(pHardwareTree, sbName, XML_TAG_COMPUTER, "");
    }
  }
  else
  {
    sbName.append(name);
  }

  pComputer = pHardwareTree->addPropTree(XML_TAG_COMPUTER,createPTree());
  pComputer->addProp(XML_ATTR_COMPUTERTYPE, sbType.str());
  pComputer->addProp(XML_ATTR_DOMAIN, sbDomain.str());
  pComputer->addProp(XML_ATTR_NAME, sbName.str());
  pComputer->addProp(XML_ATTR_NETADDRESS, sbIp.str());

  return pComputer;
}

int Hardware::add(IPropertyTree *params, StringBuffer& errMsg, StringBuffer& name, bool duplicate)
{
   return CE_OK;
}

int Hardware::addNode(IPropertyTree *node, const char* xpath, StringBuffer& errMsg, bool merge)
{
   return CE_OK;
}

int Hardware::modify(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}

int Hardware::remove(IPropertyTree *params, StringBuffer& errMsg)
{
   return CE_OK;
}


const char* Hardware::getComputerName(const char* netAddress)
{
  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTER"[@netAddress=\"%s\"]", netAddress);
  IPropertyTree* pComputer = pHardwareTree->queryPropTree(xpath);
  if (pComputer)
    return pComputer->queryProp(XML_ATTR_NAME);
  return NULL;
}

const char* Hardware::getComputerNetAddress(const char* name)
{
  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTER"[@name=\"%s\"]", name);
  IPropertyTree* pComputer = pHardwareTree->queryPropTree(xpath);
  if (pComputer)
    return pComputer->queryProp(XML_ATTR_NETADDRESS);
  return NULL;
}

IPropertyTree* Hardware::addComputerType(const char* name, const char * type,
              const char* manufacturer, const char* speed, const char* os )
{

  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTERTYPE"[@name=\"%s\"]", name);
  IPropertyTree * pComputerType =  pHardwareTree->queryPropTree(xpath);
  if (pComputerType) return pComputerType;
  
  pComputerType = pHardwareTree->addPropTree(XML_TAG_COMPUTERTYPE, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pComputerType->addProp(xpath, name);
  xpath.clear().append(XML_ATTR_MANUFACTURER);
  pComputerType->addProp(xpath, manufacturer);
  xpath.clear().append(XML_ATTR_COMPUTERTYPE);
  pComputerType->addProp(xpath, type); 
  xpath.clear().append(XML_ATTR_OPSYS);
  pComputerType->addProp(xpath, os);
  xpath.clear().append(XML_ATTR_NICSPEED);
  pComputerType->addProp(xpath, speed);

  return pComputerType;
}

}
