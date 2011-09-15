<%@ Page language="c#" Codebehind="Default.aspx.cs" AutoEventWireup="false" Inherits="SENOL._Default" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>Welcome SENOL</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<LINK href="Members.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body leftMargin="0" topMargin="0">
		<form id="Default" method="post" runat="server">
			<table height="100%" cellSpacing="0" cellPadding="0" width="100%" border="0">
				<TBODY>
					<tr>
						<td vAlign="middle" align="center">
							<table cellSpacing="0" cellPadding="0" width="590" border="0">
								<TBODY>
									<tr>
										<td colSpan="3"><IMG height="94" src="image/img_titletd.gif" width="550"></td>
									</tr>
									<tr align="center">
										<td height="300"><IMG height="241" src="image/flash.gif" width="170" border="0"></A></td>
										<td height="300"><IMG height="241" src="image/people.gif" width="170" border="0"></A></td>
										<td height="300"><IMG height="241" src="image/icon.gif" width="170" border="0"></A></td>
									</tr>
									<tr>
										<td colSpan="3" height="189">
											<OBJECT codeBase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0"
												height="189" width="605" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" VIEWASTEXT>
												<PARAM NAME="_cx" VALUE="16007">
												<PARAM NAME="_cy" VALUE="5001">
												<PARAM NAME="FlashVars" VALUE="">
												<PARAM NAME="Movie" VALUE="effect23.swf">
												<PARAM NAME="Src" VALUE="effect23.swf">
												<PARAM NAME="WMode" VALUE="Window">
												<PARAM NAME="Play" VALUE="-1">
												<PARAM NAME="Loop" VALUE="-1">
												<PARAM NAME="Quality" VALUE="High">
												<PARAM NAME="SAlign" VALUE="">
												<PARAM NAME="Menu" VALUE="-1">
												<PARAM NAME="Base" VALUE="">
												<PARAM NAME="AllowScriptAccess" VALUE="">
												<PARAM NAME="Scale" VALUE="ShowAll">
												<PARAM NAME="DeviceFont" VALUE="0">
												<PARAM NAME="EmbedMovie" VALUE="0">
												<PARAM NAME="BGColor" VALUE="">
												<PARAM NAME="SWRemote" VALUE="">
												<PARAM NAME="MovieData" VALUE="">
												<PARAM NAME="SeamlessTabbing" VALUE="1">
												<PARAM NAME="Profile" VALUE="0">
												<PARAM NAME="ProfileAddress" VALUE="">
												<PARAM NAME="ProfilePort" VALUE="0">
												<PARAM NAME="AllowNetworking" VALUE="all">
												<embed src="effect01.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer"
													type="application/x-shockwave-flash" width="605" height="189"> </embed>
											</OBJECT>
										</td>
									</tr>
									<tr>
										<td colSpan="3"><br>
										</td>
									</tr>
									<tr>
										<td align="right"><asp:image id="imageID" Runat="server" ImageUrl="image/id.gif"></asp:image></td>
										<td align="center"><asp:textbox id="txtID" Width="150" Runat="server" TabIndex="1"></asp:textbox></td>
										<td align="left"><asp:imagebutton id="imagebtnLogin" Runat="server" ImageUrl="image/b_login.gif" Height="17" TabIndex="3"></asp:imagebutton><asp:requiredfieldvalidator id="RetxtID" Runat="server" ControlToValidate="txtID" Display="None" ErrorMessage="아이디를 입력해주세요"></asp:requiredfieldvalidator></td>
									</tr>
									<tr>
										<td align="right"><asp:image id="imagePWD" Runat="server" ImageUrl="image/pwd.gif"></asp:image></td>
										<td align="center"><asp:textbox id="txtPWD" Width="150" Runat="server" TextMode="Password" TabIndex="2"></asp:textbox></td>
										<td align="left"><asp:imagebutton id="imagebtnRegister" Runat="server" ImageUrl="image/b_register.gif" Height="18"
												TabIndex="4" CausesValidation="False"></asp:imagebutton><asp:requiredfieldvalidator id="RetxtPWD" Runat="server" ControlToValidate="txtPWD" Display="None" ErrorMessage="비밀번호를 입력해주세요"></asp:requiredfieldvalidator></td>
									</tr>
									<tr>
										<td align="center" colSpan="3"><asp:label id="lblUser" Runat="server" Visible="False"></asp:label></td>
									</tr>
									<tr>
										<td align="center" colSpan="3"><asp:imagebutton id="imagebtnLogount" Runat="server" ImageUrl="image/b_logout.gif" Visible="False"></asp:imagebutton>&nbsp;&nbsp;&nbsp;&nbsp;
											<asp:ImageButton id="imagebtnmain" Runat="server" ImageUrl="image/tomain.gif" Visible="False"></asp:ImageButton></td>
									</tr>
									<tr>
										<td align="center" colSpan="3"><asp:validationsummary id="ValidationSummary1" runat="server" ShowSummary="False" ShowMessageBox="True"
												DisplayMode="List"></asp:validationsummary></td>
									</tr>
									<tr>
										<td colSpan="3"><img src="image/bottom.gif" width="590" height="69"></td>
									</tr>
								</TBODY>
							</table>
						</td>
					</tr>
				</TBODY>
			</table>
		</form>
		</TR></TBODY></TABLE></TR></TBODY></TABLE></FORM>
	</body>
</HTML>
