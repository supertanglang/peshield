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
	/// _Default에 대한 요약 설명입니다.
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
	
		private string ConnectStr;	//DB접속

		private void Page_Load(object sender, System.EventArgs e)
		{
			// 여기에 사용자 코드를 배치하여 페이지를 초기화합니다.
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

		#region Web Form 디자이너에서 생성한 코드
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: 이 호출은 ASP.NET Web Form 디자이너에 필요합니다.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
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
			
			//사용자 아이디, 비밀번호 체크
			if(Authenticate(id,pwd))
			{							
				Response.Redirect("Main.aspx");
			}
			else
			{
				lblUser.Visible = true;
				lblUser.Text = "<b>로그인에 실패하셨습니다</b>";
			}   
		}


		//아이디, 패스워드 검사
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
				if(Cmd.ExecuteScalar() != null)		//일치되는 ID와 PWD가 있다면
				{
					bExist = true;		
					Application["CurrentID"] = id;	 //ID를 변수에 저장
					FormsAuthentication.SetAuthCookie(id, false);  //로그인후 바로 메인페이지로 접속
				}					
			}
			catch (System.Exception e)
			{
				lblUser.Text = "오류로 인해 저장이 실패하였습니다. 사유는 다음과 같습니다. <br />";
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
			//로그아웃
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
