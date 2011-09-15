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
			label.Text = "인증 작업을 진행합니다. 잠시만 기다려 주십시오";
			Info.Text = "라이브러리를 로드 합니다";
	
			Chk_File();

			if(FileRun) {
				Info.Text = "소프트웨어 정보를 수집 합니다";
				Chk_Runable(Chk_Att());
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(DBRun) {
				Info.Text = "바이너리에 기록 합니다";
				FileWrite();
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(FileRun) {
				Info.Text = "소프트웨어 정보를 갱신 합니다";
				DBUpdate();
			} else {
				Info.Text = "작업이 중단 되었습니다";
				return;
			}

			if(DBRun) {
				Info.Text = "작업이 완료 되었습니다";
				label.Text = "인증 작업을 완료 하였습니다";
				MessageBox.Show(this, "모든 작업을 완료 했습니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
			} else {
				Info.Text = "작업이 중단 되었습니다";
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
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				Con.Close();
				return false;
			}
		}

		// DB에서 EXE이름 획득
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
					MessageBox.Show("해당 소프트웨어 이름을 인증 DB에서 찾을 수 없습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
					DBRun = false;
					Con.Close();
					return;
				}
			} catch(SqlException Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				Con.Close();
				return;
			}
		}

		// 인증 여부 판단 및 진행, 매개변수 status, true = 인증된적 없음, false = 인증된적 있음
		internal void Chk_Runable(bool status) {
			if(status) {
				if(DialogResult.No == MessageBox.Show("이미 인증된 적이 있는 소프트웨어 입니다\n인증을 다시 받으시겠습니까?\n다시 인증 받을 경우 기존의 소프트웨어는 더이상 사용할 수 없습니다", "안내", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
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

		// DB에 Mac Address의 해쉬값 입력
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
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				Con.Close();
				DBRun = false;
				return;
			}
		}

		// Windows System 경로 획득
		private void Get_WinPath() {
			try {
				WinPath = System.Environment.GetFolderPath(Environment.SpecialFolder.System);
				progress.Value = 10;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Certifier.dll 파일 존재여부 확인. 없으면 다운로드
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
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Windows System 경로 획득, 파일 존재여부 확인
		internal void Chk_File() {
			Get_WinPath();
			Get_Dll();
		}

		// Mac Address 획득
		private void Get_MacAddr() {
			try{
				if(!SN_GetMacAddr(MASHA1Input)) {
					MessageBox.Show("환경 정보를 얻는데 실패 했습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
				progress.Value = 70;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// Mac Address 의 SHA1 알고리즘 Hash값 획득
		private void Get_MacSHA1Hash() {
			try {
				SN_EncryptSHA1(MASHA1Result, MASHA1Input, 12);
				progress.Value = 80;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
				
		// 레지스트리 검색을 통해 파일 경로 획득
		private void Get_FilePath() {
			try {
				string regPath = "SOFTWARE\\Senol\\PeShield\\Module\\";
				RegistryKey baseKey = Registry.LocalMachine;
				RegistryKey exeKey = baseKey.OpenSubKey(regPath + ExeName);
				FullPath = (string)(exeKey.GetValue(null));
				progress.Value = 60;

				if(0 == FullPath.Length) {
					MessageBox.Show("파일의 경로를 얻을 수 없습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
			} catch(System.Security.SecurityException) {
				MessageBox.Show("읽어들일 수 없는 키 입니다");
				FileRun = false;
				return;
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// 파일 무결성 검사, 파일에 메타 데이터 기록
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
					MessageBox.Show("원본 파일이 조작 되었거나 1차 작업이 완료 되지 않았습니다\n\n암호화를 진행하지 않습니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
					fs.Close();
					FileRun = false;
					return;
				} else if(dataArray[fs.Length - 290] == 1) {
					if(DialogResult.No == MessageBox.Show("이미 작업을 완료한 파일 입니다\n\n암호화를 다시 진행 하시겠습니까?", "안내", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
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
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// 파일 경로 획득, 데이터 기록
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