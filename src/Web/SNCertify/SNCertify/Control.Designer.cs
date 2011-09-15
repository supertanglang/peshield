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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Control));
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
			this.Info.AccessibleDescription = null;
			this.Info.AccessibleName = null;
			resources.ApplyResources(this.Info, "Info");
			this.Info.Font = null;
			this.Info.Name = "Info";
			// 
			// label
			// 
			this.label.AccessibleDescription = null;
			this.label.AccessibleName = null;
			resources.ApplyResources(this.label, "label");
			this.label.Font = null;
			this.label.Name = "label";
			// 
			// progress
			// 
			this.progress.AccessibleDescription = null;
			this.progress.AccessibleName = null;
			resources.ApplyResources(this.progress, "progress");
			this.progress.BackgroundImage = null;
			this.progress.Font = null;
			this.progress.Name = "progress";
			this.progress.Step = 7;
			this.progress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
			this.progress.Value = 1;
			// 
			// line
			// 
			this.line.AccessibleDescription = null;
			this.line.AccessibleName = null;
			resources.ApplyResources(this.line, "line");
			this.line.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.line.Font = null;
			this.line.Name = "line";
			// 
			// label1
			// 
			this.label1.AccessibleDescription = null;
			this.label1.AccessibleName = null;
			resources.ApplyResources(this.label1, "label1");
			this.label1.Font = null;
			this.label1.Name = "label1";
			// 
			// Con
			// 
			this.Con.ConnectionString = "workstation id=MSX;packet size=4096;user id=sa;data source=\"203.247.221.245\";pers" +
				"ist security info=True;initial catalog=pubs;password=6561";
			this.Con.FireInfoMessageEventOnUserErrors = false;
			// 
			// sqlCom
			// 
			this.sqlCom.Connection = this.Con;
			// 
			// Execute
			// 
			this.Execute.AccessibleDescription = null;
			this.Execute.AccessibleName = null;
			resources.ApplyResources(this.Execute, "Execute");
			this.Execute.BackgroundImage = null;
			this.Execute.Font = null;
			this.Execute.Name = "Execute";
			this.Execute.UseVisualStyleBackColor = true;
			this.Execute.Click += new System.EventHandler(this.Execute_Click);
			// 
			// Control
			// 
			this.AccessibleDescription = null;
			this.AccessibleName = null;
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
			resources.ApplyResources(this, "$this");
			this.BackgroundImage = null;
			this.Controls.Add(this.Execute);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.line);
			this.Controls.Add(this.progress);
			this.Controls.Add(this.label);
			this.Controls.Add(this.Info);
			this.DoubleBuffered = true;
			this.Font = null;
			this.Name = "Control";
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
