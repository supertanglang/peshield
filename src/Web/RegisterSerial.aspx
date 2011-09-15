<%@ Page language="c#" Codebehind="RegisterSerial.aspx.cs" AutoEventWireup="false" Inherits="SENOL.RegisterSerial" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>Welcome SENOL</title>
		<meta http-equiv="Content-Type" content="text/html; charset=euc-kr">
		<script language="JavaScript" type="text/JavaScript">
<!--
function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}

function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}

function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}

function MM_swapImage() { //v3.0
  var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}
//-->
		</script>
	</HEAD>
	<body leftMargin="0" topMargin="0" onload="MM_preloadImages('images/01menu_o.gif','image/02menu_o.gif','image/03menu_o.gif','image/04menu_o.gif')">
		<form id="Default" method="post" runat="server">
			<table height="100%" cellSpacing="0" cellPadding="0" width="100%" border="0">
				<tr>
					<td vAlign="top" align="center" height="380">
						<table height="100%" cellSpacing="0" cellPadding="0" width="750" border="0">
							<tr align="right">
								<td colSpan="2" height="31"><IMG height="31" src="image/title_top.gif" width="149"></td>
							</tr>
							<tr>
								<td vAlign="top" width="125" background="image/bg_left.gif">
									<table cellSpacing="0" cellPadding="0" width="125" border="0">
										<tr>
											<td><A onmouseover="MM_swapImage('Image2','','image/01menu_o.gif',1)" onmouseout="MM_swapImgRestore()"
													href="Default.aspx"><IMG height="55" src="image/01menu.gif" width="125" border="0" name="Image2"></A></td>
										</tr>
										<tr>
											<td><A onmouseover="MM_swapImage('Image3','','image/02menu_o.gif',1)" onmouseout="MM_swapImgRestore()"
													href="RegisterSerial.aspx"><IMG height="55" src="image/02menu_o.gif" width="125" border="0" name="Image3"></A></td>
										</tr>
										<tr>
											<td><A onmouseover="MM_swapImage('Image4','','image/03menu_o.gif',1)" onmouseout="MM_swapImgRestore()"
													href="AboutSenol.aspx"><IMG height="55" src="image/03menu.gif" width="125" border="0" name="Image4"></A></td>
										</tr>
										<tr>
											<td><A onmouseover="MM_swapImage('Image5','','image/04menu_o.gif',1)" onmouseout="MM_swapImgRestore()"
													href="MemberBoard.aspx"><IMG height="55" src="image/04menu.gif" width="125" border="0" name="Image5"></A></td>
										</tr>
									</table>
								</td>
								<td vAlign="top" align="center" width="675">
									<table height="7" cellSpacing="0" cellPadding="0" width="625" border="0">
										<tr>
											<td vAlign="top" align="right">
												<table height="7" cellSpacing="0" cellPadding="0" width="615" border="0">
													<tr>
														<td background="image/bg_top.gif"></td>
													</tr>
												</table>
											</td>
										</tr>
									</table>
									<table cellSpacing="0" cellPadding="0" width="605" border="0">
										<tr>
											<td height="30"><IMG height="25" src="image/123.gif" width="157"></td>
										</tr>
										<tr>
											<td colSpan="3" height="189">
												<OBJECT codeBase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0"
													height="189" width="605" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" VIEWASTEXT>
													<PARAM NAME="_cx" VALUE="16007">
													<PARAM NAME="_cy" VALUE="5001">
													<PARAM NAME="FlashVars" VALUE="">
													<PARAM NAME="Movie" VALUE="effect22.swf">
													<PARAM NAME="Src" VALUE="effect22.swf">
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
											<td width="407" height="44"><IMG height="16" src="image/box_polraris.gif" width="378"><br>
											</td>
										</tr>
										<tr>
											<td align="center"><IMG src="image/serial.gif">
											</td>
										</tr>
									</table>
									<table cellSpacing="0" cellPadding="0" width="100%" border="0">
										<tr>
											<td align="right" width="200"><asp:image id="imagesoft" Runat="server" Width="150" ImageUrl="image/serialnumber.gif"></asp:image></td>
											<td align="center" width="200"><asp:textbox id="txtSERIAL" Runat="server" Width="150"></asp:textbox></td>
											<td align="left"><asp:imagebutton id="imagebtnSerail" Runat="server" ImageUrl="image/regi.gif"></asp:imagebutton></td>
										</tr>
										<tr>
											<td align="center" colSpan="3"><asp:image id="imageserialwrong" Runat="server" ImageUrl="image/serialwrong.gif" Visible="False"></asp:image></td>
										</tr>
										<tr>
											<td align="center" colSpan="3"><asp:image id="imagesuccess" Runat="server" ImageUrl="image/success1.gif" Visible="False"></asp:image></td>
										</tr>
									</table>
									<table cellSpacing="0" cellPadding="0" width="100%" border="0">
										<tr>
											<td><br>
											</td>
										</tr>
										<tr>
											<td align="center"><asp:imagebutton id="install" Runat="server" ImageUrl="image/install.gif" Visible="False"></asp:imagebutton></td>
											<td align="center"><asp:imagebutton id="software" Runat="server" ImageUrl="image/software.gif" Visible="False"></asp:imagebutton></td>
										</tr>
										<tr>
											<td align="center"><asp:requiredfieldvalidator id="ReSerial" Runat="server" ErrorMessage="시리얼번호를 입력해주세요" Display="None" ControlToValidate="txtSERIAL"></asp:requiredfieldvalidator></td>
											<td align="center"><asp:validationsummary id="ValidationSummary1" runat="server" DisplayMode="List" ShowMessageBox="True"
													ShowSummary="False"></asp:validationsummary></td>
										</tr>
										<tr>
										</tr>
									</table>
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</form>
	</body>
</HTML>
