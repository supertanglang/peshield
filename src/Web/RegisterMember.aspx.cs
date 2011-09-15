using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Web.Security;
using System.Data.SqlClient;
using System.Configuration;


namespace SENOL
{
	/// <summary>
	/// RegisterMember�� ���� ��� �����Դϴ�.
	/// </summary>
	public class RegisterMember : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.TextBox txtID;
		protected System.Web.UI.WebControls.RequiredFieldValidator RequirtxtID;
		protected System.Web.UI.WebControls.TextBox txtPwd;
		protected System.Web.UI.WebControls.RequiredFieldValidator RequirtxtPWD;
		protected System.Web.UI.WebControls.TextBox txtPwdConfirm;
		protected System.Web.UI.WebControls.RequiredFieldValidator RequirePWDC;
		protected System.Web.UI.WebControls.CompareValidator ComparePwd;
		protected System.Web.UI.WebControls.TextBox txtName;
		protected System.Web.UI.WebControls.RequiredFieldValidator RequireName;
		protected System.Web.UI.WebControls.ValidationSummary ValidationSummary1;
		protected System.Web.UI.WebControls.ImageButton imagebtnMember;
		protected System.Web.UI.WebControls.Image imageError;
	
		protected string ConnectStr;	//DB����

		private void Page_Load(object sender, System.EventArgs e)
		{
			// ���⿡ ����� �ڵ带 ��ġ�Ͽ� �������� �ʱ�ȭ�մϴ�.
			ConnectStr = ConfigurationSettings.AppSettings["ConnectionString"];	
		}

		#region Web Form �����̳ʿ��� ������ �ڵ�
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: �� ȣ���� ASP.NET Web Form �����̳ʿ� �ʿ��մϴ�.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// �����̳� ������ �ʿ��� �޼����Դϴ�.
		/// �� �޼����� ������ �ڵ� ������� �������� ���ʽÿ�.
		/// </summary>
		private void InitializeComponent()
		{    
			this.imagebtnMember.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnMember_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void imagebtnMember_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			if(IsMemberExists())
			{
				imageError.Visible = true;
			}
			else
			{
				AddMember();	//ȸ���������
			}
		}

		//�Էµ� ���̵��� ���翩��
		private bool IsMemberExists()
		{
			bool bExist = false;

			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText = "Select ID from member Where ID = @ID";
			
			Cmd.Parameters.Add("@ID", SqlDbType.VarChar, 20);
			Cmd.Parameters["@ID"].Value = txtID.Text;

			try
			{
				Con.Open();
				if(Cmd.ExecuteScalar() != null)	//�ش�Ǵ� ID�� ���ٸ�
				{
					bExist = true;
				}
			}
			catch (Exception e){}
			finally
			{
				Con.Close();
			}
			return bExist;	
		}		

		//ȸ������ ���
		private void AddMember()
		{
			string id	= txtID.Text;
			string pwd	= txtPwd.Text;
			string name = txtName.Text;

			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText = "INSERT INTO Member(ID, PWD, NAME) values (@ID, @PWD, @NAME)";

			Cmd.Parameters.Add("@ID", SqlDbType.VarChar, 20);
			Cmd.Parameters.Add("@PWD", SqlDbType.VarChar, 20);
			Cmd.Parameters.Add("@NAME", SqlDbType.VarChar, 10);

			Cmd.Parameters["@ID"].Value = id;
			Cmd.Parameters["@PWD"].Value = pwd;
			Cmd.Parameters["@NAME"].Value = name;
			
			try
			{
				Con.Open();
				Cmd.ExecuteNonQuery();	//ȸ������DB������
				Application["CurrentID"] = id;	//ID����������
				FormsAuthentication.SetAuthCookie(id, false); 

				//ȸ�����Կ� ������ �޼��� �ڽ� ���
				string script="<script>";
				script += "alert('ȸ���� ���ԵǼ̽��ϴ�.');";
				script += "location.href='Main.aspx';";
				script += "</script>";
				this.RegisterClientScriptBlock("Welcome", script);
			}		
			catch (System.Exception e)
			{
				
			}
			finally
			{
				Con.Close();
			}
		}
	}
}
