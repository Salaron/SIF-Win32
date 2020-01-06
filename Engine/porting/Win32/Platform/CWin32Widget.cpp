﻿/* 
   Copyright 2013 KLab Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "CWin32Widget.h"
#include "CWin32Platform.h"
#include "CPFInterface.h"
#include <ADSIid.h>
#include <ExDisp.h>
#include <comdef.h>
#include <ctime>

#include <vector>

CWin32Widget *  CWin32Widget::m_pBegin   = NULL;
CWin32Widget *  CWin32Widget::m_pEnd     = NULL;
int             CWin32Widget::m_uniqBase = 0;

CWin32Widget::CWin32Widget(CWin32Platform * pPlatform)
: m_id      (0)
, m_hWnd    (0)
, m_prev    (NULL)
, m_next    (NULL)
, m_x       (0)
, m_y       (0)
, m_width   (0)
, m_height  (0)
, m_bVisible(true)
, m_bEnable (true)
, m_bMove   (false)
, m_chgVisible  (false)
, m_chgEnable   (false)
, m_pPlatform   (pPlatform)
, m_uniqId      (++m_uniqBase)
{
	// 自身をリンクに追加する
	m_prev = m_pEnd;
	m_pEnd = this;
	if(m_prev) {
		m_prev->m_next = this;
	} else {
		m_pBegin = this;
	}
}

CWin32Widget::~CWin32Widget()
{
	// コントロールを破棄する
	if(m_hWnd) DestroyWindow(m_hWnd);

	// 自身をリンクから外す
	if(m_prev) {
		m_prev->m_next = m_next;
	} else {
		m_pBegin = m_next;
	}
	if(m_next) {
		m_next->m_prev = m_prev;
	} else {
		m_pEnd = m_prev;
	}
}

void
CWin32Widget::cmd(int /*cmd*/, ...)
{
}

int
CWin32Widget::status()
{
	return 0; 
}

bool
CWin32Widget::init(HWND hWnd, int id, int x, int y, int width, int height)
{
	m_id     = id;
	m_hWnd   = hWnd;
	m_x      = x;
	m_y      = y;
	m_width  = width;
	m_height = height;
	return true;
}

int
CWin32Widget::getTextLength()
{
	int length  = GetWindowTextLengthW(m_hWnd);
	wchar_t * pBuf = new wchar_t [length + 1];
	GetWindowTextW(m_hWnd, (LPWSTR)pBuf, length + 1);

	// control から取得した文字列は ShiftJISなので、
	// Engineが使用する UTF-8 文字列に変換したものの長さを計測する。
	//size_t charlen = (length + 1) * 2 * sizeof(wchar_t);
	//char * utf8Str = new char[charlen];
	/*
	size_t res = wcstombs(utf8Str, pBuf, charlen);
	length = strlen(utf8Str);
	delete [] utf8Str;
	delete [] pBuf;
	*/

	return WideCharToMultiByte(CP_UTF8, 0, pBuf, -1, NULL, 0, NULL, NULL);
}

bool
CWin32Widget::getText(char * pBuf, int maxlen)
{
	wchar_t* a = new wchar_t[maxlen + 1];
	int result = GetWindowTextW(m_hWnd, a, maxlen + 1);
	// 受けた文字列はShiftJISなので、EngineとしてはUTF8に変換する
	//const char * utf8Str = m_pPlatform->SJIStoUTF8(pBuf);
	result = WideCharToMultiByte(CP_UTF8, 0, a, -1, pBuf, maxlen, NULL, NULL);
	/*
	int len = strlen(utf8Str);
	if(len > maxlen) len = maxlen;
	strncpy(pBuf, utf8Str, len);	// utf8に変換したものをバッファにコピー
	pBuf[len] = 0;
	delete [] utf8Str;
	*/
	delete[] a;
	return (!result) ? false : true;
}

bool
CWin32Widget::setText(const char * string)
{
	/*
	// 渡された文字列はutf8なので、OSのコントロールに渡す前に ShiftJISに変換する
	size_t l = strlen(string);
	size_t l_w = (l + 1) * 4;
	wchar_t* a = new wchar_t[l_w];
	//const char * sjisStr = m_pPlatform->utf8toSJIS(string);
	mbstowcs(a, string, l_w - 1);
	int result = SetWindowTextW(m_hWnd, a);
	delete[] a;
	//delete [] sjisStr;
	*/
	int result;
	size_t len = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
	wchar_t* a = new wchar_t[len + 1];
	result = MultiByteToWideChar(CP_UTF8, 0, string, -1, a, len);

	if (result)
		result = SetWindowTextW(m_hWnd, a);

	delete[] a;
	return result;
}

const char *
CWin32Widget::toShiftJIS(const char * utf8)
{
	return m_pPlatform->utf8toSJIS(utf8);
}

void
CWin32Widget::move(int x, int y)
{
	m_x = x;
	m_y = y;
	m_bMove = true;
}

void
CWin32Widget::resize(int width, int height)
{
	m_width  = width;
	m_height = height;
	m_bMove  = true;
}

void
CWin32Widget::visible(bool bVisible)
{
	m_bVisible   = bVisible;
	m_chgVisible = true;
}

void
CWin32Widget::enable(bool bEnable)
{
	m_bEnable   = bEnable;
	m_chgEnable = true;
}

void
CWin32Widget::redraw()

{
    if(!m_hWnd) { return; }

	if(m_chgVisible) {
		m_chgVisible = false;
		ShowWindow(m_hWnd, (m_bVisible) ? SW_SHOW : SW_HIDE);
	}
	if(m_chgEnable) {
		m_chgEnable = false;
		EnableWindow(m_hWnd, (m_bEnable) ? TRUE : FALSE);
	}
	if(m_bMove) {
		m_bMove = false;
		MoveWindow(m_hWnd, m_x, m_y, m_width, m_height, true);
	} else {
		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	RedrawWindow(m_hWnd, NULL, 0, RDW_UPDATENOW);
}

void
CWin32Widget::ReDrawControls()
{
	CWin32Widget * pCtrl = m_pBegin;
	while(pCtrl) {
		pCtrl->redraw();
		pCtrl = pCtrl->m_next;
	}
}

LRESULT
CWin32Widget::msgCommand(HWND /*hWnd*/, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 0;
}

LRESULT
CWin32Widget::ControlCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	int id = LOWORD(wParam);
	CWin32Widget * pCtrl = m_pBegin;
	while(pCtrl) {
		if(pCtrl->m_uniqId == id) {
			lResult = pCtrl->msgCommand(hWnd, message, wParam, lParam);
			break;
		}
		pCtrl = pCtrl->m_next;
	}
	return lResult;
}


CWin32TextWidget::CWin32TextWidget(CWin32Platform * pParent, int maxlen)
: CWin32Widget  (pParent)
, m_placeholder (NULL)
, m_touched     (false)
, m_maxlen      (maxlen)
, m_hFont       (0)
{
	m_cols[0] = 0xffffffff;
	m_cols[1] = 0xff000000;
	m_cols[2] = 0xffffffff;
	m_cols[3] = 0xff000000;
}
CWin32TextWidget::~CWin32TextWidget()
{
	delete [] m_placeholder;
}

bool
CWin32TextWidget::create(IWidget::CONTROL type, int id, const char * caption,
							int x, int y, int width, int height)
{
	HWND hWnd = 0;
	wchar_t* caption_str = NULL;

	{
		// Do char to wchar conversion
		size_t len = (strlen(caption) + 1) * 4;
		// Might be too overkill
		caption_str = new wchar_t[len];
		mbstowcs(caption_str, caption, len);
	}

	switch(type)
	{
	default:
		// unknown control.
		return false;
	case TEXTBOX:
		{
			hWnd = CreateWindowExW(0,
								L"EDIT",
								caption_str,
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
								x, y, width, height,
								getPlatform().get_hWnd(),
								(HMENU)(INT_PTR)m_uniqId,
								(HINSTANCE)GetModuleHandle(NULL),
								0);
		}
		break;
	case PASSWDBOX:
		{
			hWnd = CreateWindowExW(0,
								L"EDIT",
								caption_str,
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT|ES_PASSWORD,
								x, y, width, height,
								getPlatform().get_hWnd(),
								(HMENU)(INT_PTR)m_uniqId,
								(HINSTANCE)GetModuleHandle(NULL),
								0);
		}
		break;
	}
    if(m_maxlen > 0) { SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM)m_maxlen, 0); }

	return init(hWnd, id, x, y, width, height);
}

void
CWin32TextWidget::cmd(int cmd, ...)
{
	va_list ap;
	va_start(ap, cmd);

	switch(cmd)
	{
	case TX_PLACEHOLDER:
		{
			const char * placeholder = va_arg(ap, const char *);
			// 文字列を Shift_JIS に変換
			m_placeholder = toShiftJIS(placeholder);
		}
		break;
	case TX_FONT:
		{
			// フォントオブジェクト取得
			/*
			CWin32Font * fontobj = (CWin32Font *)va_arg(ap, void *);
			HDC hDC = GetDC( get_hWnd() );
			m_hFont = fontobj->get_hFont();
			SelectObject( hDC, m_hFont );
			*/
		}
		break;
	default:
		{
			unsigned int color = va_arg(ap, unsigned int);
			color = (color & 0x00ff00) | ((color&0xff) << 16) | ((color>>16) & 0xff);
			switch(cmd)
			{
			case TX_BGCOLOR_NORMAL: m_cols[0] = color; break;
			case TX_FGCOLOR_NORMAL: m_cols[1] = color; break;
			case TX_BGCOLOR_TOUCH : m_cols[2] = color; break;
			case TX_FGCOLOR_TOUCH : m_cols[3] = color; break;
			}
			set_color();
		}
		break;
	}
	va_end(ap);
}

void
CWin32TextWidget::set_color()
{
	int base = (m_touched) ? 2 : 0;
	unsigned int bg = m_cols[base];
	unsigned int fg = m_cols[base + 1];

	HDC hDC = GetDC( m_hWnd );
	SetTextColor(hDC, fg);
	SetBkColor(hDC, bg);
	ReleaseDC(m_hWnd, hDC);
}

void
CWin32TextWidget::redraw()
{
	CWin32Widget::redraw();

	if(m_placeholder && getTextLength() == 0) {
		RECT rc;
		HDC hDC = GetDC( m_hWnd );
		int savedDC = SaveDC(hDC);

		int base = (m_touched) ? 2 : 0;

		SetTextColor(hDC , 0x999999);
		SetRect( &rc, 0, 0, m_width, m_height );
		DrawText(hDC, m_placeholder, -1, &rc, 0); 
		RestoreDC(hDC, savedDC);
		ReleaseDC( m_hWnd, hDC );
	}
}


LRESULT
CWin32TextWidget::msgCommand(HWND /*hWnd*/, UINT /*message*/, WPARAM wParam, LPARAM /*lParam*/)
{
	int code = HIWORD(wParam);
	switch(code)
	{
	case EN_SETFOCUS:
		m_touched = true;
		set_color();
		break;
	case EN_KILLFOCUS:
		m_touched = false;
		set_color();
		break;
	default:
		break;
	}
	return 0;
}








CWin32WebWidget::CWin32WebWidget(CWin32Platform * pParent) : CWin32Widget(pParent) {}
CWin32WebWidget::~CWin32WebWidget() {}

bool
CWin32WebWidget::create(IWidget::CONTROL type, int id, const char * caption,
							int x, int y, int width, int height,
							const char * token, const char * region, const char * client,
							const char * consumerKey, const char * applicationId, const char * userID)
{

	HWND hWnd = 0;
	void (*AtlAxWinInit)();
	HRESULT (__stdcall *AtlAxGetControl)(HWND,IUnknown **);
	HMODULE hAtl = LoadLibrary("atl");
	AtlAxWinInit = (void (*)())GetProcAddress(hAtl ,"AtlAxWinInit");
	AtlAxGetControl = (HRESULT (__stdcall *)(HWND, IUnknown **))GetProcAddress(hAtl, "AtlAxGetControl");
	AtlAxWinInit();

	switch(type)
	{
	default:
		// unknown control.
		return false;
	case WEBVIEW:
	case WEBNOJUMP:
		{
			hWnd = 0;
			
			hWnd = CreateWindow("AtlAxWin", "Shell.Explorer.2",
								WS_CHILD|WS_VISIBLE, x, y, width, height, 
								getPlatform().get_hWnd(),
								(HMENU)0, (HINSTANCE)GetModuleHandle(NULL), NULL);
			IUnknown * hUnknown;
			if(AtlAxGetControl(hWnd, &hUnknown) == S_OK) {
				// FIXME: set headers after navigation to other page
				IWebBrowser2* browser;
				hUnknown->QueryInterface(IID_IWebBrowser2,(void **)&browser);
				VARIANT url, header, nil;
				VariantInit(&url);
				VariantInit(&header);
				VariantInit(&nil);
				url.vt = VT_BSTR;
				header.vt = VT_BSTR;
				nil.vt = VT_BSTR;
				nil.bstrVal = NULL;
				url.bstrVal = _bstr_t(caption);
				char *authorize = KLBNEWA(char, 1000), *user_id = KLBNEWA(char, 500);
				if (token) {
					sprintf(authorize, "Authorize: consumerKey=%s&timeStamp=%d&version=1.1&token=%s&nonce=WV0\r\n"
						,consumerKey, (int)time(NULL), token);
				} else {
					sprintf(authorize, "Authorize: consumerKey=%s&timeStamp=%d&version=1.1&nonce=WV0\r\n"
						,consumerKey, (int)time(NULL));
				}
				if (userID) {
					sprintf(user_id, "User-ID: %s\r\n", userID);
				} else {
					user_id[0] = '\0';
				}
				const char* platform_info_const = CPFInterface::getInstance().platform().getPlatform();
				char* platform_info = KLBNEWA(char, 1024);
				sprintf(platform_info, "%s", platform_info_const);
				char* os_version = strchr(platform_info, ';');
				*os_version = 0;
				os_version++;
				*(strchr(os_version, ';')) = 0;
				int type = 2; //安卓
				if (strcmp(platform_info, "Win32") == 0) {
					sprintf(platform_info, "iOS");
					sprintf(os_version, "iPad4_4 iPad 7.0");
					type = 1;
				}
				else if (strcmp(platform_info, "iOS") == 0) {
					type = 1;
				}

				char *cHeader = KLBNEWA(char, 2000);
				sprintf(cHeader, "accept: */*\r\nAPI-Model: straightforward\r\nBundle-Version: %s\r\nClient-Version: %s\r\nOS-Version: %s\r\nOS: %s\r\nPlatform-Type: %d\r\nApplication-ID: %s\r\nTime-Zone: JST\r\nRegion: %s\r\n%s%s\r\nX-Requested-With: klb.android.lovelive"
					, CPFInterface::getInstance().platform().getBundleVersion(), client, os_version, platform_info, type, applicationId, region, authorize, user_id);
				KLBDELETEA(authorize);
				KLBDELETEA(user_id);
				KLBDELETEA(platform_info);
				header.bstrVal = _bstr_t(cHeader);
				browser->Navigate2(&url, &nil, &nil, &nil, &header);
				KLBDELETEA(cHeader);
				VariantClear(&url);
				VariantClear(&header);
				VariantClear(&nil);
			}
			
		}
		break;
	}
	return init(hWnd, id, x, y, width, height);
}




CWin32MovieWidget::CWin32MovieWidget(CWin32Platform * pParent) : CWin32Widget(pParent) {}
CWin32MovieWidget::~CWin32MovieWidget() {}

bool
CWin32MovieWidget::create(IWidget::CONTROL type, int id, const char * /*caption*/,
							int x, int y, int width, int height)
{
	HWND hWnd = 0;

	switch(type)
	{
	default:
		// unknown control.
		return false;
	case MOVIEPLAYER:
		{
			hWnd = 0;
					/*CreateWindow(TEXT("MovieArea"),
								caption,
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
								x, y, width, height,
								getPlatform().get_hWnd(),
								(HMENU)(INT_PTR)id,
								(HINSTANCE)GetModuleHandle(NULL),
								0);
								*/

		}
		break;
	case BGMOVIEPLAYER:
		{
			hWnd = 0;
		}
		break;
	}
	return init(hWnd, id, x, y, width, height);
}



CWin32ActivityIndicator::CWin32ActivityIndicator(CWin32Platform * pParent) : CWin32Widget(pParent) {}
CWin32ActivityIndicator::~CWin32ActivityIndicator() {}

bool
CWin32ActivityIndicator::create(IWidget::CONTROL type, int id, const char * /*caption*/,
								int x, int y, int width, int height)
{
	HWND hWnd = 0;

	switch(type)
	{
	default:
		// unknown control.
		return false;
	case ACTIVITYINDICATOR:
		{
			hWnd = 0;
					/*CreateWindow(TEXT("MovieArea"),
								caption,
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
								x, y, width, height,
								getPlatform().get_hWnd(),
								(HMENU)(INT_PTR)id,
								(HINSTANCE)GetModuleHandle(NULL),
								0);
								*/

		}
		break;
	}
	return init(hWnd, id, x, y, width, height);
}
