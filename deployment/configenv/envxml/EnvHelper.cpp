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

#include "EnvHelper.hpp"
#include "Hardware.hpp"
#include "build-config.h"

#define DEFAULT_ENV_XML CONFIG_DIR"/environment.xml"
#define DEFAULT_ENV_OPTIONS CONFIG_DIR"/environment.conf"
#define DEFAULT_GEN_ENV_RULES CONFIG_DIR"/genenvrules.conf"
#define DEFAULT_BUILDSET COMPONENTFILES_DIR"/configxml/buildset.xml"

// IPropertyTree* p
// p->addPropTree("EspConfig", createPTree());
// p->addProp(xpath, "value");
// p->queryPropTree(xpath);

/*
    Owned<IPropertyTreeIterator> it = pNode->getElements(XSD_TAG_ELEMENT);
    if (it->first() && it->isValid())
    {  
       IPropertyTree* pTemp = &it->query();
       const char* szName = pTemp->queryProp(XML_ATTR_NAME);
    }

    pBuild->queryPropTree("BuildSet[@name='topology']")
    pEnv->getElements("Programs/Build[@name]");
*/

/* wizardInputs.cpp p133 load environment.conf, configmgr

*/


void EnvConfigOptions::loadFile(const char* filename)
{
  options = createProperties(filename);
}

void GenEnvRules::loadFile(const char* filename)
{
  rules = createProperties(filename);
}


EnvHelper::EnvHelper(IPropertyTree *config)
{
   init(config);
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
   else
      envTree = createPTreeFromXMLString("<" XML_HEADER "><" XML_TAG_ENVIRONMENT "></" XML_TAG_ENVIRONMENT ">");

}

IConfigComp* EnvHelper::getEnvComp(const char *compName)
{
   if (stricmp(compName, "Hardware") == 0)
   {
      if (hardware == NULL) hardware = (IConfigComp*) new Hardware(this);
      return hardware;
   }
   
   fprintf(stderr, "Unknown environment component %s\n", compName);
   return NULL;
}

