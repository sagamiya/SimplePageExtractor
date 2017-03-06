// SimplePageExtractor.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "SimplePageExtractor.h"
#include "direct.h"
#include <string.h>
#include "mpplugin.h"
#include "resource.h"
#include "xdw_api.h"
#include "xdwapian.h"
#include "aboutdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE 
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC 
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp

BEGIN_MESSAGE_MAP(CSimplePageExtractorApp, CWinApp)
	//{{AFX_MSG_MAP(CSimplePageExtractorApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp の構築

CSimplePageExtractorApp::CSimplePageExtractorApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CSimplePageExtractorApp オブジェクト

CSimplePageExtractorApp theApp;


/* 以降がプラグインのSPIの実装です */


CString CSimplePageExtractorApp::pluginName = CString("簡易ページ収集");

CString CSimplePageExtractorApp::internalPluginName = CString("SimplePageExtractor");

class MyData {
public:
	DWORD	style;
public:
	MyData() { style = 0; };
	bool SetMyData(CString instanceName);
	bool GetMyData(CString instanceName);
	void DeleteMyData(CString instanceName);

	bool GetRemoveFusen() {
		return (style & 1) == 1;
	}
	void SetRemoveFusen(BOOL remove) {
		if (remove) {
			style = 1;
		} else {
			style = 0;
		}
	}
};


void print_error(int code)
{
	switch (code) {
	case XDW_E_NOT_INSTALLED:
		AfxMessageBox("DocuWorksがインストールされていません。");
		break;
	case XDW_E_FILE_NOT_FOUND:
		AfxMessageBox("指定されたファイルが見つかりません。");
		break;
	case XDW_E_FILE_EXISTS:
		AfxMessageBox("指定されたファイルはすでに存在します。");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		AfxMessageBox("指定されたファイルを開くことができません。");
		break;
	case XDW_E_BAD_FORMAT:
		AfxMessageBox("指定されたファイルは正しいフォーマットではありません。");
		break;
	case XDW_E_INVALID_ACCESS:
		AfxMessageBox("指定された操作をする権利がありません。");
		break;
	default:
		AfxMessageBox("エラーが発生しました。");
		break;
	}
}


bool RemoveFusen(XDW_DOCUMENT_HANDLE fascicle) {
	bool ok = false;
	int		j;

	XDW_DOCUMENT_INFO info = { sizeof(XDW_DOCUMENT_INFO), 0, 0, 0 };
	XDW_GetDocumentInformation(fascicle, &info);
	int last_page = info.nPages;

	XDW_PAGE_INFO	pageInfo;
	pageInfo.nSize = sizeof XDW_PAGE_INFO;

	for (int page = 1; page <= last_page; page++) {
		bool done = false;// 最初のループはかならず実行させるため。
		while (!done) {
			done = true;
			int api_result = XDW_GetPageInformation( fascicle, page, &pageInfo);
			if (api_result < 0 ) {
				print_error(api_result);
				goto exit_all;
			}
			for (j=1; j<= pageInfo.nAnnotations; j++) {
				XDW_ANNOTATION_INFO annotationInfo;
				annotationInfo.nSize = sizeof annotationInfo;
				api_result = XDW_GetAnnotationInformation(fascicle, page, NULL, j, &annotationInfo, NULL);
				if (api_result < 0 ) {
					print_error(api_result);
					goto exit_all;
				}
				if (annotationInfo.nAnnotationType == XDW_AID_FUSEN) {
					const int sizeOfValue = sizeof ( int );
					int		value;
					api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_FillColor, (char *)(&value), sizeOfValue, NULL);
					if (value == XDW_COLOR_FUSEN_LIME) {
						api_result = XDW_RemoveAnnotation(fascicle, annotationInfo.handle, NULL);
						done = false;
						break;
					}
				}
			}
		}
	}
	ok = true;
exit_all:
	return ok;
}

char * TmpFileName(const char *folder) {
//	return CString( tmpnam((char *)folder) );
	return _tempnam(folder,"tmp");
}

bool DoSplit (const char *path, const char *folder, const char *filename, MyData *mydata) {
	XDW_DOCUMENT_HANDLE fascicle = NULL;
	int		fascicleLastPage = 0;
	int api_result = 0;
	CString fascicleName = CString(folder)+"\\"+filename+"-抽出.xdw";
	char * tmpFileName = TmpFileName(folder);

	XDW_DOCUMENT_HANDLE h = NULL;
	XDW_OPEN_MODE_EX mode = {
		sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_READONLY, XDW_AUTH_NODIALOGUE };
	api_result = XDW_OpenDocumentHandle(path, &h, (XDW_OPEN_MODE*)&mode);
	if (api_result < 0) {
		print_error(api_result);
		AfxMessageBox("Unexpected error: 選択された文書を開けませんでした。");
		return false;
	}

	// XDW_GetDocumentInformationを用いて総ページ数を得る
	XDW_DOCUMENT_INFO info = { sizeof(XDW_DOCUMENT_INFO), 0, 0, 0 };
	XDW_GetDocumentInformation(h, &info);
	int last_page = info.nPages;
	XDW_PAGE_INFO	pageInfo;
	pageInfo.nSize = sizeof XDW_PAGE_INFO;

	for (int page=1; page <= last_page; page++) {
		bool	isRequired = false;
		api_result = XDW_GetPageInformation( h, page, &pageInfo);
		if (api_result < 0 ) {
			print_error(api_result);
			goto exit_all;
		}
		for (int j=1; j<= pageInfo.nAnnotations; j++) {
			XDW_ANNOTATION_INFO annotationInfo;
			annotationInfo.nSize = sizeof annotationInfo;
			api_result = XDW_GetAnnotationInformation(h, page, NULL, j, &annotationInfo, NULL);
			if (api_result < 0 ) {
				print_error(api_result);
				goto exit_all;
			}
			if (annotationInfo.nAnnotationType == XDW_AID_FUSEN) {
				const int sizeOfValue = sizeof ( int );
				int		value;
				api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_FillColor, (char *)(&value), sizeOfValue, NULL);
				if (value == XDW_COLOR_FUSEN_LIME) {
					isRequired = true;
					break;
				}
			}
		}
		if (isRequired) {
			if (fascicle == NULL) {
				
				api_result =  XDW_GetPage(h, page, (LPCSTR)(fascicleName), NULL);
				if (api_result < 0 ) {
					print_error(api_result);
					AfxMessageBox(CString("Unexpected error: 付箋の付いたページを、")+fascicleName+"という名前で取り出すのに失敗.");
					goto exit_all;
				}
				XDW_OPEN_MODE_EX updateMode = {
					sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_UPDATE, XDW_AUTH_NODIALOGUE };
					api_result = XDW_OpenDocumentHandle((LPCSTR)(fascicleName), &fascicle, (XDW_OPEN_MODE*)&updateMode);
					if (api_result < 0 ) {
						print_error(api_result);
						AfxMessageBox("Unexpected error: 付箋の付いたページを取り出したファイルを開くことに失敗。");
						goto exit_all;
					}
					fascicleLastPage = 1;
			} else {
				api_result =  XDW_GetPage(h, page, CString(tmpFileName)+".xdw", NULL);
				if (api_result < 0 ) {
					AfxMessageBox("Unexpected error: failed to get page.");
					print_error(api_result);
					goto exit_all;
				}
				api_result =  XDW_InsertDocument(fascicle, fascicleLastPage+1, CString(tmpFileName)+".xdw", NULL);
				if (api_result < 0 ) {
					AfxMessageBox("Unexpected error: failed to insert document.");
					print_error(api_result);
					goto exit_all;
				}
				fascicleLastPage++;
				CFile::Remove(CString(tmpFileName)+".xdw");
			}
		}
	}
	if (mydata->GetRemoveFusen() && fascicle!= NULL) {
		if (!RemoveFusen(fascicle)) {
			AfxMessageBox("Unexpected error: 付箋の削除に失敗");
			goto exit_all;
		}
	};

exit_all:
	if (fascicle!= NULL) {
		api_result =  XDW_SaveDocument (fascicle, NULL); 
		if (api_result < 0 ) {
			print_error(api_result);
		}
		XDW_CloseDocumentHandle(fascicle, NULL);
	}
	XDW_CloseDocumentHandle(h, NULL);
	free(tmpFileName);
	return true;
}

bool DoIt (const char *fullpath, MyData *mydata) {

	int api_result = 0;

    char ext[MAX_PATH];
    char fname[MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

    _splitpath(fullpath, drive, dir, fname, ext);
    
	CString foldername = CString(drive) + CString(dir);

	return DoSplit(fullpath, (LPCSTR)foldername, fname, mydata);
}


PLUGGEDIN_EXPORT( long ) PLG_ExecuteCommand(PLUGGEDIN_STRUCT *ps)
{
    /* このコマンド実行の途中、引数として渡されるDWファイルは開放されています。
       読み書き可能でアクセスできます。
    */
    /* MFCを使用する場合、AFX_MANAGE_STATE( AfxGetStaticModuleState() );を
       必ずコールしてください。これにより、このDLLのリソースが使われるようになります。
    */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	MyData	mydata;
	if (!mydata.GetMyData(ps->ps_pszFunction)) {
		AfxMessageBox(CString("Error: failed to get mydata for ")+ps->ps_pszFunction);
	};

    char *buf=NULL;
    char *tmp1=NULL;
    char *path=NULL;

    buf=new char[32+(MAX_PATH * ps->ps_nFiles)];
    tmp1=new char[MAX_PATH];
    path=new char[MAX_PATH];
    strcpy(buf,"Arg files:");
	if ( ps->ps_nFiles > 0 ) {
		for (int i=0; i<ps->ps_nFiles; i++) {
			DoIt(ps->ps_pszFiles[i], &mydata);
		}
		AfxMessageBox("Done.");
	}

    delete buf;
    delete tmp1;
    delete path;
    return 1;
}

/* EnumerateCommands Entry */
PLUGGEDIN_EXPORT( long ) PLG_EnumerateCommands(char* pszEntries, long nBufSize, long* pnRealSize)
{
    // Check buffer size is large enough
    /* *pnRealSizeに必要なバッファサイズを入れてください */
    *pnRealSize=strlen((LPCSTR)(theApp.internalPluginName))+strlen((LPCSTR)(theApp.pluginName))+1;

    if (nBufSize<*pnRealSize)
        return 0;

    /* nBufSizeが必要なバッファサイズを超えている場合のみ、pszEntriesにコピーし
       成功したことを示す return 1を返してください
    */

    /*
     0ターミネートの文字列のリストを返してください。DWDeskのプラグイン/設定の候補に出ます
    */
   // DoCopy Commands!
    ::strcpy(pszEntries, (LPCSTR)(theApp.pluginName));
	*pnRealSize=strlen(pszEntries);
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Executable(const char *pszFunction, long nSel)
{
    //選択ファイルの数 nSelに応じて実行可能性を返してください
	if (nSel>0) {
		return 1;
	} else {
		return 0;
	}
}
PLUGGEDIN_EXPORT( long ) PLG_RequireFiles(const char *pszFunction)
{
    // 0を返すと、ファイル選択状態によらず引数は渡されません
	return 1;
}

PLUGGEDIN_EXPORT( HICON ) PLG_GetCommandIcon(const char *pszFunction, long bNormal)
{
    /* 常に大きな(32x32 or 24x24)アイコンでもかまいません */

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HICON hIcon=NULL;
	unsigned resID=0;
	
	if (CString(pszFunction).Left(theApp.internalPluginName.GetLength()) == theApp.internalPluginName) {
		resID= IDI_APPICON1;
	} else return 0;
//	if (!strcmp(pszFunction, (LPCSTR)(theApp.internalPluginName)))
//		resID= IDI_APPICON1;
//	else return 0;

	HINSTANCE myInst = (HINSTANCE) AfxGetInstanceHandle();
	if (bNormal)
		hIcon=LoadIcon(myInst,MAKEINTRESOURCE(resID));
	else
		hIcon=(HICON)LoadImage(myInst,MAKEINTRESOURCE(resID),IMAGE_ICON,16,16,0);
	return hIcon;
}

/* Followin SPI s  are NO-op */

PLUGGEDIN_EXPORT( long ) PLG_Initialize(const char* cmd)
{
    /* 必ず 1を返してください. 0を返すとプラグインがロードされません*/
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Finalize(const char*  cmd)
{
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_CanFinalize(const char*  cmd)
{
	return 1;
}

/*
    IsCloningCommand/CanSetProfileは 0以外を返すと、
    GetNewClone / SetProfileがコールされます

*/

PLUGGEDIN_EXPORT( long ) PLG_IsParallel(const char *pszFunction)
{
    /* v2.0ではこのSPIは無視されます */
	return 0;
}


PLUGGEDIN_EXPORT( long ) PLG_CanSetProfile(const char *pszFunction)
{
    /* SetProfileを有効にする場合、ここで1を返してください */
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_SetProfile(const char *pszFunction)
{
    /* ここでGUIを出して自身の設定を行ってください */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	MyData mydata;
	mydata.GetMyData(pszFunction);
	AboutDlg dlg;
	dlg.m_withFusen = mydata.GetRemoveFusen();
	dlg.DoModal();
	mydata.SetRemoveFusen(dlg.m_withFusen);
	mydata.SetMyData(pszFunction);
	return 1;
}


/* IsCloning Entry*/
PLUGGEDIN_EXPORT( long ) PLG_IsCloningCommand(const char *pszFunction)
{
    /* 同一機能で複数の設定を保存して動作させたい場合などは、この
       SPIで 1 を返してください
     */
    return 1;
}

/* GetNewClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_GetNewClone(char* pszNewClone, long bufsize)
{
    /* pszNewCloneとは異なった文字列を作成し、pszNewCloneを書き換えて返してください */
    /* 例えば "プラグイン機能" -> "プラグイン機能01","プラグイン機能02"など
       以降のセッションでは、クローンされた文字列が引数pszCommandとして渡されます
       クローンごとに設定を保存してください。
       PLUGGEDIN_REGKEY\ 以下のレジストリに保存されたデータは、
       Deskのプラグイン/設定ダイアログで OK が押されない限り、一括キャンセルされます
     */

	int i;
	MyData mydata;
	mydata.style = 0;
	for (i=0; i<10; i++) {
		CString number, name;
		number.Format("%02d",i);
		name = theApp.internalPluginName+number;
		if (!mydata.GetMyData(name)) {
			if (mydata.SetMyData(name)) {
				::strcpy(pszNewClone, (LPCSTR)name);
				return 1;
			};
		}
	}
    return 0;

}

/* ReleaseClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_ReleaseClone(const char* pszClone)
{
    /* pszCloneで指定されたコマンドが使用していた領域を開放してください */
	MyData mydata;
	mydata.DeleteMyData(CString(pszClone));
    return 1;

}

bool MyData::SetMyData(CString instanceName) {
	HKEY subKey;
	LONG result;
	DWORD type = REG_DWORD;
	DWORD cbData= sizeof DWORD;
	result = RegCreateKeyEx(HKEY_CURRENT_USER,
				CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName,
				0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &subKey, NULL);
	if (result == ERROR_SUCCESS) {
		result = RegSetValueEx(subKey, "style", NULL, type, (unsigned char *)&style, cbData);
		if (result == ERROR_SUCCESS) {
			//AfxMessageBox(CString ("Successfully set mydata."));
		} else {
			//AfxMessageBox(CString ("Filed to set value ."));
		}
	} else {
		//AfxMessageBox(CString ("Filed to open/create ")+CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName);
	}
	RegCloseKey(subKey);   
	return (result==ERROR_SUCCESS);
}

bool MyData::GetMyData(CString instanceName) {
	HKEY subKey;
	LONG result;
	DWORD type = REG_DWORD;
	DWORD cbData = sizeof DWORD;
	result = RegOpenKeyEx(HKEY_CURRENT_USER,
				CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName,
				0, KEY_READ, &subKey);
	if (result == ERROR_SUCCESS) {
		result = RegQueryValueEx(subKey,"style", NULL, &type, (unsigned char *)&style, &cbData);
		if (result == ERROR_SUCCESS) {
			//AfxMessageBox(CString ("Successfully got mydata."));
		} else {
			//AfxMessageBox(CString ("Filed to query value ."));
		}
	} else {
		//AfxMessageBox(CString ("Filed to open ")+CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName);
	}
	RegCloseKey(subKey);   
	return (result==ERROR_SUCCESS);
}

void MyData::DeleteMyData(CString instanceName) {
	HKEY subKey;
	LONG result;
	DWORD cbData=0;
	result = RegOpenKeyEx(HKEY_CURRENT_USER,
				CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName,
				0, KEY_READ, &subKey);
	if (result == ERROR_SUCCESS) {
		result = RegDeleteValue(subKey, "style");
		RegCloseKey(subKey);
		result = RegDeleteKey(HKEY_CURRENT_USER, CString( PLUGGEDIN_REGKEY )+"\\"+theApp.internalPluginName+"\\"+instanceName);
	}
   
}
