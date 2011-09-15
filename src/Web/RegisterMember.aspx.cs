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
	/// RegisterMember에 대한 요약 설명입니다.
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
	
		protected string ConnectStr;	//DB접속

		private void Page_Load(object sender, System.EventArgs e)
		{
			// 여기에 사용자 코드를 배치하여 페이지를 초기화합니다.
			ConnectStr = ConfigurationSettings.AppSettings["ConnectionString"];	
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
				AddMember();	//회원정보등록
			}
		}

		//입력된 아이디의 존재여부
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
				if(Cmd.ExecuteScalar() != null)	//해당되는 ID가 없다면
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

		//회원정보 등록
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
				Cmd.ExecuteNonQuery();	//회원정보DB에저장
				Application["CurrentID"] = id;	//ID변수에저장
				FormsAuthentication.SetAuthCookie(id, false); 

				//회원가입에 성공시 메세지 박스 출력
				string script="<script>";
				script += "alert('회원에 가입되셨습니다.');";
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
