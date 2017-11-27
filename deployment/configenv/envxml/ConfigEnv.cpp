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
#include "ConfigEnv.hpp"


ConfigEnv::ConfigEnv(IPropertyTree *config)
{
   envHelper = new EnvHelper(config);
}

bool ConfigEnv::add(IPropertyTree *params)
{
   return true;
}

bool ConfigEnv::modify(IPropertyTree *params)
{
   return true;
}

bool ConfigEnv::create(IPropertyTree *params) 
{
   StringBuffer xpath, sbTemp;

   fprintf(stdout, "create environment.xml ...\n");

   IPropertyTree * envTree = envHelper->getEnvTree();
 
   //Hardware
   envHelper->getEnvComp("Hardware")->create(params);
   
   //Software
   //EnvSettings
   //Programs

   //output
   StringBuffer env, envXml; 
   toXML(envTree, envXml, 0, XML_SortTags | XML_Format);
   env.clear().appendf("<" XML_HEADER ">\n");
   env.append(envXml);

   Owned<IFile> pFile;
   const char* envFile = params->queryProp("@env");

   pFile.setown(createIFile(envFile));

   Owned<IFileIO> pFileIO;
   pFileIO.setown(pFile->open(IFOcreaterw));
   pFileIO->write(0, env.length(), env.str());

   return true;
}

bool ConfigEnv::remove(IPropertyTree *params)
{
  return true;
}

StringBuffer * ConfigEnv::queryAttribute(StringBuffer *xpath)
{
   return NULL;
}

IPropertyTree * ConfigEnv::query(StringBuffer *xpath)
{
   return NULL;
}
