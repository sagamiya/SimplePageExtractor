// AboutDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "SimplePageExtractor.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AboutDlg ダイアログ


AboutDlg::AboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AboutDlg)
	m_withFusen = FALSE;
	//}}AFX_DATA_INIT
}


void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutDlg)
	DDX_Check(pDX, IDC_CHECK1, m_withFusen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
	//{{AFX_MSG_MAP(AboutDlg)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutDlg メッセージ ハンドラ
