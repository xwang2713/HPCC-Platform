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
#ifndef _ENVHELPER_HPP_
#define _ENVHELPER_HPP_

#include "jiface.hpp"
#include "jliball.hpp"
#include "XMLTags.h"
//#include "IConfigEnv.hpp"
#include "ConfigEnvError.hpp"
#include "GenEnvRules.hpp"

namespace ech
{

#define ATTR_SEP  "^"
#define ATTR_V_SEP "|" 

//interface IConfigEnv;

//interface IConfigComp;
interface IConfigComp : public IInterface
{
  virtual void create(IPropertyTree *params) = 0;
  virtual int add(IPropertyTree *params) = 0;
  //virtual void addNode(IPropertyTree *node, const char* xpath, bool merge) = 0;
  virtual int modify(IPropertyTree *params) = 0;
  virtual void remove(IPropertyTree *params) = 0;
};

#define DEFAULT_ENV_XML CONFIG_DIR"/environment.xml"
#define DEFAULT_ENV_OPTIONS CONFIG_DIR"/environment.conf"
#define DEFAULT_GEN_ENV_RULES CONFIG_DIR"/genenvrules.conf"
#define DEFAULT_BUILDSET COMPONENTFILES_DIR"/configxml/buildset.xml"
#define ESP_CONFIG_PATH INSTALL_DIR "" CONFIG_DIR "/configmgr/esp.xml"

#define USE_WIZARD 1

enum CONFIG_TYPE { CONFIG_INPUT, CONFIG_ENV, CONFIG_ALL };

class EnvConfigOptions
{
public:
   EnvConfigOptions(const char* filename) { loadFile(filename); }
   void loadFile(const char* filename);
   const IProperties * getProperties() const { return options; }

private:
   IProperties * options;
};


class EnvHelper 
{

public: 
   EnvHelper(IPropertyTree * config);
   ~EnvHelper();
   const EnvConfigOptions& getEnvConfigOptions() const { return *envCfgOptions; }
   const GenEnvRules& getGenEnvRules() const { return *genEnvRules; }
   const IPropertyTree * getBuildSetTree() const { return buildSetTree;}
   IPropertyTree * getEnvTree() { return envTree; }
   IConfigComp* getEnvComp(const char * compName);
   IConfigComp* getEnvSWComp(const char * swCompName);
   const char* getConfig(const char* key, CONFIG_TYPE type=CONFIG_INPUT) const; 
   EnvHelper * setEnvTree(StringBuffer &envXml);
   bool validateAndToInteger(const char *str,int &out, bool throwExcepFlag);
   const char* getXMLTagName(const char* name);


   // PTree helper 
   IPropertyTree * clonePTree(const char* xpath);
   IPropertyTree * clonePTree(IPropertyTree *src);

   // ip address 
   void processNodeAddress(IPropertyTree *params);
   int  processNodeAddress(const char * ipInfo, StringArray &ips, bool isFile=false);
   const StringArray& getNodeList() const { return ipArray; } 
   bool getCompNodeList(const char * compName, StringArray *ipList, const char* cluster=NULL );
   const char * assignNode(const char * compName);
   //void  releaseNodeIp(const char * ip);

   //syncro wrapper for update env tree
   //bool addPTToEnvTree(const char *xpath, IPropertyTree *pTree );
   //bool addPropToEnvTree(const char *pTreeName, const char* attr, const char* value);
   //bool modifyPropToInEnvTree(const char *pTreeName, const char* attr, const char* oldValue, const char* newValue);
   //bool deletePTFromEnvTree(const char *pTreeParentName, const char* pTreeName);
   //bool deletePropFromEnvTree(const char *pTreeName, const char* attr);

   //Node id
   //int categoryToId(const char* category); 
   //const char*  idToCategory(int id); 

   //Schema
   //IPropertyTree * generateCompFromXsd(const char* compName, IPropertyTree* schemaPT);
   

private:
   void init(IPropertyTree * config);

   Owned<IPropertyTree> envTree;
   Owned<IPropertyTree> buildSetTree;
   EnvConfigOptions *envCfgOptions;
   GenEnvRules *genEnvRules;
   IPropertyTree * config;

   StringArray ipArray;
   int *numOfCompSigned;
   int supportIpListPosition = 0;

   MapStringToMyClass<IConfigComp> compMap;
   MapStringTo<int> baseIds;
};

}
#endif
