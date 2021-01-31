using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace UI
{
    namespace Utils
    {
        public class RegistryWrapper
        {
            private readonly string _name = @"SOFTWARE\GhostRaider";
            private readonly string _trPathName = "tr_path";
            private readonly string _savePathName = "save_path";
            private RegistryKey _key;

            public RegistryWrapper()
            {
                _key = Registry.CurrentUser.CreateSubKey(this._name);
            }

            public string getTRPath(string def)
            {
                var val = _key.GetValue(_trPathName);
                return val == null ? def : val.ToString();
            }

            public string getSavePath(string def)
            {
                var val = _key.GetValue(_savePathName);
                return val == null ? def : val.ToString();
            }

            public void setTRPath(string value)
            {
                _key.SetValue(_trPathName, value);
            }

            public void setSavePath(string value)
            {
                _key.SetValue(_savePathName, value);
            }
        }

        public class HandleIni {
            public static void save(string path, string mode, string tr_path)
            {
                if (path == "") return;
                if (tr_path == "") return;
                string[] lines = { mode, path };
                try
                {
                    File.WriteAllLines(tr_path + @"\ghostraider.ini", lines);
                } catch(UnauthorizedAccessException ex)
                {
                    System.Windows.MessageBox.Show("Cannot write config file. Please start the program as administrator.");
                }
                
            }
        }
        public class DLL
        {
            public static bool inject()
            {
                Process injectProcess = new Process();
                injectProcess.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\GhostInjector.exe";
                injectProcess.Start();
                injectProcess.WaitForExit();
                return injectProcess.ExitCode == 0;
            }
        }
    }
}
