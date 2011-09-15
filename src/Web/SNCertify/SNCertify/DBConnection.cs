//----------------------------------------------------------------------------//
// DBConnection.cs                                                            //
// PeShield Certifier Smart Client - DB Connection & Operations               //
//----------------------------------------------------------------------------//

using System;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;

namespace SNCertify {
	class DBConnection {
		private string Serial;
		private string MacAddr;
		private string ExeName;
		private bool DBRun = true;

		private SqlConnection Con_EXE;
		private SqlCommand sqlCom_EXE;

		// Serial에 대한 속성 
		internal string Serial_Att {
			get {
				return Serial;
			}
			set {
				Serial = value;
			}
		}

		// DBRun에 대한 속성 
		internal bool DBRun_Att {
			get {
				return DBRun;
			}
			set {
				DBRun = value;
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

		// ExeName에 대한 속성
		internal string ExeName_Att {
			get {
				return ExeName;
			}
			set {
				ExeName = value;
			}
		}

		// DB 연결 관련 객체 초기화
		internal void Init_DBAccess(ref SqlConnection Con, ref SqlCommand sqlCom){
			Con_EXE = Con;
			sqlCom_EXE = sqlCom;
		}

		// DB 연결 상태 초기화
		private void Init_Connection() {
			try {
				if(Con_EXE.State == ConnectionState.Open) {
					Con_EXE.Close();
				}
				sqlCom_EXE.Parameters.Clear();
				Con_EXE.Open();
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				return;
			}
		}

		// 소프트웨어의 인증 여부 확인. true = 인증 됨, false = 인증 안 됨
		internal bool Chk_Att() {
			Init_Connection();
			try {
				sqlCom_EXE.CommandText = "SELECT SERIAL FROM suc WHERE SERIAL = @st1";
				sqlCom_EXE.Parameters.Add("@st1", SqlDbType.VarChar, 20);
				sqlCom_EXE.Parameters["@st1"].Value = Serial;

				SqlDataReader R1 = sqlCom_EXE.ExecuteReader();
				R1.Read();

				if(R1.HasRows) {
					R1.Close();
					return true;
				} else {
					R1.Close();
					return false;
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				return false;
			}
		}

		// DB에서 EXE이름 획득
		private void Get_ExeName() {
			Init_Connection();
			try {
				sqlCom_EXE.CommandText = "SELECT SName FROM products WHERE Serial = @st1";
				sqlCom_EXE.Parameters.Add("@st2", SqlDbType.VarChar, 20);
				sqlCom_EXE.Parameters["@st2"].Value = Serial;

				SqlDataReader R1 = sqlCom_EXE.ExecuteReader();
				R1.Read();

				if(R1.HasRows) {
					ExeName = R1["SName"].ToString();
				} else {
					MessageBox.Show("해당 소프트웨어 이름을 인증 DB에서 찾을 수 없습니다\n작업을 중단 합니다", "안내", MessageBoxButtons.OK, MessageBoxIcon.Information);
					DBRun = false;
					return;
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
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
					Get_ExeName();
				}
			} else {
				Get_ExeName();
			}
		}

		// DB에 Mac Address의 해쉬값 입력
		internal void DBUpdate() {
			Init_Connection();
			try{
				sqlCom_EXE.CommandText = "UPDATE suc SET MAC = @st21 WHERE SERIAL = @st22";
				sqlCom_EXE.Parameters.Add("@st21", SqlDbType.VarChar, 20);
				sqlCom_EXE.Parameters.Add("@st22", SqlDbType.VarChar, 20);
				sqlCom_EXE.Parameters["@st21"].Value = MacAddr;
				sqlCom_EXE.Parameters["@st22"].Value = Serial;

				SqlDataReader R1 = sqlCom_EXE.ExecuteReader();

				R1.Close();
				Con_EXE.Close();
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "오류", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------//
//                                              End Of File : DBConnection.cs //
//                                                  (C) 2006-2007 Team SeNol, //
//     Sunmoon University BIT Training Center : Windows Developer Course 12th //
//----------------------------------------------------------------------------//
//       All's right with the world, We all live underneath the same blue sky //
//                          K&R Notation / Written by NIGUS (nigus@korea.com) //
//----------------------------------------------------------------------------//