// SimplePageExtractor.h : SIMPLEPAGEEXTRACTOR �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_)
#define AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CSimplePageExtractorApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� SimplePageExtractor.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CSimplePageExtractorApp : public CWinApp
{
public:
	CSimplePageExtractorApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CSimplePageExtractorApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSimplePageExtractorApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static CString pluginName;
	static CString internalPluginName;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SIMPLEPAGEEXTRACTOR_H__005A3F53_5F83_41C2_A996_FA5255C4C224__INCLUDED_)
