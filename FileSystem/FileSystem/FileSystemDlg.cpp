// FileSystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NTFS_OFFSET  0x03
#define FAT32_OFFSET 0x52

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFileSystemDlg �Ի���

CFileSystemDlg::CFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hRootItem = NULL;
	m_hCurItem = NULL;
	m_mapDiskOperate.clear();
}

void CFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DIRECTOR, m_DirTree);
	DDX_Control(pDX, IDC_LIST_FILE, m_FileList);
}

BEGIN_MESSAGE_MAP(CFileSystemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileSystemDlg::OnNMDblclkListFile)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTOR, &CFileSystemDlg::OnTvnSelchangedTreeDirector)
END_MESSAGE_MAP()


// CFileSystemDlg ��Ϣ�������

BOOL CFileSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	InitFileList();
	InitDiskDrivers();
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileSystemDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	ReleaseDiskMap();
}

/***********************************************************************
* �������ƣ�
* ReleaseDiskMap()
*
*����������
*  ��
*
*˵�����ͷŴ��̹���ӳ������
***********************************************************************/
void CFileSystemDlg::ReleaseDiskMap()
{
	DiskListMap::iterator iter = m_mapDiskOperate.begin();
	while (iter != m_mapDiskOperate.end())
	{
		CTBFat32DiskParse *pDisk = (*iter).second;
		if (NULL != pDisk)
		{
			TBSAFEDELETEPTR(pDisk);
		}

		iter++;
	}
	m_mapDiskOperate.clear();
}

void CFileSystemDlg::InitFileList()
{
	m_FileList.InsertColumn( 0, _T("����"), LVCFMT_LEFT, 280);
	m_FileList.InsertColumn( 1, _T("����"), LVCFMT_LEFT, 135 );
	m_FileList.InsertColumn( 2, _T("��С"),     LVCFMT_LEFT, 100 );
	m_FileList.InsertColumn( 3, _T("��������"), LVCFMT_LEFT, 135 );
}

BOOL CFileSystemDlg::FileSystemType(CString strDrive, BOOL &bFat32, BOOL &bNtfs)
{
	BOOL bRet = TRUE;
	CString strDisk;
	strDisk.Format(_T("\\\\.\\%s"), strDrive.Left(2));

#ifdef _UNICODE
	WCHAR szDiscFile[7] = { 0 };
	// ��T�ַ�ת���ɿ��ַ�
	CStringW strWide = CT2CW(strDisk);
	wcscpy_s(szDiscFile, strWide);
#else
	LPCSTR szDiscFile = strDisk;
#endif

	//��ʼ���õ����̾��
	byte sectorbuf[SECTOR_BUF_LEN] = { 0 };

	HANDLE hDisk = NULL;
	hDisk = CreateFile(szDiscFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hDisk)
	{
		// �м����������ֱ���ƫ���ֽ����ĵ�32Bit��ֵ���͸�32Bit�ĵ�ַ, ��32Bit������Ϊ��; ��ʾD:\����������ƫ���ֽ���Ϊ0
		SetFilePointer(hDisk, 0, NULL, FILE_BEGIN);
		DWORD dwReadLength;
		ReadFile(hDisk, sectorbuf, SECTOR_BUF_LEN, &dwReadLength, NULL);
		if (dwReadLength < (DWORD)SECTOR_BUF_LEN)
		{
			bRet = FALSE;
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet)
	{
		//��0���������� buf ƫ��0x03��ʼ��8��Bytes����"NTFS    "����ʾ�����������NTFS��������¼
		byte byTmpFileType[9] = { 0 };
		memset(byTmpFileType, 0, 9);
		memcpy(byTmpFileType, sectorbuf + NTFS_OFFSET, 8);
		CString strTmpFileName = CString(byTmpFileType);
		strTmpFileName.TrimRight();
		strTmpFileName.TrimLeft();
		strTmpFileName.MakeUpper();
		if (0 == strTmpFileName.CompareNoCase(_T("NTFS")))
		{
			bNtfs = TRUE;
		}

		if (!bNtfs)
		{
			//��0���������� buf ƫ��0x52��ʼ��8��Bytes����"FAT32    "����ʾ�����������FAT32�ļ�ϵͳ
			byte byTmpFileType[9] = { 0 };
			memset(byTmpFileType, 0, 9);
			memcpy(byTmpFileType, sectorbuf + FAT32_OFFSET, 8);
			CString strTmpFileName = CString(byTmpFileType);
			strTmpFileName.TrimRight();
			strTmpFileName.TrimLeft();
			if (0 == strTmpFileName.CompareNoCase(_T("FAT32")))
			{
				bFat32 = TRUE;
			}
		}
	}

	return bRet;
}

/***********************************************************************
* �������ƣ�
* InsertTreeItem()
*
*����������
*  HTREEITEM hInsertItem -���ؼ�Item
*  TBDirInfo *pDirInfo  -��Ŀ¼��
*
*˵�������ݴ����Ŀ¼���������������Ŀ¼�ṹ
***********************************************************************/
void CFileSystemDlg::InsertTreeItem(HTREEITEM hInsertItem, TBDirInfo *pDirInfo)
{
	if (pDirInfo)
	{
		m_DirTree.SetItemData(hInsertItem, (DWORD_PTR)pDirInfo);

		TBDirInfo *pTmpChild = TBCommonFun::GetChildDir(pDirInfo);
		while (pTmpChild)
		{
			if (TBCommonFun::IsShowDir(pTmpChild) && pTmpChild->bDir)
			{
				HTREEITEM hChild = m_DirTree.InsertItem(pTmpChild->strName, hInsertItem, TVI_LAST);
				m_DirTree.SetItemData(hChild, (DWORD_PTR)pTmpChild);
			}

			TBDirInfo *pTmpNextChild = TBCommonFun::GetNextChildDir(pTmpChild);
			pTmpChild = pTmpNextChild;
		}
	}
	else
	{
		m_DirTree.SetItemData(hInsertItem, (DWORD_PTR)NULL);
	}
}

void CFileSystemDlg::InitDiskDrivers()
{ 
	m_hRootItem = m_DirTree.InsertItem(_T("�����"), TVI_ROOT, TVI_LAST); 
	m_DirTree.SetItemData(m_hRootItem, (DWORD_PTR)NULL);

	TCHAR szBuf[100];
	memset(szBuf, 0, 100);
	GetLogicalDriveStrings(sizeof(szBuf)/sizeof(TCHAR), szBuf);

	for (TCHAR* str = szBuf; *str; str += _tcslen(str) + 1)
	{
		LPCTSTR sDrivePath = str;
		UINT uDriveType = GetDriveType(sDrivePath);
		if ( DRIVE_REMOVABLE == uDriveType || DRIVE_FIXED == uDriveType || DRIVE_REMOTE == uDriveType || DRIVE_RAMDISK == uDriveType )
		{
			//������ʶ��
			USES_CONVERSION;
#ifdef _UNICODE
			char * szFileName = T2A(str);
#else
			char * szFileName = str;
#endif
			CString strDrive(szFileName);
			BOOL bFat32 = FALSE;
			BOOL bNtfs = FALSE;
			FileSystemType(strDrive, bFat32, bNtfs);
			if (bFat32)
			{
				CString strFindDisk = _T("");
				strFindDisk.Format(_T("%s(FAT32)"), sDrivePath);
				HTREEITEM hInsertItem = m_DirTree.InsertItem(strFindDisk, m_hRootItem, TVI_LAST);

				//��ʼ���жϸ�Ŀ¼
				DiskListMap::iterator it = m_mapDiskOperate.find( strFindDisk );
				if ( it == m_mapDiskOperate.end() )
				{
					CTBFat32DiskParse *pDiskOptr = new CTBFat32DiskParse();
					pDiskOptr->InitDiskHandle(strFindDisk);
					pDiskOptr->ParserRootDir();

				    //������Ŀ¼
					TBDirInfo *pRootDir = TBCommonFun::GetRootDir(pDiskOptr->m_pCurPartition);
					InsertTreeItem(hInsertItem, pRootDir);
					m_mapDiskOperate[strFindDisk] = pDiskOptr;
				}
			}
			else if (bNtfs)
			{
				//NTFS��ʵ��
				CString strFindDisk = _T("");
				strFindDisk.Format(_T("%s(NTFS)"), sDrivePath);
				HTREEITEM hInsertItem = m_DirTree.InsertItem(strFindDisk, m_hRootItem, TVI_LAST);
				m_DirTree.SetItemData(hInsertItem, (DWORD_PTR)NULL);
			}
			else
			{
				CString strFindDisk = _T("");
				strFindDisk.Format(_T("%s(Unknown)"), sDrivePath);
				HTREEITEM hInsertItem = m_DirTree.InsertItem(strFindDisk, m_hRootItem, TVI_LAST);
				m_DirTree.SetItemData(hInsertItem, (DWORD_PTR)NULL);
			}
		}
	}
}

void CFileSystemDlg::OnTvnSelchangedTreeDirector(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ParserChildDir(pNMTreeView->itemNew.hItem);
	*pResult = 0;
}

/***********************************************************************
* �������ƣ�
* ParserChildDir()
*
*����������
*  HTREEITEM hItem; ѡ�����ڲ���
*
*˵��������ѡ�����ڵ㣬�������ļ�Ŀ¼��
***********************************************************************/
void CFileSystemDlg::ParserChildDir( HTREEITEM hItem)
{
	HTREEITEM hCurSelectItem = hItem;
	TBDirInfo *pSelectDir = (TBDirInfo*)m_DirTree.GetItemData(hCurSelectItem);

	//��Ϊ��������ڵ㡱�����ļ��ڵ�������һ������
	if (hCurSelectItem == m_DirTree.GetRootItem() || NULL == hCurSelectItem || NULL == pSelectDir || (pSelectDir && !pSelectDir->bDir))
	{
		return;
	}

	m_FileList.SetRedraw(FALSE);
	m_FileList.DeleteAllItems();
	TBDirInfo *pDiskRootDir = NULL;
	BOOL bNeedInsert = FALSE;

	DiskListMap::iterator iter = m_mapDiskOperate.find( pSelectDir->strDisk );
	if ( iter != m_mapDiskOperate.end() )
	{
		CTBFat32DiskParse *pDiskOper = (*iter).second;
		if (pDiskOper)
		{
			if (0 == pSelectDir->strName.CompareNoCase(_T("//")) || pSelectDir->bCheckedChildDir) //��Ŀ¼���߸���Ŀ¼���ѽ�����
			{
				pDiskRootDir = pSelectDir;
			}
			else
			{
				pDiskOper->GetCurDirChildData(pSelectDir);
				pSelectDir->bCheckedChildDir = TRUE;

				bNeedInsert = TRUE;
				m_DirTree.SetItemData(hCurSelectItem, (DWORD_PTR)pSelectDir);

				pDiskRootDir = pSelectDir;
			}

			m_hCurItem = hCurSelectItem;

			InsertData(hCurSelectItem, pDiskRootDir, bNeedInsert);
		}
	}
	m_FileList.SetRedraw(TRUE);
	UpdateInfo();
}

void CFileSystemDlg::InsertData(HTREEITEM hItem, TBDirInfo *pParentDir, BOOL bInsertToTree)
{
	if (!pParentDir)
	{
		return;
	}

	TBDirInfo *pTmpChild = TBCommonFun::GetChildDir(pParentDir);
	while (pTmpChild)
	{
		if (TBCommonFun::IsShowDir(pTmpChild))
		{
			//����Ŀ¼��
			if (hItem && pTmpChild->bDir && 0 != pParentDir->strName.CompareNoCase(_T("//")) && bInsertToTree)
			{
				HTREEITEM hChild = m_DirTree.InsertItem(pTmpChild->strName, hItem, TVI_LAST);
				m_DirTree.SetItemData(hChild, (DWORD_PTR)pTmpChild);
			}

			InsertListItem(pTmpChild);
		}

		//NEXT
		TBDirInfo *pTmpNextChild = TBCommonFun::GetNextChildDir(pTmpChild);
		pTmpChild = pTmpNextChild;
	}
}

void CFileSystemDlg::InsertListItem(TBDirInfo *pDir)
{
	//��ӵ��ļ��б�
	if (pDir)
	{
		int nIndex = m_FileList.InsertItem(m_FileList.GetItemCount(), pDir->strName);
		m_FileList.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(pDir));
		m_FileList.SetItemText(nIndex, 0, pDir->strName);

		CString strSize = _T(""), strType = _T("");
		GetFileSizeAndType(pDir, strSize, strType);

		m_FileList.SetItemText(nIndex, 1, strType);
		m_FileList.SetItemText(nIndex, 2, strSize);
		m_FileList.SetItemText(nIndex, 3, pDir->strDateTime);
	}
}

void CFileSystemDlg::GetFileSizeAndType(TBDirInfo *pDir, CString &strSize, CString &strType)
{
	if (!pDir)
	{
		return;
	}

	if (pDir->bDir)
	{
		strSize = _T("");
		strType = _T("�ļ���");
	}
	else
	{
		strSize.Format(_T("%d KB"), (0 == pDir->dwSize % 1024) ? (pDir->dwSize / 1024) : ((pDir->dwSize / 1024) + 1));
		strType.Format(_T("%s�ļ�"), pDir->strName.Mid(pDir->strName.ReverseFind(_T('.')) + 1));
	}
}

HTREEITEM CFileSystemDlg::GetTreeItemFromDirInfo(TBDirInfo *pCurDir)
{
	HTREEITEM hItem = NULL;
	if (m_hCurItem && m_DirTree.ItemHasChildren(m_hCurItem))
	{
		HTREEITEM hChild = m_DirTree.GetChildItem( m_hCurItem );
		while ( hChild ) 
		{ 
			TBDirInfo *pChildDir =(TBDirInfo *)m_DirTree.GetItemData(hChild);
			if (pChildDir == pCurDir)
			{
				hItem = hChild;
				break;
			}
			hChild = m_DirTree.GetNextItem( hChild, TVGN_NEXT ); 
		} 
	} 

	return hItem;
}

void CFileSystemDlg::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	int nIndex = pNMItemActivate->iItem;
	TBDirInfo *pCurSelectDir =(TBDirInfo *)m_FileList.GetItemData(nIndex);
	
	//���б�˫���ļ������κβ�����˫���ļ����������Ŀ¼��
	if (NULL == pCurSelectDir || !pCurSelectDir->bDir ) return;

	m_FileList.SetRedraw(FALSE);
	m_FileList.DeleteAllItems();
	TBDirInfo *pDiskRootDir = NULL;
	BOOL bNeedInsert = FALSE;

	DiskListMap::iterator iter = m_mapDiskOperate.find( pCurSelectDir->strDisk );
	if ( iter != m_mapDiskOperate.end() )
	{
		CTBFat32DiskParse *pDiskOper = (*iter).second;
		if (pDiskOper)
		{
			if (pCurSelectDir->bCheckedChildDir) //��Ŀ¼���߸���Ŀ¼���ѽ�����
			{
				pDiskRootDir = pCurSelectDir;
			}
			else
			{
				pDiskOper->GetCurDirChildData(pCurSelectDir);
				pCurSelectDir->bCheckedChildDir = TRUE;
				bNeedInsert = TRUE;

				pDiskRootDir = pCurSelectDir;
			}

			HTREEITEM hItem = NULL;
			if (bNeedInsert)
			{
				hItem = GetTreeItemFromDirInfo(pDiskRootDir);
				if (hItem)
				{
					m_DirTree.SetItemData(hItem, (DWORD_PTR)pDiskRootDir);
				}
			}

			InsertData(hItem, pDiskRootDir, bNeedInsert); 
		}
	}
	m_FileList.SetRedraw(TRUE); 
	m_FileList.Invalidate(TRUE);
	UpdateInfo();
}

void CFileSystemDlg::UpdateInfo()
{
	int nCount = m_FileList.GetItemCount();
	CString strInfo = _T("");
	strInfo.Format(_T("��Ŀ¼���ܹ�%d���ļ����ļ���"), nCount);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strInfo);
}