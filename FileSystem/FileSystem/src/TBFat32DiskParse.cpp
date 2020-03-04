/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
* All rights reserved.
*
* @file   CTBFat32DiskParse.cpp
* @author qiaozhousan
* @date   2017-05-10
*
* @brief  Parsing FAT32 file systems
*
*/
#include "StdAfx.h"
#include "TBFat32DiskParse.h"


CTBFat32DiskParse::CTBFat32DiskParse()
{
	m_strDisk = _T("");
	m_pPTHead = NULL;
	m_pCurPartition = NULL;
	m_pCurDir = NULL;
	m_pClusterBuf = NULL;
	m_HDevice = INVALID_HANDLE_VALUE;

	m_byLongFileName[12][26] = { 0 };
	m_nRowGet = 0;
	m_nRowReal = 0;
}

CTBFat32DiskParse::~CTBFat32DiskParse()
{
	ReleaseClusterBuf();
	ReleasePartitionTable();
	if(m_HDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_HDevice);
		m_HDevice = INVALID_HANDLE_VALUE;
	}
}

byte* CTBFat32DiskParse::MallocClusterBuf()
{
	if(!m_pCurPartition && !m_pCurPartition->pPartitionInfo)
		return NULL;

	if(!m_pClusterBuf)
	{
		m_pClusterBuf = new byte[m_pCurPartition->pPartitionInfo->dwByteNumPerCluster];
		memset(m_pClusterBuf, VALUE_ZERO, m_pCurPartition->pPartitionInfo->dwByteNumPerCluster * sizeof(byte));
	}
	return m_pClusterBuf;
}

void CTBFat32DiskParse::ReleaseClusterBuf()
{
	TBSAFEDELETEARRAY(m_pClusterBuf);
}

void CTBFat32DiskParse::ReleaseDir(TBDirInfo *pDirInfo)
{
	if(!pDirInfo)
		return;

	if(pDirInfo->pChildDir)
		ReleaseDir(pDirInfo->pChildDir);

	TBDirInfo *pTmp = pDirInfo;
	pDirInfo = pDirInfo->pBrotherDir;
	TBSAFEDELETEPTR(pTmp);
	ReleaseDir(pDirInfo);
}

void CTBFat32DiskParse::ReleasePartitionTable()
{
	TBPartitionTable *pPT = NULL;
	while(m_pPTHead)
	{
		pPT = m_pPTHead;
		TBPartitionInfo *pPTInfo = pPT->pPartitionInfo;
		TBDirInfo *pDirInfo = NULL;
		if(pPTInfo)
		{
			//先释放子目录项内存
			pDirInfo = pPTInfo->pDirInfo;
			ReleaseDir(pDirInfo);
			pDirInfo = NULL;
			TBSAFEDELETEPTR(pPTInfo);
		}
		m_pPTHead = m_pPTHead->pNextPT;
		TBSAFEDELETEPTR(pPT);
	}

	m_pCurDir = NULL;
	m_pCurPartition = NULL;
	m_pPTHead = NULL;
}

__int64 CTBFat32DiskParse::FileSeek(HANDLE hFile, __int64 distance, DWORD dwMoveMethod)
{   
	LARGE_INTEGER li;   
	li.QuadPart = distance;
	li.LowPart = SetFilePointer(hFile, li.LowPart, &li.HighPart, dwMoveMethod);   
	if (FILE_LOW_END == li.LowPart && GetLastError() != NO_ERROR)
	{
		li.QuadPart = -1;   
	}   

	return li.QuadPart;   
}

BOOL CTBFat32DiskParse::ReadSectors(HANDLE hFile, __int64 logicalLba, short nSectors, byte *buff)   
{
	BOOL bRet = TRUE;
	if (INVALID_HANDLE_VALUE == hFile || !buff)
	{
		bRet = FALSE;
	}
	else
	{
		UINT nByteNumPerSector = 512;
		if (m_pCurPartition && m_pCurPartition->pPartitionInfo)
		{
			nByteNumPerSector = m_pCurPartition->pPartitionInfo->dwByteNumPerSector;
		}
			
		__int64 nDistance = logicalLba * nByteNumPerSector;
		FileSeek(hFile, nDistance, FILE_BEGIN);

		DWORD dwNumOfBytesRead = 0;
		ReadFile(hFile, buff, (DWORD)(nSectors * nByteNumPerSector), &dwNumOfBytesRead, NULL);
		if (dwNumOfBytesRead < (DWORD)(nSectors * nByteNumPerSector))
		{
			bRet = FALSE;
		}
	}
		
	return bRet;
}

BOOL CTBFat32DiskParse::InitDiskHandle( CString strDiskName )
{
	BOOL bRet = FALSE;
	m_strDisk = strDiskName;
	CString strDisk = _T("");
	strDisk.Format( _T("\\\\.\\%s"), strDiskName.Left(2));

	USES_CONVERSION;
#ifdef _UNICODE
	WCHAR szDiscFile[7] = { 0 };
	// 将T字符转换成宽字符
	CStringW strWide = CT2CW(strDisk); 
	wcscpy_s(szDiscFile, strWide);
#else
	LPCSTR szDiscFile = strDisk;
#endif

	//初始化得到磁盘句柄
	HANDLE hDisk = NULL;
	hDisk = CreateFile(szDiscFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( INVALID_HANDLE_VALUE != hDisk )
	{
		m_HDevice = hDisk;
		bRet = TRUE;
	}

	return bRet;
}

BOOL CTBFat32DiskParse::SetPartitionTable(DWORD dwStartSector)
{
	BOOL bRet = TRUE;
	TBPartitionTable *pPT = new TBPartitionTable;
	if (pPT)
	{
		pPT->dwStartSector = dwStartSector;
		if (!m_pPTHead)
		{
			m_pPTHead = pPT;
			m_pCurPartition = pPT;
		}
		else
		{
			TBPartitionTable *p = m_pPTHead;
			while (p->pNextPT)
			{
				p = p->pNextPT;
			}
			p->pNextPT = pPT;
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CTBFat32DiskParse::SetPartitionInfo(DWORD dwByteNumPerSector, DWORD dwClusterSectorNum, DWORD dwFATSectorStart, DWORD dwFATSectorNum, DWORD dwDirStartClusterNum)
{
	BOOL bRet = TRUE;
	if (m_pPTHead)
	{
		TBPartitionInfo *pPInfo = new TBPartitionInfo;
		pPInfo->dwByteNumPerSector = dwByteNumPerSector;
		pPInfo->dwClusterSectorNum = dwClusterSectorNum;
		pPInfo->dwFatSectorStart = dwFATSectorStart;
		pPInfo->dwFatSectorNum = dwFATSectorNum;
		pPInfo->dwDirStartClusterNum = dwDirStartClusterNum;
		pPInfo->dwDataSectorStart = dwFATSectorStart + dwFATSectorNum * 2;
		pPInfo->dwByteNumPerCluster = dwClusterSectorNum * dwByteNumPerSector;

		TBPartitionTable *pPTable = m_pPTHead;
		while (pPTable->pNextPT)
		{
			pPTable = pPTable->pNextPT;
		}
		pPTable->pPartitionInfo = pPInfo;
	} 
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

void  CTBFat32DiskParse::ParserDirParaFromAttribute(byte nAttribute, TBDirInfo *pDirInfo)
{
	if (pDirInfo)
	{
		pDirInfo->bReadOnly = TBCommonFun::GetReadOnlyAttribute(nAttribute);
		pDirInfo->bHide		= TBCommonFun::GetHideAttribute(nAttribute);
		pDirInfo->bSystem	= TBCommonFun::GetSystemAttribute(nAttribute);
		pDirInfo->bVolumn	= TBCommonFun::GetVolumnAttribute(nAttribute);
		pDirInfo->bDir		= TBCommonFun::GetDirAttribute(nAttribute);
		pDirInfo->bArchived = TBCommonFun::GetArchiveAttribute(nAttribute);
	}
}

BOOL CTBFat32DiskParse::AppendBrotherDir(CString strName, byte nAttribute, CString strDateTime, DWORD dwStartClusterNum, DWORD nDirSize)
{
	BOOL bRet = TRUE;
	if (!m_pCurPartition || !m_pCurDir)
	{
		bRet = FALSE;
	}
	else
	{
		TBDirInfo *pDir = new TBDirInfo;
		pDir->strName	= strName;
		ParserDirParaFromAttribute(nAttribute, pDir);

		pDir->strDateTime		= strDateTime;
		pDir->dwStartClusterNum = dwStartClusterNum;
		pDir->dwSize			= nDirSize;
		pDir->strDisk			= m_strDisk;
		if (!m_pCurPartition->pPartitionInfo->pDirInfo)
		{
			m_pCurPartition->pPartitionInfo->pDirInfo = pDir;
			m_pCurDir = pDir;
		}
		else
		{
			//卷标项
			if ((FALSE == pDir->bDir && FALSE == pDir->bSystem && TRUE == pDir->bVolumn))
			{
				TBSAFEDELETEPTR(pDir);
			}
			else
			{
				pDir->pParentDir = m_pCurDir->pParentDir;
				m_pCurDir->pBrotherDir = pDir;
				m_pCurDir = m_pCurDir->pBrotherDir;
			}
		}
	}

	return bRet;
}

BOOL CTBFat32DiskParse::AppendChildDir(CString strName, byte nAttribute, CString strDateTime, DWORD dwStartClusterNum, DWORD nDirSize)
{
	BOOL bRet = TRUE;
	if (m_pCurPartition)
	{
		TBDirInfo *pDir = new TBDirInfo;
		pDir->strName = strName;
		ParserDirParaFromAttribute(nAttribute, pDir);

		pDir->strDateTime = strDateTime;
		pDir->strDisk = m_strDisk;
		pDir->dwStartClusterNum = dwStartClusterNum;
		pDir->dwSize = nDirSize;

		if (!m_pCurPartition->pPartitionInfo->pDirInfo)
		{
			TBDirInfo *pRootDir = new TBDirInfo;
			pRootDir->strName = _T("//");
			pRootDir->bDir = true;
			pRootDir->bVolumn = true;
			pRootDir->bSystem = true;
			pRootDir->dwStartClusterNum = 0;
			pRootDir->strDisk = m_strDisk;
			pRootDir->bCheckedChildDir = TRUE;  //根目录下文件项初始化默认解析
			m_pCurPartition->pPartitionInfo->pDirInfo = pRootDir;
			m_pCurDir = pRootDir;
		}

		//隐藏受保护的系统文件夹
		if (pDir->bHide && pDir->bSystem && pDir->bDir && !pDir->bVolumn)
		{
			TBDirInfo *pRootDir = TBCommonFun::GetRootDir(m_pCurPartition);
			if (pRootDir)
			{
				TBDirInfo *pTmpChild = TBCommonFun::GetChildDir(pRootDir);
				while (pTmpChild)
				{
					TBDirInfo *pTmpNextChild = TBCommonFun::GetNextChildDir(pTmpChild);
					if (pTmpNextChild)
					{
						pTmpChild = pTmpNextChild;
					}
					else
					{
						m_pCurDir = pTmpChild;
						pDir->pParentDir = m_pCurDir->pParentDir;
						m_pCurDir->pBrotherDir = pDir;
						m_pCurDir = m_pCurDir->pBrotherDir;
						break;
					}
				}
			}
		}
		else
		{
			m_pCurDir->pChildDir = pDir;
			pDir->pParentDir = m_pCurDir;
			m_pCurDir = m_pCurDir->pChildDir;
		}
	} 
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CTBFat32DiskParse::FinishAppendBrotherDir()
{
	BOOL bRet = TRUE;
	if (m_pCurDir)
	{
		m_pCurDir = m_pCurDir->pParentDir->pChildDir;
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

//返回FALSE时说明所有目录都已经搜索过了
BOOL CTBFat32DiskParse::SearchNextDir()
{
	BOOL bRet = TRUE;
	if (0 == m_pCurDir->strName.CompareNoCase(_T("//")))
	{
		bRet = FALSE;
	} 
	else
	{
		while (0 != m_pCurDir->strName.CompareNoCase(_T("//")) && !m_pCurDir->pBrotherDir)
		{
			m_pCurDir = m_pCurDir->pParentDir;
		}

		if (0 == m_pCurDir->strName.CompareNoCase(_T("//")))
		{
			bRet = FALSE;
		}
		else
		{
			m_pCurDir = m_pCurDir->pBrotherDir;
		}
	}

	return bRet;
}

BOOL CTBFat32DiskParse::ParserRootDir()
{
	if(m_HDevice != INVALID_HANDLE_VALUE)
	{
		byte sectorbuf[SECTOR_BUF_LEN] = { 0 };
		UINT nPartitionHead = 0;

		if (!ReadSectors(m_HDevice, (__int64)nPartitionHead, (short)1, sectorbuf))						//读磁盘第一个扇区
		{
			return FALSE;      
		}
		SetPartitionTable(nPartitionHead);
		//偏移0BH：扇区字节数    00 02 即0X0200,512字节
		UINT nByteNumPerSector = sectorbuf[OFFSET_SECTOR_NUM + 1] * 0x100 + sectorbuf[OFFSET_SECTOR_NUM];	
		//偏移0DH：每簇扇区数    08即每簇包括8个扇区
		UINT nClusterSectorNum = sectorbuf[OFFSET_CLUSTER_NUM];
		//偏移0EH：保留扇区数
		UINT nFatSectorStart = nPartitionHead + sectorbuf[OFFSET_RESERVE_CLUSTER_NUM + 1] * 0x100 + sectorbuf[OFFSET_RESERVE_CLUSTER_NUM];
		UINT nFatSectorNum = 0;
		for(int i = 3; i >= 0; i--)
		{
			//偏移24H：FAT表占用扇区数 
			nFatSectorNum = (nFatSectorNum << 8) + sectorbuf[OFFSET_FAT_SECTOR_NUM + i];                   
		}

		UINT nDirStartClusterNum = 0;
		for(int i = 3; i >= 0; i--)
		{
			//偏移2CH：根目录入口簇号(起始簇号) 02 00 00 00即根目录从02号簇开始
			nDirStartClusterNum = (nDirStartClusterNum << 8) + sectorbuf[OFFSET_ROOT_CLUSTER_NUM + i];         
		}

		SetPartitionInfo(nByteNumPerSector, nClusterSectorNum, nFatSectorStart, nFatSectorNum, nDirStartClusterNum);
		MakeDirTree();
	} 

	return TRUE;
}

BOOL CTBFat32DiskParse::MakeDirTree()
{
	BOOL bRet = TRUE;
	if (!m_pCurPartition || !m_pCurPartition->pPartitionInfo)
	{
		bRet = FALSE;
	}
	else
	{
		GetDirData(m_pCurPartition->pPartitionInfo->dwDirStartClusterNum, TRUE);
	}
	return bRet;
}

BOOL CTBFat32DiskParse::GetDirData(UINT nClusterNum, BOOL bDir)
{
	BOOL bRet = FALSE;
	if (!m_pCurPartition || !m_pCurPartition->pPartitionInfo)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = ReadCluster(nClusterNum, bDir);
	}

	return bRet;
}

BOOL CTBFat32DiskParse::ReadCluster(UINT nClusterNum, BOOL bDir)
{
	BOOL bRet = TRUE;
	DWORD dwTmpClusterNum = nClusterNum;
	BOOL bParserFirstChild = TRUE;
	if (ReadOneCluster(dwTmpClusterNum, bDir, bParserFirstChild))
	{
		while (GetNextClusterNum(&dwTmpClusterNum))
		{
			bRet = ReadOneCluster(dwTmpClusterNum, bDir, bParserFirstChild);
		}
	}
	return bRet;
}

BOOL CTBFat32DiskParse::ReadOneCluster(UINT nClusterNum, BOOL bDir, BOOL &bParserFirstChild)
{
	BOOL bRet = FALSE;
	if (!m_pCurPartition || !m_pCurPartition->pPartitionInfo || INVALID_HANDLE_VALUE == m_HDevice)
	{
		bRet = FALSE;
	}
	else
	{
		byte *pClusterBuf = NULL;
		pClusterBuf = MallocClusterBuf();
		if (!pClusterBuf)
		{
			bRet = FALSE;
		}
		else
		{
			UINT nSectorStart = 0;
			//簇对应的其始扇区号
			nSectorStart = m_pCurPartition->pPartitionInfo->dwDataSectorStart + (nClusterNum - 2) * m_pCurPartition->pPartitionInfo->dwClusterSectorNum;
			//从磁盘数据区读一簇数据
			if (ReadSectors(m_HDevice, (__int64)nSectorStart, (short)(m_pCurPartition->pPartitionInfo->dwClusterSectorNum), pClusterBuf))
			{
				if (bDir)
				{
					bRet = GetDirFormBuf(pClusterBuf, m_pCurPartition->pPartitionInfo->dwByteNumPerCluster, bParserFirstChild);
				}
				else
				{
					bRet = TRUE;
				}
			}
		}
	}

	return bRet;
}

BOOL CTBFat32DiskParse::GetNextClusterNum(DWORD *dwClusterNum)
{
	BOOL bRet = FALSE;
	if (!m_pCurPartition || !m_pCurPartition->pPartitionInfo || INVALID_HANDLE_VALUE == m_HDevice)
	{
		bRet = FALSE;
	}
	else
	{
		byte sectorbuf[SECTOR_BUF_LEN] = { 0 };
		UINT nSectorNum = 0;
		UINT nOffset = 0;
		UINT nClusterNum = *dwClusterNum * 4;
		nSectorNum = nClusterNum / 512 + m_pCurPartition->pPartitionInfo->dwFatSectorStart;
		nOffset = nClusterNum % 512;

		if (ReadSectors(m_HDevice, (__int64)nSectorNum, (short)1, sectorbuf))  //读FAT表的一个扇区
		{
			nClusterNum = ((sectorbuf[nOffset + 3] * 0x100 + sectorbuf[nOffset + 2]) * 0x100 + sectorbuf[nOffset + 1]) * 0x100 + sectorbuf[nOffset];
			if (TBCommonFun::IsClusterValid(nClusterNum))
			{
				*dwClusterNum = nClusterNum;
				bRet = TRUE;
			}
		}
	}
		
	return bRet;
}

void CTBFat32DiskParse::ParserLongDirFromBuf(byte *pBuf)
{
	if (pBuf)
	{
		int iRowNum = 0;
		//如果是该文件的最后一个长文件名目录项
		if (pBuf[0] & LONG_DIR_END)
		{
			m_nRowGet = pBuf[0] & 0x1F;
			iRowNum = m_nRowGet - 1;
		}
		else
		{
			iRowNum = (pBuf[0] & 0x1F) - 1;
		}
		memcpy(&m_byLongFileName[iRowNum][0], &pBuf[0 + OFFSET_LONG_NAME1], 10);
		memcpy(&m_byLongFileName[iRowNum][10], &pBuf[0 + OFFSET_LONG_NAME2], 12);
		memcpy(&m_byLongFileName[iRowNum][22], &pBuf[0 + OFFSET_LONG_NAME3], 4);
		m_nRowReal++;
	}
}

CString  CTBFat32DiskParse::ParserShortDirFromBuf(byte *pBuf)
{
	CString strName = _T("");
	lpTBShortDirInfo pShortDirInfo = (lpTBShortDirInfo)pBuf; //短文件名项
	if (TBCommonFun::GetDirAttribute(pShortDirInfo->attribute))
	{
		byte byShortFileName[15] = { 0 };
		memcpy(byShortFileName, pBuf + 0, SNAME_LEN + SEXT_LEN);
		strName = CString(byShortFileName);
		strName.TrimRight();
		memset(byShortFileName, 0, sizeof(byShortFileName));
	}
	else
	{
		BOOL bFileNameUpper = TRUE;
		BOOL bExtensionUpper = TRUE;
		if (pShortDirInfo->reserved & FILE_NAME_LOEWR)
		{
			bFileNameUpper = FALSE;
		}
		if (pShortDirInfo->reserved & FILE_EXTENSION_LOEWR)
		{
			bExtensionUpper = FALSE;
		}

		byte byTmpFileName[9] = { 0 };
		byte byTmpExtension[4] = { 0 };

		memcpy(byTmpFileName, pBuf, SNAME_LEN);       //文件名
		memcpy(byTmpExtension, pBuf + SNAME_LEN, SEXT_LEN);  //扩展名
		CString strTmpFileName = CString(byTmpFileName);
		strTmpFileName.TrimRight();
		CString strTmpExtension = CString(byTmpExtension);
		strTmpExtension.TrimRight();

		if (strTmpExtension.IsEmpty())
		{
			strName.Format(_T("%s"), bFileNameUpper ? strTmpFileName.MakeUpper() : strTmpFileName.MakeLower());
		}
		else
		{
			strName.Format(_T("%s.%s"), (bFileNameUpper ? strTmpFileName.MakeUpper() : strTmpFileName.MakeLower()), (bExtensionUpper ? strTmpExtension.MakeUpper() : strTmpExtension.MakeLower()));
		}
	}

	return strName;
}

//从buffer中解析目录项相关参数
void CTBFat32DiskParse::ParserDirParaFromBuf(byte *pBuf, byte &nAttribute, CString &strDateTime, UINT &nStartClusterNum, DWORD &nDirsize)
{
	nAttribute = pBuf[OFFSET_ATTRIBUTE + 0];
	WORD wDate = 0, wTime = 0;
	wTime = pBuf[OFFSET_TIME_HIGH + 0] * 0x100 + pBuf[OFFSET_TIME_LOW + 0];
	wDate = pBuf[OFFSET_DATE_HIGH + 0] * 0x100 + pBuf[OFFSET_DATE_LOW + 0];
	strDateTime = TBCommonFun::DateTimeToStr(wDate, wTime);
	//目录或文件的起始簇号
	nStartClusterNum = ((pBuf[OFFSET_START_SECTOR_HIGH2 + 0] * 0x100 + pBuf[OFFSET_START_SECTOR_HIGH1 + 0]) * 0x100 + pBuf[OFFSET_START_SECTOR_LOW2 + 0]) * 0x100 + pBuf[OFFSET_START_SECTOR_LOW1 + 0];
	nDirsize = ((pBuf[OFFSET_FILE_SIZE4 + 0] * 0x100 + pBuf[OFFSET_FILE_SIZE3 + 0]) * 0x100 + pBuf[OFFSET_FILE_SIZE2 + 0]) * 0x100 + pBuf[OFFSET_FILE_SIZE1 + 0];
}

//解析一簇buffer中的目录项
BOOL CTBFat32DiskParse::GetDirFormBuf(byte *pBuf, UINT nBufLen, BOOL &bParserFirstChild)
{
	if (!pBuf || nBufLen % DIR_LEN != 0)
		return FALSE;

	CString strDirName = _T("");
	CString strDateTime = _T("");

	for (UINT pos = 0; pos < nBufLen; pos = pos + DIR_LEN)
	{
		strDirName = _T("");

		PBYTE pDirBuffer = new BYTE[DIR_LEN];
		memset(pDirBuffer, 0, DIR_LEN);
		memcpy(pDirBuffer, pBuf + pos, DIR_LEN);

		if (FDT_END == pDirBuffer[0])
		{
			FinishAppendBrotherDir();
			TBSAFEDELETEARRAY(pDirBuffer);
			break;
		}

		if (FDT_DELETE == pDirBuffer[0] || FDT_CUR_DIR == pDirBuffer[0])  //E5-表示该项曾经被使用过，但文件被删除。 2E-表示“.”“..”文件夹
		{
			TBSAFEDELETEARRAY(pDirBuffer);
			continue;
		}
			
		if (FDT_LONG == pDirBuffer[0 + OFFSET_ATTRIBUTE])  //长文件名
		{
			ParserLongDirFromBuf(pDirBuffer);
			TBSAFEDELETEARRAY(pDirBuffer);
			continue;
		}
		else
		{
			if (m_nRowGet > 0 && m_nRowGet == m_nRowReal) //长文件项解析完成后对应的整个文件名
			{
				strDirName = _T("");
				WCHAR *wch = NULL;
				wch = (WCHAR*)m_byLongFileName;
				strDirName = strDirName + CString(wch);
				strDirName.TrimRight();
				memset(m_byLongFileName, 0, sizeof(m_byLongFileName));
			}
			else //直接解析短文件名项
			{
				strDirName = ParserShortDirFromBuf(pDirBuffer);
			}
		}

		byte nAttribute = 0;
		CString strDateTime = _T("");
		UINT nStartClusterNum = 0;
		DWORD nDirsize = 0;
		ParserDirParaFromBuf(pDirBuffer, nAttribute, strDateTime, nStartClusterNum, nDirsize);

		if (bParserFirstChild)
		{
			AppendChildDir(strDirName, nAttribute, strDateTime, nStartClusterNum, nDirsize);
			bParserFirstChild = FALSE;
		}
		else
		{
			AppendBrotherDir(strDirName, nAttribute, strDateTime, nStartClusterNum, nDirsize);
		}
		m_nRowGet = 0;
		m_nRowReal = 0;

		TBSAFEDELETEARRAY(pDirBuffer);
	}
	return TRUE;
}


//根据参数目录项的获取其子目录项
BOOL CTBFat32DiskParse::GetCurDirChildData(TBDirInfo* pDir)
{
	BOOL bRet = FALSE;
	if (!m_pCurPartition || !m_pCurPartition->pPartitionInfo || !pDir)
	{
		return bRet;
	}
	else
	{
		m_pCurDir = pDir;
		while (m_pCurDir && !m_pCurDir->bDir && m_pCurDir->pBrotherDir)
		{
			m_pCurDir = m_pCurDir->pBrotherDir;
		}

		BOOL bContinue = TRUE;
		if (m_pCurDir && !m_pCurDir->bDir)
		{
			if (!SearchNextDir())
			{
				bContinue = FALSE;
			}
		}

		if (bContinue)
		{
			bRet = ReadCluster(m_pCurDir->dwStartClusterNum, TRUE);
		}
	}
		
	return bRet;
}
