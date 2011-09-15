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

		// Serial�� ���� �Ӽ� 
		internal string Serial_Att {
			get {
				return Serial;
			}
			set {
				Serial = value;
			}
		}

		// DBRun�� ���� �Ӽ� 
		internal bool DBRun_Att {
			get {
				return DBRun;
			}
			set {
				DBRun = value;
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

		// ExeName�� ���� �Ӽ�
		internal string ExeName_Att {
			get {
				return ExeName;
			}
			set {
				ExeName = value;
			}
		}

		// DB ���� ���� ��ü �ʱ�ȭ
		internal void Init_DBAccess(ref SqlConnection Con, ref SqlCommand sqlCom){
			Con_EXE = Con;
			sqlCom_EXE = sqlCom;
		}

		// DB ���� ���� �ʱ�ȭ
		private void Init_Connection() {
			try {
				if(Con_EXE.State == ConnectionState.Open) {
					Con_EXE.Close();
				}
				sqlCom_EXE.Parameters.Clear();
				Con_EXE.Open();
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				return;
			}
		}

		// ����Ʈ������ ���� ���� Ȯ��. true = ���� ��, false = ���� �� ��
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
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
				return false;
			}
		}

		// DB���� EXE�̸� ȹ��
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
					MessageBox.Show("�ش� ����Ʈ���� �̸��� ���� DB���� ã�� �� �����ϴ�\n�۾��� �ߴ� �մϴ�", "�ȳ�", MessageBoxButtons.OK, MessageBoxIcon.Information);
					DBRun = false;
					return;
				}
			} catch(Exception Ex) {
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
				DBRun = false;
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
					Get_ExeName();
				}
			} else {
				Get_ExeName();
			}
		}

		// DB�� Mac Address�� �ؽ��� �Է�
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
				MessageBox.Show(Ex.Message, "����", MessageBoxButtons.OK, MessageBoxIcon.Error);
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