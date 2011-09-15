//----------------------------------------------------------------------------//
// FileWork.cs                                                                //
// PeShield Certifier Smart Client - File Operations                          //
//----------------------------------------------------------------------------//

using System;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace SNCertify {
	class FIleWork {
		[DllImport(@"C:\certifier.dll")]
		private static extern void SHA1_Crypt(byte[] tgt, byte[] src, int len);

		[DllImport(@"C:\certifier.dll")]
		private static extern bool GetMacAddress(byte[] mac);
		
		private string MacAddr;
		private string ExeName;
		private string FullPath;
		private string WinPath;
		private string Serial;

		private byte[] MASHA1Input = new byte[12];
		private byte[] MASHA1Result = new byte[20];

		private bool FileRun = true;

		// Serial�� ���� �Ӽ�
		internal string Serial_Att {
			get {
				return Serial;
			}
			set {
				Serial = value;
			}
		}

		// ExeName�� ���� �Ӽ�
		internal string ExeName_Att {
			get {
				return ExeName;
			}
			set {
				ExeName = value;
			}
		}

		// FileRun�� ���� �Ӽ�
		internal bool FileRun_Att {
			get {
				return FileRun;
			}
			set {
				FileRun = value;
			}
		}

		// MacAddr�� ���� �Ӽ� 
		internal string MacAddr_Att {
			get {
				return MacAddr;
			}
			set {
				MacAddr = value;
			}
		}

		// Windows System ���丮 ��� ȹ��
		private void Get_WinPath() {
			try {
				WinPath = System.Environment.GetFolderPath(Environment.SpecialFolder.System);
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Certifier.dll ���� ���翩�� Ȯ��. ������ �ٿ�ε�
		private void Get_Dll() {
			try {
				if(!File.Exists(@"C\certifier.dll")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.221.245/SEL/SmartClient/certifier.dll", @"C:\certifier.dll");
					File.SetAttributes(@"C:\certifier.dll", FileAttributes.Hidden);
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// PeShield.sys ���� ���翩�� Ȯ��. ������ �ٿ�ε�
		private void Get_Sys() {
			try {
				if(!File.Exists(WinPath + @"\drivers" + @"\PeShield.sys")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.221.245/SEL/SmartClient/PeShield.sys", WinPath + @"\drivers" + @"\PeShield.sys");
					File.SetAttributes(WinPath + @"\drivers" + @"\PeShield.sys", FileAttributes.Hidden);
				}
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
			Get_Sys();
		}

		// Mac Address ȹ��
		private void Get_MacAddr() {
			try{
				if(!GetMacAddress(MASHA1Input)) {
					MessageBox.Show("ȯ�� ������ ��µ� ���� �߽��ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;			
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// Mac Address �� SHA1 �˰��� Hash�� ȹ��
		private void Get_MacSHA1Hash() {
			try {
				SHA1_Crypt(MASHA1Result, MASHA1Input, 12);
				MacAddr = Encoding.Default.GetString(MASHA1Result);
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
				
		// ������Ʈ�� �˻��� ���� ���� ��� ȹ��
		private void Get_FilePath() {
			try {
				string regPath = "SOFTWARE\\Senol\\";
				RegistryKey baseKey = Registry.LocalMachine;
				RegistryKey exeKey = baseKey.OpenSubKey(regPath + ExeName);
				FullPath = exeKey.GetValue(null).ToString();

				if(0 == FullPath.Length) {
					MessageBox.Show("������ ��θ� ���� �� �����ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// ���� ���Ἲ �˻�, ���Ͽ� ��Ÿ ������ ���
		private void MDWrite() {
			Get_MacAddr();
			Get_MacSHA1Hash();

			try{
				const byte x1 = 83;
				const byte x2 = 78;
				byte[] chk_w = new byte[1];
				chk_w[0] = 1;

				FileStream fs = new FileStream(@FullPath, FileMode.Open);

				byte[] dataArray = new byte[fs.Length];

				fs.Read(dataArray, 0, (int)fs.Length);

				if(dataArray[fs.Length - 284] != x1 || dataArray[fs.Length - 283] != x2) {
					MessageBox.Show("���� ������ ���� �Ǿ��ų� 1�� �۾��� �Ϸ� ���� �ʾҽ��ϴ�\n\n��ȣȭ�� �������� �ʽ��ϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
					fs.Close();
					FileRun = false;
					return;
				} else if(dataArray[fs.Length - 282] == 1) {
					if(DialogResult.No == MessageBox.Show("�̹� �۾��� �Ϸ��� ���� �Դϴ�\n\n��ȣȭ�� �ٽ� ���� �Ͻðڽ��ϱ�?", "�ȳ�", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
						fs.Close();
						FileRun = false;
						return;
					}
				}

				fs.Position = fs.Length - 6;
				fs.Write(Encoding.Default.GetBytes(Serial), 0, 5);
				fs.Position = fs.Length - 282;
				fs.Write(chk_w, 0, 1);

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
			MDWrite();
		}
	}
}

//----------------------------------------------------------------------------//
//                                                  End Of File : FileWork.cs //
//                                                  (C) 2006-2007 Team SeNol, //
//     Sunmoon University BIT Training Center : Windows Developer Course 12th //
//----------------------------------------------------------------------------//
//       All's right with the world, We all live underneath the same blue sky //
//                          K&R Notation / Written by NIGUS (nigus@korea.com) //
//----------------------------------------------------------------------------//