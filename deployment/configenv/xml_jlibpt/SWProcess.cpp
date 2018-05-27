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
#include "Hardware.hpp"

namespace ech
{

SWProcess::SWProcess(const char* name, EnvHelper * envHelper):SWComponentBase(name, envHelper)
{
  instanceElemName.clear().append("Instance");
}

IPropertyTree * SWProcess::addComponent(IPropertyTree *params)
{
  IPropertyTree * pCompTree = SWComponentBase::addComponent(params);

  Owned<IPropertyTreeIterator> instanceIter =  pCompTree->getElements(instanceElemName);
  synchronized block(mutex);
  ForEach(*instanceIter)
  {
    pCompTree->removeTree(&instanceIter->query());
  }

  return pCompTree;
}

void SWProcess::create(IPropertyTree *params)
{
  SWComponentBase::create(params);

  IPropertyTree * envTree = envHelper->getEnvTree();
  
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_SOFTWARE "/%s[@name=\"my%s\"",processName, name);
  IPropertyTree * pCompTree = envTree->queryPropTree(xpath.str());
  assert(pCompTree);

  //create instance

}


int SWProcess::add(IPropertyTree *params)
{
  int rc = SWComponentBase::add(params);

  IPropertyTree * envTree = envHelper->getEnvTree();
  const char* key = params->queryProp("@key");
  StringBuffer xpath;
  xpath.clear().appendf(XML_TAG_SOFTWARE "/%s[@name=\"%s\"",processName, key);
  IPropertyTree * compTree = envTree->queryPropTree(xpath.str());

  const char* selector = params->queryProp("@selector");
  if (selector && !stricmp(selector, "Instance"))
  {
     addInstances(compTree, params);
  }
  
  StringBuffer compXML;
  toXML(compTree, compXML.clear());
  printf("compTree %s\n",name);
  printf("%s\n",compXML.str());


  return rc;
}

//int SWProcess::addNode(IPropertyTree *params, const char* xpath, bool merge)
//{
//   return 0;
//}

/*
int SWProcess::modify(IPropertyTree *params)
{
   return 0;
}
*/



IConfigComp* SWProcess::getInstanceNetAddresses(StringArray& ipList, const char* clusterName)
{
  IPropertyTree * envTree = envHelper->getEnvTree();
  StringBuffer xpath;
  if (clusterName)
     xpath.clear().appendf(XML_TAG_SOFTWARE "/%s[@name=\"%s\"",processName, clusterName);
  else
     xpath.clear().appendf(XML_TAG_SOFTWARE "/%s[1]",processName);

  IPropertyTree *compTree = envTree->queryPropTree(xpath.str());

  Owned<IPropertyTreeIterator> iter = compTree->getElements(instanceElemName.str());
  ForEach (*iter)
  {
     IPropertyTree *instance = &iter->query();
     ipList.append(instance->queryProp("@netAddress"));

  }
  return (IConfigComp*)this;
}


unsigned SWProcess::getInstanceCount(const char* clusterName)
{
   StringBuffer xpath;
   if (clusterName && *clusterName)
     xpath.clear().appendf("%s[@name=\"%s\"]", processName, clusterName);
   else
     xpath.clear().appendf("%s[1]",processName);
   IPropertyTree * comp = envHelper->getEnvTree()->getPropTree(xpath);
   
   return comp->getCount(instanceElemName);
}

void SWProcess::addInstances(IPropertyTree *parent, IPropertyTree *params)
{
  Owned<IPropertyTree> pAttrs = params->queryPropTree("Attributes");
  if (!pAttrs)
     throw MakeStringException(CfgEnvErrorCode::InvalidParams, "Miss instance attributes input");

  Owned<IPropertyTreeIterator> iter = pAttrs->getElements("Attribute");
  ForEach (*iter)
  {
     IPropertyTree *attr = &iter->query();
     const char* name = attr->queryProp("@name");
     if (!stricmp(name, "ip"))
     {
        IPropertyTree * computerNode = addComputer(attr->queryProp("@value"));
        addInstance(computerNode, parent, pAttrs);
     }
     else if (!stricmp(name, "ipfile"))
     {
        StringArray ips;
        envHelper->processNodeAddress(attr->queryProp("@value"), ips, true);
        for ( unsigned i = 0; i < ips.ordinality() ; i++)
        {
           IPropertyTree * computerNode = addComputer(ips.item(i));
           addInstance(computerNode, parent, pAttrs);
        }
     }
  }
  
}

IPropertyTree * SWProcess::addComputer(const char* ip)
{
   Hardware *hd = (Hardware*) envHelper->getEnvComp("hardware");
   StringBuffer sbTask;
   sbTask.clear().append("<Task operation=\"add\" category=\"hardware\" component=\"" XML_TAG_COMPUTER ">");
   sbTask.appendf("<Attributes><Attribute name=\"ip\" value=\"%s\"/></Attributes></Task>", ip);

   Owned<IPropertyTree> params = createPTreeFromXMLString(sbTask.str());
   return hd->addComputer(params);
}

void SWProcess::addInstance(IPropertyTree *computerNode, IPropertyTree *parent, IPropertyTree *attrs)
{
   // create instance
   IPropertyTree *instanceNode = createPTree(instanceElemName.str()); 
   instanceNode->addProp("@computer", computerNode->queryProp(XML_ATTR_COMPUTERTYPE));   
   instanceNode->addProp(XML_ATTR_NAME, computerNode->queryProp(XML_ATTR_NAME));   
   instanceNode->addProp(XML_ATTR_NETADDRESS, computerNode->queryProp(XML_ATTR_NETADDRESS));   

   StringArray excludeList;
   excludeList.append("ip");
   excludeList.append("ipfile");
   if (attrs)
      updateNode(instanceNode, attrs, &excludeList);

   checkInstanceAttributes(instanceNode, parent);
   
   synchronized block(mutex);
   parent->addPropTree(instanceElemName, instanceNode);
 
}

void SWProcess::checkInstanceAttributes(IPropertyTree *instanceNode, IPropertyTree *parent)
{
   if (portIsRequired() && !instanceNode->hasProp("@port"))
   {
   
      int port = getDefaultPort();
      if (!port)
         throw MakeStringException(CfgEnvErrorCode::InvalidParams, "Miss port attribute in instance"); 
      instanceNode->addPropInt("@port", port);
   }

   if (!instanceNode->hasProp("@directory"))
   {
      const IProperties *props = envHelper->getEnvConfigOptions().getProperties();
      StringBuffer sb;
      sb.clear().appendf("%s/%s/%s",
         props->queryProp("runtime"), processName, parent->queryProp(XML_ATTR_NAME));
      instanceNode->addProp("@directory", sb.str());
   } 

}

void SWProcess::computerAdded(IPropertyTree *computerNode)
{
   StringBuffer sb;
   sb.clear().appendf("%s[1]", processName);
   IPropertyTree * comp = envHelper->getEnvTree()->getPropTree(sb.str());
   addInstance(computerNode, comp, NULL);
}

void SWProcess::computerUpdated(IPropertyTree *computerNode, const char* oldName)
{
   Owned<IPropertyTreeIterator> compIter = envHelper->getEnvTree()->getElements(processName);

   synchronized block(mutex);
   ForEach (*compIter)
   {
      IPropertyTree *comp = &compIter->query();
      Owned<IPropertyTreeIterator> instanceIter = comp->getElements(instanceElemName);
      ForEach (*instanceIter)
      {
         IPropertyTree *instance = &instanceIter->query();
         if (!stricmp(instance->queryProp(XML_ATTR_NAME), oldName))
         {
            if (stricmp(computerNode->queryProp(XML_ATTR_NAME), instance->queryProp(XML_ATTR_NAME) ))
               instance->setProp(XML_ATTR_NAME, computerNode->queryProp(XML_ATTR_NAME));
            if (stricmp(computerNode->queryProp(XML_ATTR_NETADDRESS), instance->queryProp(XML_ATTR_NETADDRESS)))
               instance->setProp(XML_ATTR_NETADDRESS, computerNode->queryProp(XML_ATTR_NETADDRESS));
            if (stricmp(computerNode->queryProp(XML_ATTR_COMPUTERTYPE), instance->queryProp("@computer")))
               instance->setProp("@computer", computerNode->queryProp(XML_ATTR_COMPUTERTYPE));
         }
      }
   }
}

void SWProcess::computerDeleted(const char* name)
{
   IPropertyTree * envTree = envHelper->getEnvTree();
   Owned<IPropertyTreeIterator> compIter = envTree->getElements(processName);

   synchronized block(mutex);
   ForEach (*compIter)
   {
      IPropertyTree * comp = &compIter->query();
      Owned<IPropertyTreeIterator> instanceIter = comp->getElements(instanceElemName);
      ForEach (*instanceIter)
      {
         IPropertyTree * instance = &instanceIter->query();
         if (stricmp(instance->queryProp(XML_ATTR_NETADDRESS), name))
            comp->removeTree(instance);
      } 
   }
}

IPropertyTree * SWProcess::getPortDefinition()
{
   StringBuffer xpath;
   xpath.clear().appendf("xs:element[@name=\"%s\"]",instanceElemName.str());
   IPropertyTree * instanceNode =  pSchema->queryPropTree(xpath.str());

   xpath.clear().append("xs:attribute[@name=\"port\"]");
   return  instanceNode->queryPropTree(xpath.str());
}

bool SWProcess::portIsRequired()
{
   IPropertyTree * portAttrNode =  getPortDefinition();
   if (!portAttrNode) return false;

   const char* portUseAttr = portAttrNode->queryProp("@use");
   if (portUseAttr && stricmp(portUseAttr, "required"))
      return false;

   return true;
}

int SWProcess::getDefaultPort()
{
   IPropertyTree * portAttrNode =  getPortDefinition();
   if (!portAttrNode) return 0;
   int defaultValue = portAttrNode->getPropInt("@default");

   if (!defaultValue) return 0;

   return defaultValue;
}

IPropertyTree * SWProcess::cloneComponent(IPropertyTree *params)
{
   IPropertyTree * targetNode =  SWComponentBase::cloneComponent(params);
   Owned<IPropertyTreeIterator> instanceIter =  targetNode->getElements(instanceElemName);
   ForEach(*instanceIter)
   {
      targetNode->removeTree(&instanceIter->query());
   }

   return targetNode;
}

}
