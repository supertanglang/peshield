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
	/// _Default�� ���� ��� �����Դϴ�.
	/// </summary>
	public class _Default : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.Image imageID;
		protected System.Web.UI.WebControls.TextBox txtID;
		protected System.Web.UI.WebControls.ImageButton imagebtnLogin;
		protected System.Web.UI.WebControls.RequiredFieldValidator RetxtID;
		protected System.Web.UI.WebControls.Image imagePWD;
		protected System.Web.UI.WebControls.TextBox txtPWD;
		protected System.Web.UI.WebControls.ImageButton imagebtnRegister;
		protected System.Web.UI.WebControls.RequiredFieldValidator RetxtPWD;
		protected System.Web.UI.WebControls.Label lblUser;
		protected System.Web.UI.WebControls.ImageButton imagebtnLogount;
		protected System.Web.UI.WebControls.ImageButton imagebtnmain;
		protected System.Web.UI.WebControls.ValidationSummary ValidationSummary1;
	
		private string ConnectStr;	//DB����

		private void Page_Load(object sender, System.EventArgs e)
		{
			// ���⿡ ����� �ڵ带 ��ġ�Ͽ� �������� �ʱ�ȭ�մϴ�.
			ConnectStr = ConfigurationSettings.AppSettings["ConnectionString"];
			if(User.Identity.IsAuthenticated)
			{
				imageID.Visible = false;
				imagePWD.Visible = false;
				imagebtnLogin.Visible =false;
				imagebtnRegister.Visible=false;
				txtID.Visible = false;
				txtPWD.Visible = false;
				imagebtnLogount.Visible =true;
				imagebtnmain.Visible = true;
			}
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
			this.imagebtnLogin.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnLogin_Click);
			this.imagebtnRegister.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnRegister_Click);
			this.imagebtnLogount.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnLogount_Click);
			this.imagebtnmain.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnmain_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void imagebtnLogin_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			string id = txtID.Text;
			string pwd = txtPWD.Text;
			
			//����� ���̵�, ��й�ȣ üũ
			if(Authenticate(id,pwd))
			{							
				Response.Redirect("Main.aspx");
			}
			else
			{
				lblUser.Visible = true;
				lblUser.Text = "<b>�α��ο� �����ϼ̽��ϴ�</b>";
			}   
		}


		//���̵�, �н����� �˻�
		private bool Authenticate(string id, string pwd)
		{
			bool bExist = false;			
			
			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText ="Select ID from Member Where ID = @ID and PWD = @PWD";

			Cmd.Parameters.Add("@ID", SqlDbType.VarChar, 20);
			Cmd.Parameters.Add("@PWD", SqlDbType.VarChar, 20);
			
			Cmd.Parameters["@ID"].Value = id;
			Cmd.Parameters["@PWD"].Value = pwd;
			
			try
			{
				Con.Open();				
				if(Cmd.ExecuteScalar() != null)		//��ġ�Ǵ� ID�� PWD�� �ִٸ�
				{
					bExist = true;		
					Application["CurrentID"] = id;	 //ID�� ������ ����
					FormsAuthentication.SetAuthCookie(id, false);  //�α����� �ٷ� ������������ ����
				}					
			}
			catch (System.Exception e)
			{
				lblUser.Text = "������ ���� ������ �����Ͽ����ϴ�. ������ ������ �����ϴ�. <br />";
				lblUser.Text += e.Message;
			}
			finally
			{
				Con.Close();
			}
			return bExist;
		}

		
		private void imagebtnRegister_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			Response.Redirect("RegisterMember.aspx");
		}	

		private void imagebtnLogount_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			//�α׾ƿ�
			FormsAuthentication.SignOut();	

			imageID.Visible = true;
			imagePWD.Visible = true;
			txtID.Visible = true;
			txtPWD.Visible = true;
			imagebtnLogin.Visible = true;
			imagebtnRegister.Visible = true;
			imagebtnLogount.Visible = false;
		}

		private void imagebtnmain_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			Response.Redirect("Main.aspx");
		}
	}
}
