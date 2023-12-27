using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BuildNDeploy {
    public static class Extension {

        public static void AppendText(this RichTextBox box, string text, Color color) {
            box.SuspendLayout();

            box.SelectionStart = box.TextLength;
            box.SelectionLength = 0;

            box.SelectionColor = color;
            box.AppendText(text);
            box.SelectionColor = box.ForeColor;
            box.ResumeLayout();
        }

        public static void ExecuteShellCmd( this Process p, string cmd,int timeoutinMs = 1000) { 
            p.StandardInput.WriteLine(cmd);
            Thread.Sleep(timeoutinMs);
            
        }
        public static void ExecuteBashCmd(this Process p, string cmd, int timeoutinMs = 1000) {
            p.StandardInput.Write(cmd+"\n");
            Thread.Sleep(timeoutinMs);
        }
    }
}
