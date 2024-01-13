using System.Security.Policy;

namespace BuildNDeploy {
    partial class Form1 {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            components = new System.ComponentModel.Container();
            checkBox1 = new CheckBox();
            button2 = new Button();
            button3 = new Button();
            logtext = new TextBox();
            reboot = new CheckBox();
            timer1 = new System.Windows.Forms.Timer(components);
            logtimer = new System.Windows.Forms.Timer(components);
            chkTKGL = new CheckBox();
            chkDEBUG = new CheckBox();
            comboBox1 = new ComboBox();
            SuspendLayout();
            // 
            // checkBox1
            // 
            checkBox1.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            checkBox1.AutoSize = true;
            checkBox1.Location = new Point(10, 500);
            checkBox1.Margin = new Padding(3, 2, 3, 2);
            checkBox1.Name = "checkBox1";
            checkBox1.Size = new Size(240, 19);
            checkBox1.TabIndex = 3;
            checkBox1.Text = "Show Log (journalctl -u inmusic-mpc -f)";
            checkBox1.UseVisualStyleBackColor = true;
            checkBox1.CheckedChanged += checkBox1_CheckedChanged;
            // 
            // button2
            // 
            button2.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            button2.Location = new Point(1033, 500);
            button2.Margin = new Padding(3, 2, 3, 2);
            button2.Name = "button2";
            button2.Size = new Size(143, 25);
            button2.TabIndex = 4;
            button2.Text = "deploy to mpc";
            button2.UseVisualStyleBackColor = true;
            button2.Click += copyTestBinToMpc_click;
            // 
            // button3
            // 
            button3.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            button3.Location = new Point(1182, 500);
            button3.Margin = new Padding(3, 2, 3, 2);
            button3.Name = "button3";
            button3.Size = new Size(143, 25);
            button3.TabIndex = 5;
            button3.Text = "run build on wsl2";
            button3.UseVisualStyleBackColor = true;
            button3.Click += runTestBuild_Click;
            // 
            // logtext
            // 
            logtext.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            logtext.BackColor = Color.MidnightBlue;
            logtext.BorderStyle = BorderStyle.FixedSingle;
            logtext.Font = new Font("Lucida Console", 11.25F, FontStyle.Regular, GraphicsUnit.Point);
            logtext.ForeColor = SystemColors.Window;
            logtext.Location = new Point(0, 0);
            logtext.Margin = new Padding(3, 2, 3, 2);
            logtext.Multiline = true;
            logtext.Name = "logtext";
            logtext.ScrollBars = ScrollBars.Vertical;
            logtext.Size = new Size(1336, 494);
            logtext.TabIndex = 12;
            logtext.WordWrap = false;
            // 
            // reboot
            // 
            reboot.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            reboot.AutoSize = true;
            reboot.Location = new Point(848, 504);
            reboot.Margin = new Padding(3, 2, 3, 2);
            reboot.Name = "reboot";
            reboot.Size = new Size(61, 19);
            reboot.TabIndex = 14;
            reboot.Text = "reboot";
            reboot.UseVisualStyleBackColor = true;
            // 
            // timer1
            // 
            timer1.Interval = 1000;
            timer1.Tick += timer1_Tick;
            // 
            // logtimer
            // 
            logtimer.Interval = 10;
            logtimer.Tick += logtimer_Tick;
            // 
            // chkTKGL
            // 
            chkTKGL.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            chkTKGL.AutoSize = true;
            chkTKGL.Location = new Point(256, 500);
            chkTKGL.Margin = new Padding(3, 2, 3, 2);
            chkTKGL.Name = "chkTKGL";
            chkTKGL.Size = new Size(78, 19);
            chkTKGL.TabIndex = 15;
            chkTKGL.Text = "only TKGL";
            chkTKGL.UseVisualStyleBackColor = true;
            // 
            // chkDEBUG
            // 
            chkDEBUG.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            chkDEBUG.AutoSize = true;
            chkDEBUG.Location = new Point(340, 500);
            chkDEBUG.Margin = new Padding(3, 2, 3, 2);
            chkDEBUG.Name = "chkDEBUG";
            chkDEBUG.Size = new Size(89, 19);
            chkDEBUG.TabIndex = 16;
            chkDEBUG.Text = "only DEBUG";
            chkDEBUG.UseVisualStyleBackColor = true;
            // 
            // comboBox1
            // 
            comboBox1.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBox1.FormattingEnabled = true;
            comboBox1.Items.AddRange(new object[] { "ALL", "LUMI", "LPMK3", "LPMK2", "APCKEY25MK2", "APCMINIMK2", "KIKPADMK3", "LPPROMK3", "LPX", "NONE" });
            comboBox1.Location = new Point(927, 502);
            comboBox1.Name = "comboBox1";
            comboBox1.Size = new Size(98, 23);
            comboBox1.TabIndex = 17;
            comboBox1.SelectedIndexChanged += comboBox1_SelectedIndexChanged;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1337, 529);
            Controls.Add(comboBox1);
            Controls.Add(chkDEBUG);
            Controls.Add(chkTKGL);
            Controls.Add(reboot);
            Controls.Add(logtext);
            Controls.Add(button3);
            Controls.Add(button2);
            Controls.Add(checkBox1);
            Margin = new Padding(3, 2, 3, 2);
            Name = "Form1";
            Text = "Form1";
            FormClosing += Form1_FormClosing;
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private CheckBox checkBox1;
        private Button button2;
        private Button button3;
        private TextBox logtext;
        private CheckBox reboot;
       
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Timer logtimer;
        private CheckBox chkTKGL;
        private CheckBox chkDEBUG;
        private ComboBox comboBox1;
    }
}