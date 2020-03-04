// FileSystemDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include ".\include\TBCommonFunction.h"
#include ".\include\TBFat32DiskParse.h"
#include ".\include\TBNTFSDataStruct.h"

// CFileSystemDlg 对话框
class CFileSystemDlg : public CDialog
{
// 构造
public:
	CFileSystemDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILESYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeDirector(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

private:
	void InitFileList();
    void InitDiskDrivers();
	BOOL FileSystemType(CString strDrive, BOOL &bFat32, BOOL &bNtfs);
	void ParserChildDir( HTREEITEM hItem); 
	HTREEITEM GetTreeItemFromDirInfo(TBDirInfo *pCurDir); 
	void GetFileSizeAndType(TBDirInfo *pDir, CString &strSize, CString &strType);
	void InsertData(HTREEITEM hItem, TBDirInfo *pParentDir, BOOL bInsertToTree);
	void InsertListItem(TBDirInfo *pDir);
	void InsertTreeItem(HTREEITEM hInsertItem, TBDirInfo *pDirInfo);
	void ReleaseDiskMap();
	void UpdateInfo();

protected:
    DiskListMap m_mapDiskOperate;

private:
	CTreeCtrl m_DirTree;
	CListCtrl m_FileList;
	HTREEITEM m_hRootItem; // 树的根节点的句柄 
	HTREEITEM m_hCurItem;  
};
