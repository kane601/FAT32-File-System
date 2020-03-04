
/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
* All rights reserved.
*
* @file   TBCommonFunction.h
* @author qiaozhousan
* @date   2017-05-10
*
* @brief  Common functions are used for attribute judgment and tree traversal
*
*/

#ifndef _TBFat32CommonFunc_H__
#define _TBFat32CommonFunc_H__

#include "TBFat32DataStruct.h"
#include "TBFat32MaskDefine.h"

namespace TBCommonFun
{
	//获取当前分区根目录节点
	inline TBDirInfo* GetRootDir(TBPartitionTable *pCurPartition)
	{
		TBDirInfo* pRootDir = NULL;
		if (pCurPartition && pCurPartition->pPartitionInfo && pCurPartition->pPartitionInfo->pDirInfo)
		{
			pRootDir = pCurPartition->pPartitionInfo->pDirInfo;
		}

		return pRootDir;
	}

	//获取当前目录的子目录项及文件项
	inline TBDirInfo* GetChildDir(TBDirInfo* pDir)
	{
		TBDirInfo* pChildDir = NULL;
		if (pDir && pDir->pChildDir)
		{
			pChildDir = pDir->pChildDir;
		}

		return pChildDir;
	}

	//获取参数目录项的兄弟目录项
	inline TBDirInfo* GetNextChildDir(TBDirInfo* pDir)
	{
		TBDirInfo* pBrotherDir = NULL;
		if (pDir && pDir->pBrotherDir)
		{
			pBrotherDir = pDir->pBrotherDir;
		}
		return pBrotherDir;
	}

	//获取当前文件项的父节点项
	inline TBDirInfo* GetParentDir(TBDirInfo* pDir)
	{
		TBDirInfo* pParentDir = NULL;
		if (pDir && pDir->pParentDir)
		{
			pParentDir = pDir->pParentDir;
		}
		return pParentDir;
	}

	inline BOOL IsShowDir(TBDirInfo* pDir)
	{
		BOOL bShow = TRUE;
		if (pDir)
		{
			//卷标名或系统隐藏文件夹不显示到列表
			if ((pDir->bVolumn && 0 == pDir->dwSize && 0 == pDir->dwStartClusterNum) || (pDir->bSystem && pDir->bHide))
			{
				bShow = FALSE;
			}
		}

		return bShow;
	}

	//只读属性判断
	inline bool GetReadOnlyAttribute(byte nAttribute)
	{
		bool bReadOnly = false;
		if (nAttribute & FDT_ATTRI_READONLY)
		{
			bReadOnly = true;
		}

		return bReadOnly;
	}

	//隐藏属性判断
	inline bool GetHideAttribute(byte nAttribute)
	{
		bool bHide = false;
		if (nAttribute & FDT_ATTRI_HIDE)
		{
			bHide = true;
		}

		return bHide;
	}

	//系统属性判断
	inline bool GetSystemAttribute(byte nAttribute)
	{
		bool bSystem = false;
		if (nAttribute & FDT_ATTRI_SYSTEM)
		{
			bSystem = true;
		}

		return bSystem;
	}

	//卷标属性判断
	inline bool GetVolumnAttribute(byte nAttribute)
	{
		bool bVolumn = false;
		if (nAttribute & FDT_ATTRI_VOLUMN)
		{
			bVolumn = true;
		}

		return bVolumn;
	}

	//子目录属性判断
	inline bool GetDirAttribute(byte nAttribute)
	{
		bool bDir = false;
		if (nAttribute & FDT_ATTRI_DIR)
		{
			bDir = true;
		}

		return bDir;
	}

	//归档属性判断
	inline bool GetArchiveAttribute(byte nAttribute)
	{
		bool bArchived = false;
		if (nAttribute & FDT_ATTRI_ARCHIVE)
		{
			bArchived = true;
		}

		return bArchived;
	}

	inline bool IsClusterValid(UINT &uClusterNum)
	{
		bool bValid = false;
		if (uClusterNum != CLUSTER_ATTRIBUTE_NOTUSED
			&& uClusterNum != CLUSTER_ATTRIBUTE_RESERVE0
			&& uClusterNum != CLUSTER_ATTRIBUTE_RESERVE6
			&& uClusterNum != CLUSTER_ATTRIBUTE_BAD
			&& uClusterNum != CLUSTER_ATTRIBUTE_EOF8
			&& uClusterNum != CLUSTER_ATTRIBUTE_EOFF
			&& uClusterNum != CLUSTER_ATTRIBUTE_FILEEOF)
		{
			bValid = true;
		}

		return bValid;
	}

	//时间转换函数
	inline CString DateTimeToStr(WORD wDate, WORD wTime)
	{
		int nDay = wDate & 0x001F;
		int nMonth = (wDate >> 5) & 0x0F;
		int nYear = (wDate >> 9) & 0x7F;
		nYear += 1980;

		int nSecond = wTime & 0x001F;
		int nMinute = (wTime >> 5) & 0x3F;
		int nHour = (wTime >> 11) & 0x1F;

		CString strDateTime;
		strDateTime.Format(_T("%d-%02d-%02d %02d:%02d"), nYear, nMonth, nDay, nHour, nMinute);
		return strDateTime;
	}

}

#endif