
/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
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
	BOOL InitDiskHandle( CString strDiskName );    //根据参数初始化盘符句柄
	BOOL ParserRootDir();                          //解析根目录文件项
	BOOL GetCurDirChildData(TBDirInfo* pDir);      

public:
	TBPartitionTable *m_pCurPartition;

private:
	//构建分区结构
	BOOL SetPartitionTable(DWORD dwStartSector);
	BOOL SetPartitionInfo(DWORD dwByteNumPerSector, DWORD dwClusterSectorNum, DWORD dwFATSectorStart, DWORD dwFATSectorNum, DWORD dwDirStartClusterNum);
	
	//文件目录项
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
	
	//释放内存
	void     ReleasePartitionTable();
	void     ReleaseDir(TBDirInfo *pDirInfo);

	//簇内存申请
	byte*    MallocClusterBuf();
	void     ReleaseClusterBuf();


	//通过buffer解析文件目录项相关参数
	void     ParserLongDirFromBuf(byte *pBuf);
	CString  ParserShortDirFromBuf(byte *pBuf);
	void     ParserDirParaFromBuf(byte *pBuf, byte &nAttribute, CString &strDateTime, UINT &nStartClusterNum, DWORD &nDirsize);
	void     ParserDirParaFromAttribute(byte nAttribute, TBDirInfo *pDirInfo);

private:
	TBPartitionTable *m_pPTHead;       
	TBDirInfo *m_pCurDir;
	byte *m_pClusterBuf;   
	HANDLE m_HDevice;      //盘符句柄
	CString m_strDisk; //标识盘符名

	//一个文件或文件夹对应的长文件项和短文件名项可能跨簇，所以需要全局变量存储
	byte m_byLongFileName[12][26]; //存储长文件名
	int m_nRowGet;                 //长文件名总共项数
	int m_nRowReal;                //实际解析长文件名项数
};

typedef std::map<CString, CTBFat32DiskParse*> DiskListMap;

#endif