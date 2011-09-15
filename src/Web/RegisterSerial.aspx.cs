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
	/// RegisterSerial에 대한 요약 설명입니다.
	/// </summary>
	public class RegisterSerial : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.Image imagesoft;
		protected System.Web.UI.WebControls.TextBox txtSERIAL;
		protected System.Web.UI.WebControls.ImageButton imagebtnSerail;
		protected System.Web.UI.WebControls.Image imageserialwrong;
		protected System.Web.UI.WebControls.Image imagesuccess;
		protected System.Web.UI.WebControls.ImageButton install;
		protected System.Web.UI.WebControls.ImageButton software;
		protected System.Web.UI.WebControls.RequiredFieldValidator ReSerial;
		protected System.Web.UI.WebControls.ValidationSummary ValidationSummary1;
	
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
			this.imagebtnSerail.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnSerail_Click);
			this.install.Click += new System.Web.UI.ImageClickEventHandler(this.install_Click);
			this.software.Click += new System.Web.UI.ImageClickEventHandler(this.software_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void imagebtnSerail_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			if(IsP_SerialExists())	//products테이블에서 해당되는 시리얼이 있다면
			{	
				if(IsS_SerialExists())	//suc테이블에 해당되는 시리얼이 없다면
				{
					AddSuc();	//suc테이블에 해당되는 시리얼과 아이디를 삽입
					imagesoft.Visible = false;
					txtSERIAL.Visible = false;
					imagebtnSerail.Visible = false;		
					imageserialwrong.Visible = false;
					install.Visible = true;
					software.Visible = true;
					imagesuccess.Visible = true;
				}
				else	//suc테이블에 해당되는 시리얼이 있다면
				{
					if(IsS_IDExists())	//suc테이블에서 해당되는 아이디가 있다면
					{
						imagesoft.Visible = false;
						txtSERIAL.Visible = false;
						imagebtnSerail.Visible = false;
						imageserialwrong.Visible = false;
						install.Visible = true;
						software.Visible = true;						
						imagesuccess.Visible = true;
					}
					else	//suc테이블에서 해당되는 시리얼이 없다면
					{
						imageserialwrong.Visible = true;						
					}					
				}				
			}
			else	//products테이블에서 해당되는 시리얼이 없다면
			{				
				imageserialwrong.Visible = true;				
			}
		}

		//products테이블에서 시리얼의 존재여부 판단 
		private bool IsP_SerialExists()
		{
			bool bExist = false;		
			
			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText="select serial from products where serial = @serial";
			
			Cmd.Parameters.Add("@serial", SqlDbType.VarChar, 20);			
			Cmd.Parameters["@serial"].Value = txtSERIAL.Text;			

			try
			{
				Con.Open();
				if(Cmd.ExecuteScalar() != null)		//해당되는 시리얼키가 존재한다면
				{
					bExist = true;										
				}
			}
			catch (System.Exception e){}
			finally
			{
				Con.Close();
			}
			return bExist;			
		}

		//Suc테이블에서 시리얼의 존재여부판단
		private bool IsS_SerialExists()
		{
			bool bExist1 = true;

			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText = "select Serial from suc where Serial = @serial";

			Cmd.Parameters.Add("@Serial", SqlDbType.VarChar, 20);
			Cmd.Parameters["@Serial"].Value = txtSERIAL.Text;			

			try
			{
				Con.Open();
				if(Cmd.ExecuteScalar() != null)		//해당되는 시리얼키가 존재한다면
				{
					bExist1 = false;								
				}
			}
			catch (System.Exception e){}
			finally
			{
				Con.Close();
			}
			return bExist1;			
		}		

		//Suc테이블에서 ID의 존재여부판단
		private bool IsS_IDExists()
		{
			bool bExist2 = false;			
			string CID = Application["CurrentID"].ToString();	

			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText="Select ID from Suc where Serial = @Serial and ID = @ID ";
		
			Cmd.Parameters.Add("@ID", SqlDbType.VarChar, 20);			
			Cmd.Parameters.Add("@Serial", SqlDbType.VarChar, 20);			
			Cmd.Parameters["@ID"].Value = CID;			
			Cmd.Parameters["@Serial"].Value = txtSERIAL.Text;			
	
			try
			{
				Con.Open();
				if(Cmd.ExecuteScalar() != null)		//해당되는 ID가 존재한다면
				{
					bExist2 = true;
				}
			}
			catch (System.Exception e){}
			finally
			{
				Con.Close();
			}
			return bExist2;	
		}

		//suc 테이블에 시리얼키와 아이디 입력하기		
		private void AddSuc()
		{			
			string CID = Application["CurrentID"].ToString();						

			SqlConnection Con = new SqlConnection(ConnectStr);
			SqlCommand Cmd = new SqlCommand();
			Cmd.Connection = Con;
			Cmd.CommandText ="insert into Suc (Serial, ID) values (@Serial, @ID)";			

			Cmd.Parameters.Add("@Serial", SqlDbType.VarChar, 20);
			Cmd.Parameters.Add("@ID", SqlDbType.VarChar, 20);
			Cmd.Parameters["@Serial"].Value = txtSERIAL.Text;			
			Cmd.Parameters["@ID"].Value = CID;

			try
			{
				Con.Open();
				Cmd.ExecuteNonQuery();		//suc테이블에 시리얼번호와 아이디 입력
			}		
			catch (System.Exception e)
			{
				
			}
			finally
			{
				Con.Close();
			}				
		}

		private void install_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			Response.Redirect("http://203.247.220.211/SENOL/security/SmartClient.msi"); 		
		}

		private void software_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			FormsAuthentication.SignOut();	//로그아웃

			//스마트클라이언트에게 SERIAL값을 넘긴다. 
			string ProgramName = txtSERIAL.Text;									
			string url = "http://203.247.220.211/SENOL/SmartClient.aspx?Serial=";
			Response.Redirect(url + ProgramName);

			//http://203.247.221.245/SENOL/SmartClient/SmartClient.aspx?Serial=ProgramName
			//위와같이 바로 인자값을 넘기면 넘어가지 않는다. 
		}

	
	}
}
