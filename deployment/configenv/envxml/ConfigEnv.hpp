include "IConfigEnv.hpp"

class ConfigEnv implements IConfigEnv
{

   //typedef map<StringBuffer, IConfigEnv*> ddd;
   IConfigComp* getIConfigComp(IPropertyTree *setting);
}
