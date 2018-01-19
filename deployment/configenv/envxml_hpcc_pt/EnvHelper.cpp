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
   buildSet = createPTreeFromXMLFile(fileName.str());

   const char* envXmlFileName = config->queryProp("@envxml");
   if (envXmlFileName && *envXmlFileName)
      envTree = createPTreeFromXMLFile(envXmlFileName);
   else if (!USE_WIZARD)
   {
      envTree = createPTreeFromXMLString("<" XML_HEADER "><" XML_TAG_ENVIRONMENT "></" XML_TAG_ENVIRONMENT ">");

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

EnvHelper * EnvHelper::setEnvTree(StringBuffer &envXml)
{
  envTree = createPTreeFromXMLString(envXml);
  return this;
}

IConfigComp* EnvHelper::getEnvComp(const char *compName)
{
   const char * compNameLC = (StringBuffer(compName).toLowerCase()).str(); 
   IConfigComp * pComp = (IConfigComp*) compMap.getValue(compNameLC);
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
  return ((Software*)getEnvComp("software"))->getSWComp(swCompName);
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
      throw MakeStringException( -1 , "Error: non-integer parameter '%s' specified.\n",str);
  }
  else if ('\0' != *end)
  {
    if (throwExcepFlag)
      throw MakeStringException( -1 , "Error: non-integer characters found in '%s' when expecting integer input.\n",str);
  }
  else if ( (INT_MAX < sl || INT_MIN > sl) || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno))
  {
    if (throwExcepFlag)
      throw MakeStringException( -1 , "Error: integer '%s' is out of range.\n",str);
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

}

