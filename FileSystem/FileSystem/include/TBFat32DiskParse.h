
/**
*
* Copyright (C) 2002-2017�ɶ����ҿƼ����޹�˾
* All rights reserved.
*
* @file   CTBFat32DiskParse.h
* @author qiaozhousan
* @date   2017-05-10
*
* @brief  Parsing FAT32 file systems
*
*/

#ifndef _TBFat32DiskParser_H__
#define _TBFat32DiskParser_H__
#include "TBCommonFunction.h"

#include <map>

class CTBFat32DiskParse
{
public:
	CTBFat32DiskParse();
	~CTBFat32DiskParse();

public:
	BOOL InitDiskHandle( CString strDiskName );    //���ݲ�����ʼ���̷����
	BOOL ParserRootDir();                          //������Ŀ¼�ļ���
	BOOL GetCurDirChildData(TBDirInfo* pDir);      

public:
	TBPartitionTable *m_pCurPartition;

private:
	//���������ṹ
	BOOL SetPartitionTable(DWORD dwStartSector);
	BOOL SetPartitionInfo(DWORD dwByteNumPerSector, DWORD dwClusterSectorNum, DWORD dwFATSectorStart, DWORD dwFATSectorNum, DWORD dwDirStartClusterNum);
	
	//�ļ�Ŀ¼��
	BOOL AppendBrotherDir(CString strName, byte nAttribute, CString strDateTime, DWORD dwStartClusterNum, DWORD nDirSize);
	BOOL AppendChildDir(CString strName, byte nAttribute, CString strDateTime, DWORD dwStartClusterNum, DWORD nDirSize);
	BOOL FinishAppendBrotherDir();
	BOOL SearchNextDir();

	BOOL MakeDirTree();
	BOOL GetDirData(UINT nClusterNum, BOOL bDir);
	BOOL GetDirFormBuf(byte *pBuf, UINT BufLen, BOOL &bParserFirstChild);

	BOOL ReadCluster(UINT nClusterNum, BOOL bDir);
	BOOL ReadOneCluster(UINT nClusterNum, BOOL bDir, BOOL &bParserFirstChild);
	BOOL GetNextClusterNum(DWORD *dwClusterNum);

	BOOL     ReadSectors(HANDLE hFile, __int64 logicalLba, short nSectors, unsigned char *buff);
	__int64  FileSeek(HANDLE hFile, __int64 distance, DWORD dwMoveMethod);
	
	//�ͷ��ڴ�
	void     ReleasePartitionTable();
	void     ReleaseDir(TBDirInfo *pDirInfo);

	//���ڴ�����
	byte*    MallocClusterBuf();
	void     ReleaseClusterBuf();


	//ͨ��buffer�����ļ�Ŀ¼����ز���
	void     ParserLongDirFromBuf(byte *pBuf);
	CString  ParserShortDirFromBuf(byte *pBuf);
	void     ParserDirParaFromBuf(byte *pBuf, byte &nAttribute, CString &strDateTime, UINT &nStartClusterNum, DWORD &nDirsize);
	void     ParserDirParaFromAttribute(byte nAttribute, TBDirInfo *pDirInfo);

private:
	TBPartitionTable *m_pPTHead;       
	TBDirInfo *m_pCurDir;
	byte *m_pClusterBuf;   
	HANDLE m_HDevice;      //�̷����
	CString m_strDisk; //��ʶ�̷���

	//һ���ļ����ļ��ж�Ӧ�ĳ��ļ���Ͷ��ļ�������ܿ�أ�������Ҫȫ�ֱ����洢
	byte m_byLongFileName[12][26]; //�洢���ļ���
	int m_nRowGet;                 //���ļ����ܹ�����
	int m_nRowReal;                //ʵ�ʽ������ļ�������
};

typedef std::map<CString, CTBFat32DiskParse*> DiskListMap;

#endif