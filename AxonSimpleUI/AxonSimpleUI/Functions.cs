﻿using System;
using System.Threading;
using System.Windows.Forms;

namespace AxonSimpleUI
{
    class Functions
    {
        public static string exploitdllname = "Axon.dll";//Axon.dll this is the name of your dll
        public static void Inject()
        {
            if (NamedPipes.NamedPipeExist(NamedPipes.luapipename))//check if the pipe exist
            {
                MessageBox.Show("Already injected!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);//if the pipe exist that's mean that we don't need to inject
                return;
            }
            else if (!NamedPipes.NamedPipeExist(NamedPipes.luapipename))//check if the pipe don't exist
            {
                switch (Injector.DllInjector.GetInstance.Inject("RobloxPlayerBeta", AppDomain.CurrentDomain.BaseDirectory + exploitdllname))//Process name and dll directory
                {
                    case Injector.DllInjectionResult.DllNotFound://if can't find the dll
                        MessageBox.Show("Couldn't find " + exploitdllname, "Dll was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);//display messagebox to tell that dll was not found
                        return;
                    case Injector.DllInjectionResult.GameProcessNotFound://if can't find the process
                        MessageBox.Show("Couldn't find RobloxPlayerBeta.exe!", "Target process was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);//display messagebox to tell that proccess was not found
                        return;
                    case Injector.DllInjectionResult.InjectionFailed://if injection fails(this don't work or only on special cases)
                        MessageBox.Show("Injection Failed!", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);//display messagebox to tell that injection failed
                        return;
                }
                //Thread.Sleep(3000);//pause the ui for 3 seconds <<DONT PAUSE SHIT
                if (!NamedPipes.NamedPipeExist(NamedPipes.luapipename))//check if the pipe dont exist
                {
                    //display that the pipe was not found so the injection was unsuccessful <<YES IT IS SUCCESSFUL BUT THE MESSAGE BOX THING TAKES HERE :C
                }
            }
        }

        public static OpenFileDialog openfiledialog = new OpenFileDialog
        {
            Filter = "Lua Script Txt (*.txt)|*.txt|All files (*.*)|*.*",//add txt and all files filter
            FilterIndex = 1,//choose what filter will be the default
            RestoreDirectory = true,//restore the last used directory
            Title = "Lua Open Script"//OpenFileDialog Tittle <<BAD GRAMMAR XD
        };//Initialize OpenFileDialog
    }
}
