// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__444C0E78_9AD3_4FE9_AF11_E9296A7382A2__INCLUDED_)
#define AFX_STDAFX_H__444C0E78_9AD3_4FE9_AF11_E9296A7382A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "agtsvr.h"
#include "Agent.h"
//#include "HtmlHelp.h"
//#pragma comment(lib, "HtmlHelp.lib")


#define TOOL_REGKEY			_T("Tool")
#define TOOL_REGVALUE_WEB	_T("WebAddress")
#define TOOL_DEF_WEB		_T("http://")
#define MAX_URL				255
#define CX_PROGRESS	250
#define CY_PROGRESS	17

#define MESSAGE(id)	((g_Message.LoadString(id)) ?	\
					(g_Message) : ("치명적 오류가 발생했습니다.관리자에게 문의하세요."))	
extern CString	g_Message;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__444C0E78_9AD3_4FE9_AF11_E9296A7382A2__INCLUDED_)
