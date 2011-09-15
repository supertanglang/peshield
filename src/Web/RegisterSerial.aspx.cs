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
	/// RegisterSerial�� ���� ��� �����Դϴ�.
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
			this.imagebtnSerail.Click += new System.Web.UI.ImageClickEventHandler(this.imagebtnSerail_Click);
			this.install.Click += new System.Web.UI.ImageClickEventHandler(this.install_Click);
			this.software.Click += new System.Web.UI.ImageClickEventHandler(this.software_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void imagebtnSerail_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			if(IsP_SerialExists())	//products���̺��� �ش�Ǵ� �ø����� �ִٸ�
			{	
				if(IsS_SerialExists())	//suc���̺� �ش�Ǵ� �ø����� ���ٸ�
				{
					AddSuc();	//suc���̺� �ش�Ǵ� �ø���� ���̵� ����
					imagesoft.Visible = false;
					txtSERIAL.Visible = false;
					imagebtnSerail.Visible = false;		
					imageserialwrong.Visible = false;
					install.Visible = true;
					software.Visible = true;
					imagesuccess.Visible = true;
				}
				else	//suc���̺� �ش�Ǵ� �ø����� �ִٸ�
				{
					if(IsS_IDExists())	//suc���̺��� �ش�Ǵ� ���̵� �ִٸ�
					{
						imagesoft.Visible = false;
						txtSERIAL.Visible = false;
						imagebtnSerail.Visible = false;
						imageserialwrong.Visible = false;
						install.Visible = true;
						software.Visible = true;						
						imagesuccess.Visible = true;
					}
					else	//suc���̺��� �ش�Ǵ� �ø����� ���ٸ�
					{
						imageserialwrong.Visible = true;						
					}					
				}				
			}
			else	//products���̺��� �ش�Ǵ� �ø����� ���ٸ�
			{				
				imageserialwrong.Visible = true;				
			}
		}

		//products���̺��� �ø����� ���翩�� �Ǵ� 
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
				if(Cmd.ExecuteScalar() != null)		//�ش�Ǵ� �ø���Ű�� �����Ѵٸ�
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

		//Suc���̺��� �ø����� ���翩���Ǵ�
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
				if(Cmd.ExecuteScalar() != null)		//�ش�Ǵ� �ø���Ű�� �����Ѵٸ�
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

		//Suc���̺��� ID�� ���翩���Ǵ�
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
				if(Cmd.ExecuteScalar() != null)		//�ش�Ǵ� ID�� �����Ѵٸ�
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

		//suc ���̺� �ø���Ű�� ���̵� �Է��ϱ�		
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
				Cmd.ExecuteNonQuery();		//suc���̺� �ø����ȣ�� ���̵� �Է�
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
			FormsAuthentication.SignOut();	//�α׾ƿ�

			//����ƮŬ���̾�Ʈ���� SERIAL���� �ѱ��. 
			string ProgramName = txtSERIAL.Text;									
			string url = "http://203.247.220.211/SENOL/SmartClient.aspx?Serial=";
			Response.Redirect(url + ProgramName);

			//http://203.247.221.245/SENOL/SmartClient/SmartClient.aspx?Serial=ProgramName
			//���Ͱ��� �ٷ� ���ڰ��� �ѱ�� �Ѿ�� �ʴ´�. 
		}

	
	}
}
