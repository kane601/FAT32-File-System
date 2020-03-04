
/**
*
* Copyright (C) 2002-2017�ɶ����ҿƼ����޹�˾
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
	//��ȡ��ǰ������Ŀ¼�ڵ�
	inline TBDirInfo* GetRootDir(TBPartitionTable *pCurPartition)
	{
		TBDirInfo* pRootDir = NULL;
		if (pCurPartition && pCurPartition->pPartitionInfo && pCurPartition->pPartitionInfo->pDirInfo)
		{
			pRootDir = pCurPartition->pPartitionInfo->pDirInfo;
		}

		return pRootDir;
	}

	//��ȡ��ǰĿ¼����Ŀ¼��ļ���
	inline TBDirInfo* GetChildDir(TBDirInfo* pDir)
	{
		TBDirInfo* pChildDir = NULL;
		if (pDir && pDir->pChildDir)
		{
			pChildDir = pDir->pChildDir;
		}

		return pChildDir;
	}

	//��ȡ����Ŀ¼����ֵ�Ŀ¼��
	inline TBDirInfo* GetNextChildDir(TBDirInfo* pDir)
	{
		TBDirInfo* pBrotherDir = NULL;
		if (pDir && pDir->pBrotherDir)
		{
			pBrotherDir = pDir->pBrotherDir;
		}
		return pBrotherDir;
	}

	//��ȡ��ǰ�ļ���ĸ��ڵ���
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
			//�������ϵͳ�����ļ��в���ʾ���б�
			if ((pDir->bVolumn && 0 == pDir->dwSize && 0 == pDir->dwStartClusterNum) || (pDir->bSystem && pDir->bHide))
			{
				bShow = FALSE;
			}
		}

		return bShow;
	}

	//ֻ�������ж�
	inline bool GetReadOnlyAttribute(byte nAttribute)
	{
		bool bReadOnly = false;
		if (nAttribute & FDT_ATTRI_READONLY)
		{
			bReadOnly = true;
		}

		return bReadOnly;
	}

	//���������ж�
	inline bool GetHideAttribute(byte nAttribute)
	{
		bool bHide = false;
		if (nAttribute & FDT_ATTRI_HIDE)
		{
			bHide = true;
		}

		return bHide;
	}

	//ϵͳ�����ж�
	inline bool GetSystemAttribute(byte nAttribute)
	{
		bool bSystem = false;
		if (nAttribute & FDT_ATTRI_SYSTEM)
		{
			bSystem = true;
		}

		return bSystem;
	}

	//��������ж�
	inline bool GetVolumnAttribute(byte nAttribute)
	{
		bool bVolumn = false;
		if (nAttribute & FDT_ATTRI_VOLUMN)
		{
			bVolumn = true;
		}

		return bVolumn;
	}

	//��Ŀ¼�����ж�
	inline bool GetDirAttribute(byte nAttribute)
	{
		bool bDir = false;
		if (nAttribute & FDT_ATTRI_DIR)
		{
			bDir = true;
		}

		return bDir;
	}

	//�鵵�����ж�
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

	//ʱ��ת������
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