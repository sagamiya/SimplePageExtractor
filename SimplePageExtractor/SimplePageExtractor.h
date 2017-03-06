// SimplePageExtractor.h : SIMPLEPAGEEXTRACTOR アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_)
#define AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp
// このクラスの動作の定義に関しては SimplePageExtractor.cpp ファイルを参照してください。
//

class CSimplePageExtractorApp : public CWinApp
{
public:
	CSimplePageExtractorApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CSimplePageExtractorApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSimplePageExtractorApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static CString pluginName;
	static CString internalPluginName;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_)
