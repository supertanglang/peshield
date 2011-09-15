//----------------------------------------------------------------------------//
// Control.cs                                                                 //
// PeShield Certifier Smart Client - Main Operation                           //
//----------------------------------------------------------------------------//

using System;
using System.Windows.Forms;

namespace SNCertify {
	public partial class Control: UserControl {
		private System.Data.SqlClient.SqlConnection Con;
		private System.Data.SqlClient.SqlCommand sqlCom;
		
		private DBConnection DB_Exe = new DBConnection();
		private FIleWork File_Exe = new FIleWork();
		
		public Control() {
			InitializeComponent();
			DB_Exe.Init_DBAccess(ref Con, ref sqlCom);
		}

		public string g_param1 {
			get {
				return DB_Exe.Serial_Att;
			}
			set {
				File_Exe.Serial_Att = value;
				DB_Exe.Serial_Att = value;
			}
		}

		private void Execute_Click(object sender, EventArgs e) {
			Execute.Enabled = false;
			
			File_Exe.Chk_File();
			label.Text = "인증 작업을 진행합니다. 잠시만 기다려 주십시오";
			Info.Text = "라이브러리를 로드 합니다";
			progress.Value = 24;

			if(File_Exe.FileRun_Att) {
				Info.Text = "소프트웨어 정보를 수집 합니다";
				DB_Exe.Chk_Runable(DB_Exe.Chk_Att());
				progress.Value = 48;
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(DB_Exe.DBRun_Att) {
				Info.Text = "바이너리에 기록 합니다";
				File_Exe.ExeName_Att = DB_Exe.ExeName_Att;
				File_Exe.FileWrite();
				progress.Value = 72;
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(File_Exe.FileRun_Att) {
				Info.Text = "소프트웨어 정보를 갱신 합니다";
				DB_Exe.MacAddr_Att = File_Exe.MacAddr_Att;
				DB_Exe.DBUpdate();
				progress.Value = 96;
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(DB_Exe.DBRun_Att) {
				Info.Text = "작업이 완료 되었습니다";
				MessageBox.Show(this, "모든 작업을 완료 했습니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
				progress.Value = 100;
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------//
//                                                   End Of File : Control.cs //
//                                                  (C) 2006-2007 Team SeNol, //
//     Sunmoon University BIT Training Center : Windows Developer Course 12th //
//----------------------------------------------------------------------------//
//       All's right with the world, We all live underneath the same blue sky //
//                          K&R Notation / Written by NIGUS (nigus@korea.com) //
//----------------------------------------------------------------------------//