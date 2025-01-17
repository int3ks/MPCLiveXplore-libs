

using Microsoft.Extensions.Configuration;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;



namespace BuildNDeploy {
    public partial class Form1 : Form {

        [DllImport("user32.dll")]
        private static extern bool SetForegroundWindow(IntPtr WindowHandle);

        private const int SW_NORMAL = 1;
        private const int SW_SHOWMINIMIZED = 2;

        [DllImport("user32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        private readonly IConfiguration config;
        Process Powershell = null;
        static bool gccChecked = false;
        static Process logprocess;

        bool mpconline = false;
        private void timer1_Tick(object sender, EventArgs e) {
            timer1.Enabled = false;
            var ping = new Process {
                StartInfo = new ProcessStartInfo {
                    FileName = "ping.exe",
                    Arguments = $"{config["mpc_ip"]} -n 1 -w 30",
                    //  RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                },
                EnableRaisingEvents = true
            };
            ping.Start();
            if (ping.WaitForExit(50)) {
                var result = ping.StandardOutput.ReadToEnd();
                if (result.Contains("Zeit=")) {
                    mpconline = true;
                    Text = "online";
                }
            }
            else {
                mpconline = false;
                Text = "offline";
                ping.Kill();
            }

            timer1.Enabled = true;
        }

        public Form1() {

            InitializeComponent();

            config = new ConfigurationBuilder()
            .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
            .Build();

            timer1.Enabled = true;
            comboBox1.SelectedIndex = (int) Properties.Settings.Default["MyPad"];
        }
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e) {
            Properties.Settings.Default["MyPad"] = comboBox1.SelectedIndex;
            Properties.Settings.Default.Save();
        }

        private void Form1_Load(object sender, EventArgs e) {
            log(@"*** CHECK PATHES IN ..\BuildNDeploy\bin\Debug\net6.0-windows\appsettings.json ***" + "\r\n");
            log($"Iamforce2_src_path_wsl = {config["Iamforce2_src_path_wsl"]}");
            log($"Iamforce2_src_path_windows  = {config["Iamforce2_src_path_windows"]}");
            log($"Iamforce2_bin_path_windows = {config["Iamforce2_bin_path_windows"]}");
            log($"Iamforce2_remote_dir = {config["Iamforce2_remote_dir"]}");
            log($"MyMpc = {config["MyMpc"]}");
            log($"MyPad = {comboBox1.SelectedItem}");
            log("*******");
        }

        void log(string info) {
            Invoke(DoGuiAccess, info);
        }

        [DllImport("user32.dll")]
        public static extern bool LockWindowUpdate(IntPtr hWndLock);

        bool DoGuiAccess(string info) {
            LockWindowUpdate(logtext.Handle);
            logtext.Text = $"{logtext.Text}\r\n{info}";
            logtext.SelectionStart = logtext.Text.Length;
            logtext.SelectionLength = 0;
            logtext.ScrollToCaret();
            LockWindowUpdate(IntPtr.Zero);
            return true;
        }

        private void StartPowerShell() {
            Powershell = new Process {
                StartInfo = new ProcessStartInfo {
                    FileName = "powershell.exe",
                    UseShellExecute = false,
                    RedirectStandardInput = true,
                    CreateNoWindow = false,
                },
                EnableRaisingEvents = true
            };
            Powershell.Start();
            Powershell.Exited += (sender, e) => { Powershell = null; };
            Powershell.ExecuteShellCmd("Function prompt { \"PS > \" } ; cls", 100);
        }

        private void EnsurePowershell() {
            if (Powershell == null) {
                StartPowerShell();
            }
            ShowWindow(Powershell.MainWindowHandle, SW_NORMAL);
            SetForegroundWindow(Powershell.MainWindowHandle);
        }


        private void StartBash() {
            Powershell.ExecuteBashCmd("Bash");
            if (!gccChecked) {
                Powershell.ExecuteBashCmd("sudo apt-get install gcc-arm-linux-gnueabihf -y");
                Powershell.ExecuteBashCmd("sudo apt-get install libasound2-dev -y");
                gccChecked = true;
            }
        }

        private void runbuild_Click(object sender, EventArgs e) {
            var src = config["Iamforce2_src_path_wsl"];
            EnsurePowershell();
            StartBash();
            src = src.Replace("\r\n", "\n");

            Powershell.ExecuteBashCmd($"cd {src}");

            Powershell.ExecuteBashCmd("./wsl2_mk");
            Powershell.ExecuteBashCmd($"echo build done '{DateTime.Now}' ");

            Powershell.ExecuteBashCmd("exit");
            return;
        }




        private void runTestBuild_Click(object sender, EventArgs e) {

            var src = config["Iamforce2_src_path_wsl"];

            EnsurePowershell();
            StartBash();
            src = src.Replace("\r\n", "\n");

            Powershell.ExecuteBashCmd($"cd {src}");

            if (comboBox1.SelectedIndex == 0) {
                Powershell.ExecuteBashCmd("./wsl2_mk");
            }
            else {
                //Powershell.ExecuteBashCmd($"./wsl2_mk1 {config["MyPad"]} {config["MyMpc"]} ");
                Powershell.ExecuteBashCmd($"./wsl2_mk1 {comboBox1.SelectedItem} {config["MyMpc"]} ");
            }



            Powershell.ExecuteBashCmd($"echo build done '{DateTime.Now}' ");
            Powershell.ExecuteBashCmd("exit");
            SetForegroundWindow(this.Handle);
            return;
        }


        int restartcounter = 0;
        private void copyTestBinToMpc_click(object sender, EventArgs e) {
            EnsurePowershell();
            Powershell.ExecuteShellCmd($"ssh root@{config["mpc_ip"]} systemctl stop inmusic-mpc");

            IEnumerable<string> bins;


            //if (runAll.Checked) {
            if (comboBox1.SelectedIndex == 0) {
                bins = Directory.EnumerateFiles(config["Iamforce2_bin_path_windows"], "*Iamforce*.so");

            }
            else {
                bins = new[] { $"{config["Iamforce2_bin_path_windows"]}\\tmm-IamForce-{comboBox1.SelectedItem}-{config["MyMpc"]}.so" };
            }

            CopyBins(bins);
            if (restartcounter > 5 || reboot.Checked) {
                Powershell.ExecuteShellCmd($"ssh root@{config["mpc_ip"]} reboot");
                restartcounter = 0;
                reboot.Checked = false;
            }
            else {
                Powershell.ExecuteShellCmd($"ssh root@{config["mpc_ip"]} systemctl start inmusic-mpc");
            }
            restartcounter++;
            SetForegroundWindow(this.Handle);
            return;
        }
        //private void copyAllBinsToMpc_Click(object sender, EventArgs e) {
        //    EnsurePowershell();

        //    Powershell.ExecuteShellCmd("ssh root@192.168.50.210 systemctl stop inmusic-mpc", 100);
        //    var bins = Directory.EnumerateFiles(config["Iamforce2_bin_path_windows"], "*.so");
        //    CopyBins(bins);
        //    Powershell.ExecuteShellCmd("ssh root@192.168.50.210 systemctl start inmusic-mpc", 100);
        //}






        private void CopyBins(IEnumerable<string> bins) {
            foreach (var bin in bins) {
                log(bin.ToString());

                var filename = Path.GetFileName(bin);
                var remotepath = $"{config["Iamforce2_remote_dir"]}/{filename}";

                var file = new FileInfo(bin);
                Powershell.ExecuteShellCmd($" scp \"{file}\" root@{config["mpc_ip"]}:{remotepath}", 100);
            }
        }



       


        public void Bashlog() {
            try {
                logprocess = new Process {
                    StartInfo = new ProcessStartInfo {
                        FileName = "powershell.exe",
                        Arguments = $"ssh root@{config["mpc_ip"]} \"journalctl -u inmusic-mpc -f\"",
                        //  RedirectStandardInput = true,
                        RedirectStandardOutput = true,
                        UseShellExecute = false,
                        CreateNoWindow = true
                    },
                    EnableRaisingEvents = true
                };
                logprocess.Exited += (sender, e) => Invoke(() => { /*checkBox1.Checked = false;*/
                    logprocess?.CloseMainWindow();
                    logprocess?.Kill();
                    logprocess = null;
                });
                logprocess.OutputDataReceived += (sender, e) => {
                    if (e.Data == null) { return; }
                    if (chkTKGL.Checked) {
                        if (chkDEBUG.Checked) {
                            if (e.Data.Contains("DEBUG")) {
                                log((e.Data + "").Replace("mpc-live-ii az01-launch-MPC", ""));

                            }
                            return;
                        }
                        if (e.Data.Contains("[tkgl")) {
                            log((e.Data + "").Replace("mpc-live-ii az01-launch-MPC", ""));

                        }
                        return;
                    }
                    log((e.Data + "").Replace("mpc-live-ii az01-launch-MPC", ""));
                };
                logprocess.Start();
                logprocess.BeginOutputReadLine();
            } catch (Exception e) {
                log($"Command 'journalctl -u inmusic-mpc -f' failed {e}");
            }
        }

        private void logtimer_Tick(object sender, EventArgs e) {
            if (logprocess == null && checkBox1.Checked && mpconline) {
                Bashlog();
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e) {

            logtimer.Enabled = checkBox1.Checked;




            if (!checkBox1.Checked) {

                logprocess.CloseMainWindow();
                logprocess?.Kill();
                logprocess = null;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e) {
            Powershell?.Kill();
            logprocess?.Kill();
        }

        
    }
}