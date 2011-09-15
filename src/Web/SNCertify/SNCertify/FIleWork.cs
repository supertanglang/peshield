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

		// Serial에 대한 속성
		internal string Serial_Att {
			get {
				return Serial;
			}
			set {
				Serial = value;
			}
		}

		// ExeName에 대한 속성
		internal string ExeName_Att {
			get {
				return ExeName;
			}
			set {
				ExeName = value;
			}
		}

		// FileRun에 대한 속성
		internal bool FileRun_Att {
			get {
				return FileRun;
			}
			set {
				FileRun = value;
			}
		}

		// MacAddr에 대한 속성 
		internal string MacAddr_Att {
			get {
				return MacAddr;
			}
			set {
				MacAddr = value;
			}
		}

		// Windows System 디렉토리 경로 획득
		private void Get_WinPath() {
			try {
				WinPath = System.Environment.GetFolderPath(Environment.SpecialFolder.System);
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// Certifier.dll 파일 존재여부 확인. 없으면 다운로드
		private void Get_Dll() {
			try {
				if(!File.Exists(@"C\certifier.dll")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.221.245/SEL/SmartClient/certifier.dll", @"C:\certifier.dll");
					File.SetAttributes(@"C:\certifier.dll", FileAttributes.Hidden);
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// PeShield.sys 파일 존재여부 확인. 없으면 다운로드
		private void Get_Sys() {
			try {
				if(!File.Exists(WinPath + @"\drivers" + @"\PeShield.sys")) {
					WebClient DownloadFile = new WebClient();
					DownloadFile.DownloadFile(@"http://203.247.221.245/SEL/SmartClient/PeShield.sys", WinPath + @"\drivers" + @"\PeShield.sys");
					File.SetAttributes(WinPath + @"\drivers" + @"\PeShield.sys", FileAttributes.Hidden);
				}
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
			Get_Sys();
		}

		// Mac Address 획득
		private void Get_MacAddr() {
			try{
				if(!GetMacAddress(MASHA1Input)) {
					MessageBox.Show("환경 정보를 얻는데 실패 했습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;			
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
		
		// Mac Address 의 SHA1 알고리즘 Hash값 획득
		private void Get_MacSHA1Hash() {
			try {
				SHA1_Crypt(MASHA1Result, MASHA1Input, 12);
				MacAddr = Encoding.Default.GetString(MASHA1Result);
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}
				
		// 레지스트리 검색을 통해 파일 경로 획득
		private void Get_FilePath() {
			try {
				string regPath = "SOFTWARE\\Senol\\";
				RegistryKey baseKey = Registry.LocalMachine;
				RegistryKey exeKey = baseKey.OpenSubKey(regPath + ExeName);
				FullPath = exeKey.GetValue(null).ToString();

				if(0 == FullPath.Length) {
					MessageBox.Show("파일의 경로를 얻을 수 없습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Error);
					FileRun = false;
					return;
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// 파일 무결성 검사, 파일에 메타 데이터 기록
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
					MessageBox.Show("원본 파일이 조작 되었거나 1차 작업이 완료 되지 않았습니다\n\n암호화를 진행하지 않습니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
					fs.Close();
					FileRun = false;
					return;
				} else if(dataArray[fs.Length - 282] == 1) {
					if(DialogResult.No == MessageBox.Show("이미 작업을 완료한 파일 입니다\n\n암호화를 다시 진행 하시겠습니까?", "안내", MessageBoxButtons.YesNo, MessageBoxIcon.Information)) {
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
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				FileRun = false;
				return;
			}
		}

		// 파일 경로 획득, 데이터 기록
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