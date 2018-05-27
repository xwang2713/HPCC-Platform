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
#include "SWProcess.hpp"
#include "deployutils.hpp"

namespace ech
{

const char* Hardware::TYPE    = "linuxmachine";
const char* Hardware::MAKER   = "unknown";
const char* Hardware::SPEED   = "1000";
const char* Hardware::DOMAIN  = "localdomain";
const char* Hardware::OS      = "linux";

Hardware::Hardware(EnvHelper * envHelper):ComponentBase("hardware", envHelper)
{
  //notifyUpdateList.appendListUniq("dafilesrv,dali,dfuserver,DropZone,eclagent", ",");
  //notifyUpdateList.appendListUniq("eclccserver,eclscheduler,esp,sasha,ws_ecl", ",");
  //notifyUpdateList.appendListUniq("espsmc,ftslave", ",");
  //notifyUpdateList.appendListUniq("thor");
  notifyUpdateList.append("roxie");

  notifyAddList.append("dafilesrv");

}

void Hardware::create(IPropertyTree *params)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
  assert (envTree);

  if (envTree->queryPropTree("./" XML_TAG_HARDWARE))
  {
     throw MakeStringException(CfgEnvErrorCode::ComponentExists,
       "Cannot create Hardware component which  already exists");
  }

  StringBuffer xpath, sbdefaultValue("");
  Owned<IPropertyTree> pCompTree = createPTree(XML_TAG_HARDWARE);

  IPropertyTree* pSwitch = pCompTree->addPropTree(XML_TAG_SWITCH, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pSwitch->addProp(xpath, "Switch") ;

  IPropertyTree* pDomain = pCompTree->addPropTree(XML_TAG_DOMAIN, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pDomain->addProp(xpath, Hardware::DOMAIN);

  envTree->addPropTree(XML_TAG_HARDWARE, createPTreeFromIPT(pCompTree)); 

  StringBuffer task;
  task.clear().append("<Task operation=\"add\" category=\"hardware\"");
  task.appendf("selector= \"" XML_TAG_COMPUTERTYPE "\"><Attributes/></Task>"); 
  Owned<IPropertyTree> taskPT = createPTreeFromXMLString(task.str()); 
  addComputerType(taskPT);

  const StringArray& ipArray = envHelper->getNodeList(); 
  for (unsigned i = 0; i < ipArray.ordinality(); i++)
  {
     task.clear().append("<Task operation=\"add\" category=\"hardware\" component=\"" XML_TAG_COMPUTER ">");
     task.appendf("<Attributes><Attribute name=\"ip\" value=\"%s\"/></Attributes></Task>", ipArray.item(i));
     taskPT.setown(createPTreeFromXMLString(task.str())); 
     addComputer(taskPT);
  }
}

IPropertyTree* Hardware::addComputer(IPropertyTree *params) 
{
  assert(params);
  Owned<IPropertyTree> attrs  = params->queryPropTree("Task[@component=\"" XML_TAG_COMPUTER "\"]/Attributes");
  assert(attrs);

  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  assert(pHardwareTree);

  const char * ip           = getAttributeFromParams(attrs, "ip", NULL);
  if (!ip)
    throw MakeStringException(CfgEnvErrorCode::InvalidParams, "Miss ip information in adding hardware"); 

  const char * computerName = getAttributeFromParams(attrs, "name", NULL);
  const char * type         = getAttributeFromParams(attrs, "type", Hardware::TYPE);
  const char * domain       = getAttributeFromParams(attrs, "domain", Hardware::DOMAIN);
  const char * namePrefix   = getAttributeFromParams(attrs, "namePrefix", NULL);

  StringBuffer  sbIp;
  IpAddress ipAddr; 

  sbIp.clear();
  if ((ip == NULL) || (ip == "")) 
  {
    if ((computerName == NULL) || (computerName == ""))
      throw MakeStringException(CfgEnvErrorCode::InvalidParams, "Cannot add a computer without both ip and name.");
    else
    {
      ipAddr.ipset(computerName);
      ipAddr.getIpText(sbIp);
    }
  }
  else
    sbIp.append(ip);

  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTER"[@netAddress=\"%s\"]", sbIp.str()); 
  IPropertyTree* pComputer = pHardwareTree->queryPropTree(xpath);
  if (pComputer)
    return pComputer;


  xpath.clear().appendf(XML_TAG_DOMAIN "[@name=\"%s\"]", domain);
  IPropertyTree* pDomain = pHardwareTree->queryPropTree(xpath);
  if (!pDomain)
  {
     xpath.clear().append("<Task operation=\"add\" category=\"hardware\" selector=\"" XML_TAG_DOMAIN "\">");
     xpath.appendf("<Attribute><Attribute name=\"name\" value=\"%s\"/></Attributes></Task>", domain);
     Owned<IPropertyTree> taskPT = createPTreeFromXMLString(xpath.str()); 
     addDomain(taskPT);
  }


  StringBuffer sbName;
  sbName.clear();
  if ((computerName == NULL) || (computerName == ""))
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
    sbName.append(computerName);
  }

  //synchronized block(mutex);
  mutex.lock();

  pComputer = pHardwareTree->addPropTree(XML_TAG_COMPUTER,createPTree());
  pComputer->addProp(XML_ATTR_COMPUTERTYPE, type);
  pComputer->addProp(XML_ATTR_DOMAIN, domain);
  pComputer->addProp(XML_ATTR_NAME, sbName.str());
  pComputer->addProp(XML_ATTR_NETADDRESS, sbIp.str());
  mutex.unlock();

  //notify a new computer added
  for (int i = 0; i < notifyAddList.ordinality(); i++)
  {
     ((SWProcess*)envHelper->getEnvSWComp(notifyAddList.item(i)))->computerAdded(pComputer);
  }

  return pComputer;
}

int Hardware::add(IPropertyTree *params)
{
   return 0;
}

/*
int Hardware::addNode(IPropertyTree *node, const char* xpath, bool merge)
{
   return 0;
}
*/

/*
int Hardware::modify(IPropertyTree *params)
{
   return 0;
}

void Hardware::remove(IPropertyTree *params)
{
   return;
}

*/

const char* Hardware::getComputerName(const char* netAddress)
{
  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTER "[@netAddress=\"%s\"]", netAddress);
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
  xpath.clear().appendf(XML_TAG_COMPUTER "[@name=\"%s\"]", name);
  IPropertyTree* pComputer = pHardwareTree->queryPropTree(xpath);
  if (pComputer)
    return pComputer->queryProp(XML_ATTR_NETADDRESS);
  return NULL;
}

IPropertyTree* Hardware::addComputerType(IPropertyTree *params)
{

  assert(params);
  Owned<IPropertyTree> attrs  = params->queryPropTree("Task[@selector=\"Domain\"]/Attributes");
  assert(attrs);

  const char * typeName         = getAttributeFromParams(attrs, "name", Hardware::TYPE);
  
  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_COMPUTERTYPE "[@name=\"%s\"]", typeName);
  IPropertyTree * pComputerType =  pHardwareTree->queryPropTree(xpath);
  if (pComputerType) return pComputerType;


  const char * manufacturer = getAttributeFromParams(attrs, "manufacturer", Hardware::MAKER);
  const char * type         = getAttributeFromParams(attrs, "computerType", Hardware::TYPE);
  const char * os           = getAttributeFromParams(attrs, "computerType", Hardware::OS);
  const char * speed        = getAttributeFromParams(attrs, "nicSpeed", Hardware::SPEED);
  
  synchronized block(mutex);
  pComputerType = pHardwareTree->addPropTree(XML_TAG_COMPUTERTYPE, createPTree());
  xpath.clear().append(XML_ATTR_NAME);
  pComputerType->addProp(xpath, typeName);
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

IPropertyTree* Hardware::addDomain(IPropertyTree *params)
{
  assert(params);
  Owned<IPropertyTree> attrs  = params->queryPropTree("Task[@selector=\"Domain\"]/Attributes");
  assert(attrs);

  IPropertyTree* envTree = envHelper->getEnvTree();
  IPropertyTree* pHardwareTree = envTree->queryPropTree(XML_TAG_HARDWARE);
  assert(pHardwareTree);

  const char * domain  = getAttributeFromParams(attrs, "domain", Hardware::DOMAIN);
  
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_DOMAIN "[@name=\"%s\"]", domain);
  IPropertyTree* pDomain = pHardwareTree->queryPropTree(xpath);
  if (pDomain) return pDomain;

  synchronized block(mutex);

  pDomain = pHardwareTree->addPropTree(XML_TAG_DOMAIN, createPTree());
  pDomain->addProp(XML_ATTR_NAME, domain);

  return pDomain;
}



}
