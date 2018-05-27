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

#include "build-config.h"
#include "EnvHelper.hpp"
#include "Hardware.hpp"
#include "Programs.hpp"
#include "EnvSettings.hpp"
#include "Software.hpp"
#include "deployutils.hpp"
#include "build-config.h"
#include "confighelper.hpp"

namespace ech
{

void EnvConfigOptions::loadFile(const char* filename)
{
  options = createProperties(filename);
}

EnvHelper::EnvHelper(IPropertyTree *config)
{
   this->config = config;
   init(config);
}


const char* EnvHelper::getConfig(const char* key, CONFIG_TYPE type) const
{
   StringBuffer sbKey;
   sbKey.clear();
   if (key[0] != '@')
     sbKey.append("@");
   sbKey.append(key);

   if ((type == CONFIG_INPUT) ||(type == CONFIG_ALL))
   {
     if (config->hasProp(sbKey.str()))
       return config->queryProp(sbKey.str());
   }
   if ((type == CONFIG_ENV) || (type == CONFIG_ALL))
   {
     const IProperties * envCfg = envCfgOptions->getProperties();
     if (envCfg->hasProp(sbKey.str()))
       return envCfg->queryProp(sbKey.str());
   }

   return NULL;
}

EnvHelper::~EnvHelper()
{
  if (numOfCompSigned) delete [] numOfCompSigned;
}



void EnvHelper::init(IPropertyTree *config)
{

   /*
   //id   
   baseIds.setValue("Hardware", 1);  
   baseIds.setValue("EnvSettings", 2);  
   baseIds.setValue("Programs", 3);  
   baseIds.setValue("Software", 4);  
   */
  

   StringBuffer fileName;
   const char* optionsFileName = config->queryProp("@options");
   if (optionsFileName && *optionsFileName) 
      fileName.clear().append(optionsFileName);
   else 
      fileName.clear().append(DEFAULT_ENV_OPTIONS);

   envCfgOptions = new EnvConfigOptions(fileName.str()); 

   const char* genEnvRulesFileName = config->queryProp("@rules");
   if (genEnvRulesFileName && *genEnvRulesFileName) 
      fileName.clear().append(genEnvRulesFileName);
   else 
      fileName.clear().append(DEFAULT_GEN_ENV_RULES);
   genEnvRules = new GenEnvRules(fileName.str());


   const char* buildSetFileName = config->queryProp("@buildset");
   if (buildSetFileName && * buildSetFileName) 
      fileName.clear().append(buildSetFileName);
   else 
      fileName.clear().append(DEFAULT_BUILDSET);
   buildSetTree.setown(createPTreeFromXMLFile(fileName.str()));

   const char* envXmlFileName = config->queryProp("@envxml");
   if (envXmlFileName && *envXmlFileName)
      envTree.setown(createPTreeFromXMLFile(envXmlFileName));
   else if (!USE_WIZARD)
   {
      envTree.setown(createPTreeFromXMLString("<" XML_HEADER "><" XML_TAG_ENVIRONMENT "></" XML_TAG_ENVIRONMENT ">"));

      //Initialize CConfigHelper
      const char* espConfigPath =  (config->hasProp("@esp-config"))?
         config->queryProp("@esp-config") : ESP_CONFIG_PATH; 
      Owned<IPropertyTree> espCfg = createPTreeFromXMLFile(espConfigPath);

      const char* espServiceName =  (config->hasProp("@esp-service"))?
         config->queryProp("@esp-service") : "WsDeploy_wsdeploy_esp"; 

      // just initialize the instance which is static member 
      CConfigHelper *pch = CConfigHelper::getInstance(espCfg, espServiceName);
      if (pch == NULL)
      {
        throw MakeStringException( -1 , "Error loading buildset from configuration");
      }
   }

}

/*
 Can't find MapStringTo<int> method to return kes or navigation methods
 Will do it later
int EnvHelper::categoryToId(const char* category)
{
  return baseIds.find(category);
}

const char*  EnvHelper::idToCategory(int id)   
{
  HashIterator iter(baseIds);
  iter.first();
  ForEach(iter.isValid())
  {
    const char* key = iter.query();     
    if (baseIds(key) = id) return key;
    iter.next();
  }

  return "";
   
}
*/

EnvHelper * EnvHelper::setEnvTree(StringBuffer &envXml)
{
  envTree.setown(createPTreeFromXMLString(envXml));
  return this;
}

IConfigComp* EnvHelper::getEnvComp(const char *compName)
{
   const char * compNameLC = (StringBuffer(compName).toLowerCase()).str(); 
   IConfigComp * pComp = compMap.getValue(compNameLC);
   if (pComp) return pComp;


   pComp = NULL;
   if (stricmp(compNameLC, "hardware") == 0)
   {
      pComp = (IConfigComp*) new Hardware(this);
   }
   else if (stricmp(compNameLC, "programs") == 0)
   {
      pComp = (IConfigComp*) new Programs(this);
   }   
   else if (stricmp(compNameLC, "envsettings") == 0)
   {
      pComp = (IConfigComp*) new EnvSettings(this);
   }   
   else if (stricmp(compNameLC, "software") == 0)
   {
      pComp = (IConfigComp*) new Software(this);
   }   

   if (pComp != NULL)
   {
      compMap.setValue(compNameLC,  pComp);
   }

   return pComp;
}

IConfigComp* EnvHelper::getEnvSWComp(const char *swCompName)
{
  Software* sw =  (Software*)getEnvComp("software");
  IConfigComp* icc = sw->getSWComp(swCompName);
  return icc;

 
  //return ((Software*)getEnvComp("software"))->getSWComp(swCompName);
}

int  EnvHelper::processNodeAddress(const char *ipInfo, StringArray &ips, bool isFile)
{
   int len  = ips.ordinality();

   if (!ipInfo || !(*ipInfo)) return 0;

   if (isFile)
   {
     StringBuffer ipAddrs; 
     ipAddrs.loadFile(ipInfo);
     if (ipAddrs.str())
       formIPList(ipAddrs.str(), ips); 
     else
       return 0;
   }
   else
   {
     formIPList(ipInfo, ips); 
   }

   return ips.ordinality() - len;

}

void EnvHelper::processNodeAddress(IPropertyTree * param)
{
   const char* ipList = param->queryProp("@iplist");
   if (ipList)
   {
     formIPList(ipList, ipArray); 
     processNodeAddress(ipList, ipArray);
   }

   const char* ipFileName = param->queryProp("@ipfile");
   if (ipFileName)
   {
     processNodeAddress(ipFileName, ipArray, true);
   }

   if (ipArray.ordinality() > 0)
   {
     numOfCompSigned = new int(ipArray.ordinality());
     for (unsigned i=0; i < ipArray.ordinality(); i++)
       numOfCompSigned[i] = 0;
   }


}

bool EnvHelper::getCompNodeList(const char * compName, StringArray *ipList, const char *cluster)
{
  return true;
}

const char* EnvHelper::assignNode(const char * compName) 
{
  return NULL;
}


bool EnvHelper::validateAndToInteger(const char *str,int &out, bool throwExcepFlag)
{
  bool bIsValid = false;
  char *end = NULL;

  errno = 0;

  const long sl = strtol(str,&end,10);

  if (end == str)
  {
    if (throwExcepFlag)
      throw MakeStringException( CfgEnvErrorCode::NonInteger , "Error: non-integer parameter '%s' specified.\n",str);
  }
  else if ('\0' != *end)
  {
    if (throwExcepFlag)
      throw MakeStringException( CfgEnvErrorCode::NonInteger , "Error: non-integer characters found in '%s' when expecting integer input.\n",str);
  }
  else if ( (INT_MAX < sl || INT_MIN > sl) || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno))
  {
    if (throwExcepFlag)
      throw MakeStringException( CfgEnvErrorCode::OutOfRange , "Error: integer '%s' is out of range.\n",str);
  }
  else
  {
    out = (int)sl;
    bIsValid = true;
  }

  return bIsValid;
}


/*

bool EnvHelper::addPTToEnvTree(const char *xpath, IPropertyTree *pTree)
{
  // do to 
  // 0. check input
  // 1. sync
  // 2. throw Exception if fails
  envTree->addPropTree(xpath, pTree);

  return true;
  
}
*/

/*
IPropertyTree * EnvHelper::generateCompFromXsd(const char* compName, IPropertyTree* schemaPT)
{

  Owned<IPropertyTree> pCompTree(createPTree(compName));

   StringBuffer compXML;
   toXML(pCompTree, compXML.clear());
   printf("comp tree %s\n",compName);
   printf("%s\n",compXML.str());

  return pCompTree.getLink();
}
*/

const char* EnvHelper::getXMLTagName(const char* name)
{
   if (!name)
      throw MakeStringException(CfgEnvErrorCode::InvalidParams, "Null string in getXMLTagName");

   const char * nameLC = (StringBuffer(name).toLowerCase()).str();
    
   if (!strcmp(nameLC, "hardware") || !strcmp(nameLC, "hd"))
      return XML_TAG_HARDWARE;
   else if (!strcmp(nameLC, "software") || strcmp(nameLC, "sw"))
      return XML_TAG_SOFTWARE;
   else if (!strcmp(nameLC, "envsettings") || strcmp(nameLC, "es"))
      return "EnvSettings";
   else if (!strcmp(nameLC, "programs") || strcmp(nameLC, "pg") ||
            !strcmp(nameLC, "build") || strcmp(nameLC, "bd"))
      return "Programs/Build";
   else if (!strcmp(nameLC, "directories") || !strcmp(nameLC, "dirs"))
      return XML_TAG_DIRECTORIES;
   else if (!strcmp(nameLC, "roxie") || !strcmp(nameLC, "roxiecluster"))
      return XML_TAG_ROXIECLUSTER;
   else if (!strcmp(nameLC, "dali") || !strcmp(nameLC, "daliserverprocess") ||
            !strcmp(nameLC, "dalisrv"))
      return XML_TAG_DALISERVERPROCESS;
   else if (!strcmp(nameLC, "dafile") || !strcmp(nameLC, "dafilesrv") ||
            !strcmp(nameLC, "dafileserverprocess"))
      return XML_TAG_DAFILESERVERPROCESS;
   else if (!strcmp(nameLC, "dfu") || !strcmp(nameLC, "dfusrv") ||
            !strcmp(nameLC, "dfuserverprocess"))
      return  "DfuServerProcess";
   else if (!strcmp(nameLC, "dropzone"))
      return XML_TAG_DROPZONE;
   else if (!strcmp(nameLC, "eclcc") || !strcmp(nameLC, "eclccsrv") ||
            !strcmp(nameLC, "eclccserver"))
      return XML_TAG_ECLCCSERVERPROCESS;
   else if (!strcmp(nameLC, "esp") || !strcmp(nameLC, "espprocess"))
      return XML_TAG_ESPPROCESS;
   else if (!strcmp(nameLC, "espsvc") || !strcmp(nameLC, "espservice"))
      return XML_TAG_ESPSERVICE;
   else if (!strcmp(nameLC, "binding") || !strcmp(nameLC, "espbinding"))
      return XML_TAG_ESPBINDING;
   else if (!strcmp(nameLC, "sasha") || !strcmp(nameLC, "sashasrv"))
      return XML_TAG_SASHA_SERVER_PROCESS;
   else if (!strcmp(nameLC, "eclsch") || !strcmp(nameLC, "eclscheduler"))
      return XML_TAG_ECLSCHEDULERPROCESS;
   else if (!strcmp(nameLC, "topology") || !strcmp(nameLC, "topo"))
      return XML_TAG_TOPOLOGY;

   else
      return name;
}


IPropertyTree * EnvHelper::clonePTree(const char* xpath)
{
   StringBuffer error;
   if (!validateXPathSyntax(xpath, &error))
      throw MakeStringException(CfgEnvErrorCode::InvalidParams, 
         "Syntax error in xpath  %s: %s", xpath, error.str());
      
   IPropertyTree *src = envTree->queryPropTree(xpath); 
   return clonePTree(src);
}

IPropertyTree * EnvHelper::clonePTree(IPropertyTree *src)
{
   StringBuffer xml;
   toXML(src, xml);
   return createPTreeFromXMLString(xml.str());
}

}
