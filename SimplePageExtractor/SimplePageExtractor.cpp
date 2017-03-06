// SimplePageExtractor.cpp : DLL �p�̏����������̒�`���s���܂��B
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
//	����!
//
//		���� DLL �� MFC DLL �ɑ΂��ē��I�Ƀ����N�����ꍇ�A
//		MFC ���ŌĂяo����邱�� DLL ����G�N�X�|�[�g���ꂽ
//		�ǂ̊֐����֐��̍ŏ��ɒǉ������ AFX_MANAGE_STATE 
//		�}�N�����܂�ł��Ȃ���΂Ȃ�܂���B
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �ʏ�֐��̖{�̂͂��̈ʒu�ɂ���܂�
//		}
//
//		���̃}�N�����e�֐��Ɋ܂܂�Ă��邱�ƁAMFC ����
//		�ǂ̌Ăяo�����D�悷�邱�Ƃ͔��ɏd�v�ł��B
//		����͊֐����̍ŏ��̃X�e�[�g�����g�łȂ���΂�
//		��Ȃ����Ƃ��Ӗ����܂��A�R���X�g���N�^�� MFC 
//		DLL ���ւ̌Ăяo�����s���\��������̂ŁA�I�u
//		�W�F�N�g�ϐ��̐錾�����O�łȂ���΂Ȃ�܂���B
//
//		�ڍׂɂ��Ă� MFC �e�N�j�J�� �m�[�g 33 �����
//		58 ���Q�Ƃ��Ă��������B
//

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp

BEGIN_MESSAGE_MAP(CSimplePageExtractorApp, CWinApp)
	//{{AFX_MSG_MAP(CSimplePageExtractorApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp �̍\�z

CSimplePageExtractorApp::CSimplePageExtractorApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance �̒��̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CSimplePageExtractorApp �I�u�W�F�N�g

CSimplePageExtractorApp theApp;


/* �ȍ~���v���O�C����SPI�̎����ł� */


CString CSimplePageExtractorApp::pluginName = CString("�ȈՃy�[�W���W");

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
		AfxMessageBox("DocuWorks���C���X�g�[������Ă��܂���B");
		break;
	case XDW_E_FILE_NOT_FOUND:
		AfxMessageBox("�w�肳�ꂽ�t�@�C����������܂���B");
		break;
	case XDW_E_FILE_EXISTS:
		AfxMessageBox("�w�肳�ꂽ�t�@�C���͂��łɑ��݂��܂��B");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		AfxMessageBox("�w�肳�ꂽ�t�@�C�����J�����Ƃ��ł��܂���B");
		break;
	case XDW_E_BAD_FORMAT:
		AfxMessageBox("�w�肳�ꂽ�t�@�C���͐������t�H�[�}�b�g�ł͂���܂���B");
		break;
	case XDW_E_INVALID_ACCESS:
		AfxMessageBox("�w�肳�ꂽ��������錠��������܂���B");
		break;
	default:
		AfxMessageBox("�G���[���������܂����B");
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
		bool done = false;// �ŏ��̃��[�v�͂��Ȃ炸���s�����邽�߁B
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
	CString fascicleName = CString(folder)+"\\"+filename+"-���o.xdw";
	char * tmpFileName = TmpFileName(folder);

	XDW_DOCUMENT_HANDLE h = NULL;
	XDW_OPEN_MODE_EX mode = {
		sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_READONLY, XDW_AUTH_NODIALOGUE };
	api_result = XDW_OpenDocumentHandle(path, &h, (XDW_OPEN_MODE*)&mode);
	if (api_result < 0) {
		print_error(api_result);
		AfxMessageBox("Unexpected error: �I�����ꂽ�������J���܂���ł����B");
		return false;
	}

	// XDW_GetDocumentInformation��p���đ��y�[�W���𓾂�
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
					AfxMessageBox(CString("Unexpected error: �tⳂ̕t�����y�[�W���A")+fascicleName+"�Ƃ������O�Ŏ��o���̂Ɏ��s.");
					goto exit_all;
				}
				XDW_OPEN_MODE_EX updateMode = {
					sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_UPDATE, XDW_AUTH_NODIALOGUE };
					api_result = XDW_OpenDocumentHandle((LPCSTR)(fascicleName), &fascicle, (XDW_OPEN_MODE*)&updateMode);
					if (api_result < 0 ) {
						print_error(api_result);
						AfxMessageBox("Unexpected error: �tⳂ̕t�����y�[�W�����o�����t�@�C�����J�����ƂɎ��s�B");
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
			AfxMessageBox("Unexpected error: �tⳂ̍폜�Ɏ��s");
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
    /* ���̃R�}���h���s�̓r���A�����Ƃ��ēn�����DW�t�@�C���͊J������Ă��܂��B
       �ǂݏ����\�ŃA�N�Z�X�ł��܂��B
    */
    /* MFC���g�p����ꍇ�AAFX_MANAGE_STATE( AfxGetStaticModuleState() );��
       �K���R�[�����Ă��������B����ɂ��A����DLL�̃��\�[�X���g����悤�ɂȂ�܂��B
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
    /* *pnRealSize�ɕK�v�ȃo�b�t�@�T�C�Y�����Ă������� */
    *pnRealSize=strlen((LPCSTR)(theApp.internalPluginName))+strlen((LPCSTR)(theApp.pluginName))+1;

    if (nBufSize<*pnRealSize)
        return 0;

    /* nBufSize���K�v�ȃo�b�t�@�T�C�Y�𒴂��Ă���ꍇ�̂݁ApszEntries�ɃR�s�[��
       �����������Ƃ����� return 1��Ԃ��Ă�������
    */

    /*
     0�^�[�~�l�[�g�̕�����̃��X�g��Ԃ��Ă��������BDWDesk�̃v���O�C��/�ݒ�̌��ɏo�܂�
    */
   // DoCopy Commands!
    ::strcpy(pszEntries, (LPCSTR)(theApp.pluginName));
	*pnRealSize=strlen(pszEntries);
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Executable(const char *pszFunction, long nSel)
{
    //�I���t�@�C���̐� nSel�ɉ����Ď��s�\����Ԃ��Ă�������
	if (nSel>0) {
		return 1;
	} else {
		return 0;
	}
}
PLUGGEDIN_EXPORT( long ) PLG_RequireFiles(const char *pszFunction)
{
    // 0��Ԃ��ƁA�t�@�C���I����Ԃɂ�炸�����͓n����܂���
	return 1;
}

PLUGGEDIN_EXPORT( HICON ) PLG_GetCommandIcon(const char *pszFunction, long bNormal)
{
    /* ��ɑ傫��(32x32 or 24x24)�A�C�R���ł����܂��܂��� */

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
    /* �K�� 1��Ԃ��Ă�������. 0��Ԃ��ƃv���O�C�������[�h����܂���*/
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
    IsCloningCommand/CanSetProfile�� 0�ȊO��Ԃ��ƁA
    GetNewClone / SetProfile���R�[������܂�

*/

PLUGGEDIN_EXPORT( long ) PLG_IsParallel(const char *pszFunction)
{
    /* v2.0�ł͂���SPI�͖�������܂� */
	return 0;
}


PLUGGEDIN_EXPORT( long ) PLG_CanSetProfile(const char *pszFunction)
{
    /* SetProfile��L���ɂ���ꍇ�A������1��Ԃ��Ă������� */
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_SetProfile(const char *pszFunction)
{
    /* ������GUI���o���Ď��g�̐ݒ���s���Ă������� */
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
    /* ����@�\�ŕ����̐ݒ��ۑ����ē��삳�������ꍇ�Ȃǂ́A����
       SPI�� 1 ��Ԃ��Ă�������
     */
    return 1;
}

/* GetNewClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_GetNewClone(char* pszNewClone, long bufsize)
{
    /* pszNewClone�Ƃ͈قȂ�����������쐬���ApszNewClone�����������ĕԂ��Ă������� */
    /* �Ⴆ�� "�v���O�C���@�\" -> "�v���O�C���@�\01","�v���O�C���@�\02"�Ȃ�
       �ȍ~�̃Z�b�V�����ł́A�N���[�����ꂽ�����񂪈���pszCommand�Ƃ��ēn����܂�
       �N���[�����Ƃɐݒ��ۑ����Ă��������B
       PLUGGEDIN_REGKEY\ �ȉ��̃��W�X�g���ɕۑ����ꂽ�f�[�^�́A
       Desk�̃v���O�C��/�ݒ�_�C�A���O�� OK ��������Ȃ�����A�ꊇ�L�����Z������܂�
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
    /* pszClone�Ŏw�肳�ꂽ�R�}���h���g�p���Ă����̈���J�����Ă������� */
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
