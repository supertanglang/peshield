<%@ Register TagPrefix="uc1" TagName="SmartTop" Src="SmartTop.ascx" %>
<%@ Register TagPrefix="uc1" TagName="DefaultBottom" Src="../DefaultBottom.ascx" %>
<%@ Page %>
<HTML>
	<HEAD>
		<title>2차 암호화모듈 실행 페이지</title>
		<META http-equiv="Content-Type" content="text/html; charset=ks_c_5601-1987">
		<style type="text/css"> .title { FONT-SIZE: 9pt } </style>
		<LINK href="../Members.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body leftMargin="0" topMargin="0">
	<uc1:SmartTop id="SmartTop1" runat="server"></uc1:SmartTop>
		<table cellSpacing="0" cellPadding="0" rules="none" width="900" border="1">
			<tr>
				<td align="left" width="830" bgColor="#5a9cd6"><b><font color="white">소프트웨어 인증을 받아주세요</font></b></td>
			</tr>
		</table>
		<table cellSpacing="0" cellPadding="0" rules="none" width="900" border="1">
			<tr>
				<td align="center"><img src="../image/123.jpg" width="380"></td>				
			</tr>
			<tr>
				<td align="center">
					<OBJECT class="title" height="144" width="347" border="1" classid="SNCertify.DLL#SNCertify.Control"
						VIEWASTEXT>
						<PARAM NAME="g_param1" VALUE="<%=Request.QueryString["Serial"]%>">
					</OBJECT>
				</td>
			</tr>
		</table>		
		<uc1:DefaultBottom id="DefaultBottom1" runat="server"></uc1:DefaultBottom>		
	</body>
</HTML>
