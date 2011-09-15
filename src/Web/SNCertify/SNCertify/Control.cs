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
			label.Text = "���� �۾��� �����մϴ�. ��ø� ��ٷ� �ֽʽÿ�";
			Info.Text = "���̺귯���� �ε� �մϴ�";
			progress.Value = 24;

			if(File_Exe.FileRun_Att) {
				Info.Text = "����Ʈ���� ������ ���� �մϴ�";
				DB_Exe.Chk_Runable(DB_Exe.Chk_Att());
				progress.Value = 48;
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(DB_Exe.DBRun_Att) {
				Info.Text = "���̳ʸ��� ��� �մϴ�";
				File_Exe.ExeName_Att = DB_Exe.ExeName_Att;
				File_Exe.FileWrite();
				progress.Value = 72;
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(File_Exe.FileRun_Att) {
				Info.Text = "����Ʈ���� ������ ���� �մϴ�";
				DB_Exe.MacAddr_Att = File_Exe.MacAddr_Att;
				DB_Exe.DBUpdate();
				progress.Value = 96;
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(DB_Exe.DBRun_Att) {
				Info.Text = "�۾��� �Ϸ� �Ǿ����ϴ�";
				MessageBox.Show(this, "��� �۾��� �Ϸ� �߽��ϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
				progress.Value = 100;
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
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