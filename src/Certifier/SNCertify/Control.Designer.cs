namespace SNCertify {
	partial class Control {
		/// <summary> 
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		/// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
		protected override void Dispose(bool disposing) {
			if(disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region 구성 요소 디자이너에서 생성한 코드

		/// <summary> 
		/// 디자이너 지원에 필요한 메서드입니다. 
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		private void InitializeComponent() {
			this.Info = new System.Windows.Forms.Label();
			this.label = new System.Windows.Forms.Label();
			this.progress = new System.Windows.Forms.ProgressBar();
			this.line = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.Con = new System.Data.SqlClient.SqlConnection();
			this.sqlCom = new System.Data.SqlClient.SqlCommand();
			this.Execute = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// Info
			// 
			this.Info.Location = new System.Drawing.Point(3, 76);
			this.Info.Name = "Info";
			this.Info.Size = new System.Drawing.Size(335, 24);
			this.Info.TabIndex = 0;
			this.Info.Text = "인증 작업을 시작 합니다...";
			this.Info.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// label
			// 
			this.label.Location = new System.Drawing.Point(3, 49);
			this.label.Name = "label";
			this.label.Size = new System.Drawing.Size(335, 24);
			this.label.TabIndex = 1;
			this.label.Text = "인증 작업을 진행합니다. 진행 버튼을 눌러주십시오";
			this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// progress
			// 
			this.progress.Location = new System.Drawing.Point(9, 15);
			this.progress.Name = "progress";
			this.progress.Size = new System.Drawing.Size(261, 26);
			this.progress.Step = 7;
			this.progress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
			this.progress.TabIndex = 2;
			this.progress.Value = 1;
			// 
			// line
			// 
			this.line.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.line.Location = new System.Drawing.Point(9, 105);
			this.line.Name = "line";
			this.line.Size = new System.Drawing.Size(329, 2);
			this.line.TabIndex = 3;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(7, 112);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(331, 24);
			this.label1.TabIndex = 4;
			this.label1.Text = "Copyright 2006-2007., Project team SeNol";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// Con
			// 
			this.Con.ConnectionString = "workstation id=MSX;packet size=4096;user id=sa;data source=\"203.247.220.211\";pers" +
				"ist security info=True;initial catalog=pubs;password=6561";
			this.Con.FireInfoMessageEventOnUserErrors = false;
			// 
			// sqlCom
			// 
			this.sqlCom.Connection = this.Con;
			// 
			// Execute
			// 
			this.Execute.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.Execute.Location = new System.Drawing.Point(276, 15);
			this.Execute.Name = "Execute";
			this.Execute.Size = new System.Drawing.Size(62, 26);
			this.Execute.TabIndex = 5;
			this.Execute.Text = "진 행";
			this.Execute.UseVisualStyleBackColor = true;
			this.Execute.Click += new System.EventHandler(this.Execute_Click);
			// 
			// Control
			// 
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
			this.Controls.Add(this.Execute);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.line);
			this.Controls.Add(this.progress);
			this.Controls.Add(this.label);
			this.Controls.Add(this.Info);
			this.DoubleBuffered = true;
			this.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.Name = "Control";
			this.Size = new System.Drawing.Size(347, 144);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label Info;
		private System.Windows.Forms.Label label;
		private System.Windows.Forms.Label line;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button Execute;
		public System.Windows.Forms.ProgressBar progress;
	}
}
