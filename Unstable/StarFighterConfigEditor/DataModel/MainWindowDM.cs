using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DataModel.IO;

namespace DataModel
{
    public class MainWindowDM
    {
        //TODO: pass configuration
        public MainWindowDM()
        {
            this.ConfigFiles = new List<ConfigFile>();

            //Populate config Files list according to conf
        }

        public List<ConfigFile> ConfigFiles { get; set; }
    }
}
