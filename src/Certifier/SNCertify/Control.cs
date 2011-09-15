//----------------------------------------------------------------------------//
// Control.cs                                                                 //
// PeShield Certifier Smart Client - Main Operation                           //
//----------------------------------------------------------------------------//

using System;
using System.Windows.Forms;
using System.Net;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Text;

namespace SNCertify {
	public partial class Control: UserControl {
		[DllImport(@"C:\crypto.dll")]
		private static extern void SN_EncryptSHA1(byte[] pBufTgt, byte[] pcBufSrc, int cbSrc);

		[DllImport(@"C:\comm.dll")]
		private static extern bool SN_GetMacAddr(byte[] pMacAddr);

		private System.Data.SqlClient.SqlConnection Con;
		private System.Data.SqlClient.SqlCommand sqlCom;

		private string Serial = null;
		private string ExeName;
		private bool DBRun = true;
		private bool FileRun = true;

		private string FullPath;
		private string WinPath;

		private byte[] MASHA1Input = new byte[12];
		private byte[] MASHA1Result = new byte[20];

		public Control() {
			InitializeComponent();
		}

		public string g_param1 {
			get {
				return Serial;
			}
			set {
				Serial = value;
			}
		}

		private void Execute_Click(object sender, EventArgs e) {
			Execute.Enabled = false;
			label.Text = "���� �۾��� �����մϴ�. ��ø� ��ٷ� �ֽʽÿ�";
			Info.Text = "���̺귯���� �ε� �մϴ�";
	
			Chk_File();

			if(FileRun) {
				Info.Text = "����Ʈ���� ������ ���� �մϴ�";
				Chk_Runable(Chk_Att());
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(DBRun) {
				Info.Text = "���̳ʸ��� ��� �մϴ�";
				FileWrite();
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(FileRun) {
				Info.Text = "����Ʈ���� ������ ���� �մϴ�";
				DBUpdate();
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}

			if(DBRun) {
				Info.Text = "�۾��� �Ϸ� �Ǿ����ϴ�";
				label.Text = "���� �۾��� �Ϸ� �Ͽ����ϴ�";
				MessageBox.Show(this, "��� �۾��� �Ϸ� �߽��ϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
			} else {
				Info.Text = "�۾��� �ߴ� �Ǿ����ϴ�";
				return;
			}
		}

		internal bool Chk_Att() {
			try {
				if(Con.State == ConnectionState.Open) {
					Con.Close();
				}
				Con.Open();
				sqlCom.CommandText = "SELECT MAC1 FROM suc WHERE SERIAL = @st1";
				sqlCom.Parameters.Add("@st1", SqlDbType.VarChar, 20).Value = Serial;
		
				SqlDataReader R1 = sqlCom.ExecuteReader();
				R1.Read();

				progress.Value = 30;

				if(R1.HasRows) {
					if(0 == R1["MAC1"].ToString().Length || R1["MAC1"] == null) {
						return false;
					} else {
						return true;
					}
				}

				return false;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				Con.Close();
				return false;
			}
		}

		// DB���� EXE�̸� ȹ��
		private void Get_ExeName() {
			try {
				if(Con.State == ConnectionState.Open) {
					Con.Close();
				}
				sqlCom.Parameters.Clear();
				Con.Open();
				sqlCom.CommandText = "SELECT SNAME FROM products WHERE SERIAL = @st2";
				sqlCom.Parameters.Add("@st2", SqlDbType.VarChar, 20).Value = Serial;

				SqlDataReader R1 = sqlCom.ExecuteReader();
				R1.Read();

				if(R1.HasRows) {
					ExeName = R1["SNAME"].ToString();
					progress.Value = 50;
					Con.Close();
				} else {
					MessageBox.Show("�ش� ����Ʈ���� �̸��� ���� DB���� ã�� �� �����ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
					DBRun = false;
					Con.Close();
					return;
				}
			} catch(SqlException Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				Con.Close();
				return;
			}
		}

		// ���� ���� �Ǵ� �� ����, �Ű����� status, true = �������� ����, false = �������� ����
		internal void Chk_Runable(bool status) {
			if(status) {
				if(DialogResult.No == MessageBox.Show("�̹� ������ ���� �ִ� ����Ʈ���� �Դϴ�\n������ �ٽ� �����ðڽ��ϱ�?\n�ٽ� ���� ���� ��� ������ ����Ʈ����� ���̻� ����� �� �����ϴ�", "�ȳ�", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
					DBRun = false;
					return;
				} else {
					progress.Value = 40;
					Get_ExeName();
				}
			} else {
				progress.Value = 40;
				Get_ExeName();
			}
		}

		// DB�� Mac Address�� �ؽ��� �Է�
		internal void DBUpdate() {
			try {
				if(Con.State == ConnectionState.Open) {
					Con.Close();
				}
				sqlCom.Parameters.Clear();
				Con.Open();
				sqlCom.CommandText = "UPDATE suc SET MAC1 = @st21 WHERE SERIAL = @st22";
				sqlCom.Parameters.Add("@st21", SqlDbType.VarBinary, 20).Value = MASHA1Result;
				sqlCom.Parameters.Add("@st22", SqlDbType.VarChar, 20).Value = Serial;
				sqlCom.ExecuteNonQuery();
				progress.Value = 100;
				Con.Close();
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				Con.Close();
				DBRun = false;
				return;
			}
		}

		// Windows System ��� ȹ��
		private void Get_WinPath() {
			try {
				WinPath = System.Environment.GetFolderPath(Environment.SpecialFolder.System);
				progress.Value = 10;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Certifier.dll ���� ���翩�� Ȯ��. ������ �ٿ�ε�
		private void Get_Dll() {
			try {
				if(!File.Exists(@"C:\crypto.dll")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.220.211/SENOL/crypto.dll", @"C:\crypto.dll");
					File.SetAttributes(@"C:\crypto.dll", FileAttributes.Hidden);
				}
				if(!File.Exists(@"C:\comm.dll")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.220.211/SENOL/comm.dll", @"C:\comm.dll");
					File.SetAttributes(@"C:\comm.dll", FileAttributes.Hidden);
				}
				progress.Value = 20;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Windows System ��� ȹ��, ���� ���翩�� Ȯ��
		internal void Chk_File() {
			Get_WinPath();
			Get_Dll();
		}

		// Mac Address ȹ��
		private void Get_MacAddr() {
			try{
				if(!SN_GetMacAddr(MASHA1Input)) {
					MessageBox.Show("ȯ�� ������ ��µ� ���� �߽��ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
				progress.Value = 70;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// Mac Address �� SHA1 �˰��� Hash�� ȹ��
		private void Get_MacSHA1Hash() {
			try {
				SN_EncryptSHA1(MASHA1Result, MASHA1Input, 12);
				progress.Value = 80;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
				
		// ������Ʈ�� �˻��� ���� ���� ��� ȹ��
		private void Get_FilePath() {
			try {
				string regPath = "SOFTWARE\\Senol\\PeShield\\Module\\";
				RegistryKey baseKey = Registry.LocalMachine;
				RegistryKey exeKey = baseKey.OpenSubKey(regPath + ExeName);
				FullPath = (string)(exeKey.GetValue(null));
				progress.Value = 60;

				if(0 == FullPath.Length) {
					MessageBox.Show("������ ��θ� ���� �� �����ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
			} catch(System.Security.SecurityException) {
				MessageBox.Show("�о���� �� ���� Ű �Դϴ�");
				FileRun = false;
				return;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// ���� ���Ἲ �˻�, ���Ͽ� ��Ÿ ������ ���
		private void MDWrite() {
			try{
				const byte x1 = 83;
				const byte x2 = 78;
				byte[] chk_w = new byte[1];
				chk_w[0] = 1;

				FileStream fs = new FileStream(FullPath, FileMode.Open);

				byte[] dataArray = new byte[fs.Length];

				fs.Read(dataArray, 0, (int)fs.Length);

				if(dataArray[fs.Length - 292] != x1 || dataArray[fs.Length - 291] != x2) {
					MessageBox.Show("���� ������ ���� �Ǿ��ų� 1�� �۾��� �Ϸ� ���� �ʾҽ��ϴ�\n\n��ȣȭ�� �������� �ʽ��ϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
					fs.Close();
					FileRun = false;
					return;
				} else if(dataArray[fs.Length - 290] == 1) {
					if(DialogResult.No == MessageBox.Show("�̹� �۾��� �Ϸ��� ���� �Դϴ�\n\n��ȣȭ�� �ٽ� ���� �Ͻðڽ��ϱ�?", "�ȳ�", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
						fs.Close();
						FileRun = false;
						return;
					}
				}
				fs.Position = fs.Length - 262;
				fs.Write(Encoding.Default.GetBytes(Serial), 0, 5);
				fs.Position = fs.Length - 290;
				fs.Write(chk_w, 0, 1);

				progress.Value = 90;

				fs.Close();
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// ���� ��� ȹ��, ������ ���
		internal void FileWrite() {
			Get_FilePath();
			Get_MacAddr();
			Get_MacSHA1Hash();
			MDWrite();
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