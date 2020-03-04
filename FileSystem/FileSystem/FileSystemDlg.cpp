// FileSystemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NTFS_OFFSET  0x03
#define FAT32_OFFSET 0x52

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CFileSystemDlg 对话框

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


// CFileSystemDlg 消息处理程序

BOOL CFileSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitFileList();
	InitDiskDrivers();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileSystemDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	ReleaseDiskMap();
}

/***********************************************************************
* 函数名称：
* ReleaseDiskMap()
*
*函数参数：
*  无
*
*说明：释放磁盘管理映射数组
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
	m_FileList.InsertColumn( 0, _T("名称"), LVCFMT_LEFT, 280);
	m_FileList.InsertColumn( 1, _T("类型"), LVCFMT_LEFT, 135 );
	m_FileList.InsertColumn( 2, _T("大小"),     LVCFMT_LEFT, 100 );
	m_FileList.InsertColumn( 3, _T("创建日期"), LVCFMT_LEFT, 135 );
}

BOOL CFileSystemDlg::FileSystemType(CString strDrive, BOOL &bFat32, BOOL &bNtfs)
{
	BOOL bRet = TRUE;
	CString strDisk;
	strDisk.Format(_T("\\\\.\\%s"), strDrive.Left(2));

#ifdef _UNICODE
	WCHAR szDiscFile[7] = { 0 };
	// 将T字符转换成宽字符
	CStringW strWide = CT2CW(strDisk);
	wcscpy_s(szDiscFile, strWide);
#else
	LPCSTR szDiscFile = strDisk;
#endif

	//初始化得到磁盘句柄
	byte sectorbuf[SECTOR_BUF_LEN] = { 0 };

	HANDLE hDisk = NULL;
	hDisk = CreateFile(szDiscFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hDisk)
	{
		// 中间两个参数分别是偏移字节数的低32Bit数值，和高32Bit的地址, 高32Bit不用则为零; 表示D:\从首扇区起，偏移字节数为0
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
		//第0个扇区数据 buf 偏移0x03开始的8个Bytes若是"NTFS    "，表示这个扇区就是NTFS的引导记录
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
			//第0个扇区数据 buf 偏移0x52开始的8个Bytes若是"FAT32    "，表示这个扇区就是FAT32文件系统
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
* 函数名称：
* InsertTreeItem()
*
*函数参数：
*  HTREEITEM hInsertItem -树控件Item
*  TBDirInfo *pDirInfo  -根目录项
*
*说明：根据传入根目录项参数遍历插入树目录结构
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
	m_hRootItem = m_DirTree.InsertItem(_T("计算机"), TVI_ROOT, TVI_LAST); 
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
			//声明标识符
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

				//初始化判断根目录
				DiskListMap::iterator it = m_mapDiskOperate.find( strFindDisk );
				if ( it == m_mapDiskOperate.end() )
				{
					CTBFat32DiskParse *pDiskOptr = new CTBFat32DiskParse();
					pDiskOptr->InitDiskHandle(strFindDisk);
					pDiskOptr->ParserRootDir();

				    //插入子目录
					TBDirInfo *pRootDir = TBCommonFun::GetRootDir(pDiskOptr->m_pCurPartition);
					InsertTreeItem(hInsertItem, pRootDir);
					m_mapDiskOperate[strFindDisk] = pDiskOptr;
				}
			}
			else if (bNtfs)
			{
				//NTFS待实现
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
	// TODO: 在此添加控件通知处理程序代码
	ParserChildDir(pNMTreeView->itemNew.hItem);
	*pResult = 0;
}

/***********************************************************************
* 函数名称：
* ParserChildDir()
*
*函数参数：
*  HTREEITEM hItem; 选择树节参数
*
*说明：根据选择树节点，解析子文件目录项
***********************************************************************/
void CFileSystemDlg::ParserChildDir( HTREEITEM hItem)
{
	HTREEITEM hCurSelectItem = hItem;
	TBDirInfo *pSelectDir = (TBDirInfo*)m_DirTree.GetItemData(hCurSelectItem);

	//若为“计算机节点”或者文件节点则不做进一步处理
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
			if (0 == pSelectDir->strName.CompareNoCase(_T("//")) || pSelectDir->bCheckedChildDir) //根目录或者改子目录项已解析过
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
			//插入目录树
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
	//添加到文件列表
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
		strType = _T("文件夹");
	}
	else
	{
		strSize.Format(_T("%d KB"), (0 == pDir->dwSize % 1024) ? (pDir->dwSize / 1024) : ((pDir->dwSize / 1024) + 1));
		strType.Format(_T("%s文件"), pDir->strName.Mid(pDir->strName.ReverseFind(_T('.')) + 1));
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int nIndex = pNMItemActivate->iItem;
	TBDirInfo *pCurSelectDir =(TBDirInfo *)m_FileList.GetItemData(nIndex);
	
	//在列表双击文件则不做任何操作，双击文件夹则进入子目录项
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
			if (pCurSelectDir->bCheckedChildDir) //根目录或者改子目录项已解析过
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
	strInfo.Format(_T("该目录下总共%d项文件或文件夹"), nCount);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strInfo);
}