

interface IConfigComp
{
  bool update(IPropertyTree *setting);
  bool add(IPropertyTree *setting);
  bool delete(IPropertyTree *setting);
}

interface IConfigEnv extends IConfigComp
{
  StringBuffer * queryAttribute(StringBuffer *xpath);
  IPropertyTree * query(StringBuffer *xpath);
  bool createBaseLineConfig(IPropertyTree *config);
}


class ConfigEnvFactory
{
  public:
     static IConfigEnv * getIConfigEnv()
     {
         return new CConfigEnv();
     }
}



/* earlier thoughts but don't like it
interface IConfigEnv
{
   //ICluster getCluser(StringBuffer *type, StringBuffer *name);
  IConfigEnv* getIConfigEnv();

  bool loadXMLConfig(StringBuffer xmlConfig);
  bool createBaseEnvXml();

  bool insert(StringBuffer *xpath, IPropertyTree *ipt);
  bool update(StringBuffer *xpath, StringBuffer *value);
  bool update(StringBuffer *xpath, IPropertyTree *ipt);
  bool delete(StringBuffer *xpath);
  bool delete(StringBuffer *xpath, StringBuffer *value);
  bool delete(StringBuffer *xpath, IPropertyTree *ipt);
  BufferString* queryValue(StringBuffer *xpath);
  IPropertyTree* queryPT(StringBuffer *xpath);
   
}

/*
interface ICluster
{
  
}
*/
*/
