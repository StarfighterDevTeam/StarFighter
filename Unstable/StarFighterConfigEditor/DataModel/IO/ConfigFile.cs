using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;

namespace DataModel.IO
{
    public class ConfigFile
    {
        public ConfigFile(string path)
        {
            if (!File.Exists(path))
            {
                throw new ArgumentException("Can't find file '" + path + "', check app.config.");
            }

            List<string> fileLines = File.ReadAllLines(path).ToList(); ;
            this.Path = path;
            this.Columns = fileLines.Count == 0 ? new List<string>() : ReadHeader(fileLines[0]);
            CheckFileIsCsv(fileLines, true);

        }

        public string Path { get; private set; }

        public List<List<string>> Content { get; set; }

        public List<string> Columns { get; set; }

        public void CheckFileIsCsv(List<string> fileLines, bool updateContent = false)
        {
            if(updateContent)
            {
                Content = new List<List<string>>();
            }

            int i = 0;
            foreach (string line in fileLines)
            {
                i++;
                if (line.StartsWith(Constants.CommentChar.ToString()))
                {
                    continue;
                }

                string[] lineSplitted = line.Split(',');
                if (lineSplitted.Count() != Columns.Count)
                {
                    throw new ArgumentException("File: '" + this.Path + "' parsing error at line " + i + ": invalid # of lines");
                }

                if(updateContent)
                {
                    Content.Add(lineSplitted.ToList());
                }
            }

        }

        private static List<string> ReadHeader(string header)
        {
            if (string.IsNullOrEmpty(header))
            {
                throw new ArgumentNullException("header");
            }
            if (!header.StartsWith(Constants.CommentChar.ToString()))
            {
                throw new ArgumentNullException("FIel header should start with char '" + Constants.CommentChar + "'");
            }

            return header.TrimStart(Constants.CommentChar).Split(',').ToList();
        }
    }
}
