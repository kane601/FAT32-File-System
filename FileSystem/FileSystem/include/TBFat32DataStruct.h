/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
* All rights reserved.
*
* @file   TBFat32DataStruct.h
* @author qiaozhousan
* @date   2017-05-11
*
* @brief  Data structures are used to parse FAT32 file systems
*
*/

#ifndef _TBFat32DataStruct_H__
#define _TBFat32DataStruct_H__

typedef  struct _TBBootSector
{
	BYTE  JMPCode[3];							// 0  0x00~0x02：3字节，跳转指令。
	unsigned char  ucOEMName[8];				// 3  0x03~0x0A：8字节，文件系统标志和版本号，这里为MSDOC5.0
	WORD           wBytesPerSector;				// 11 0x0B~0x0C：2字节，每扇区字节数，512（0X02 00）。 
	unsigned char  ucSectorsPerCluster;         // 13 0x0D~0x0D：1字节，每簇扇区数，8（0x08）。
	unsigned short usReservedSectors;			// 14 0x0E~0x0F：2字节，保留扇区数，38（0x00 26），符合FAT1起始地址为38扇区。
	unsigned char  ucFATCount;					// 16 0x10~0x10：1字节，FAT表个数，2。
	unsigned short usRootEntries;				// 17 0x11~0x12：2字节，FAT32必须等于0，FAT12/FAT16为根目录中目录的个数；
	unsigned short usSectors;					//    0x13~0x14：2字节，FAT32必须等于0，FAT12/FAT16为扇区总数。 
	unsigned char  ucMedia;						// 21 0x15~0x15：1字节，哪种存储介质，0xF8标准值，可移动存储介质，常用的 0xF0。 
	unsigned short usFATSectors;				// 22 0x16~0x17: 2字节，FAT32必须为0，FAT12/FAT16为一个FAT 表所占的扇区数。
	unsigned short usSectorsPerTrack;			//    0x18~0x19：2字节，每磁道扇区数，只对于有“特殊形状”（由磁头和柱面每 分割为若干磁道）的存储介质有效，63（0x00 3F）。 
	unsigned short usHeads;						//    0x1A~0x1B：2字节，磁头数，只对特殊的介质才有效，255（0x00 FF）。
	unsigned long  ulHiddenSectors;				// 28 0x1C~0x1F：4字节，EBR分区之前所隐藏的扇区数，8192（0x00 00 20 00），与MBR中地址0x1C6开始的4个字节数值相等。
	unsigned long  ulSectorsHuge;				//    0x20~0x23：4字节，文件系统总扇区数，7736320（0x 00 76 0C 00），7736320 *  512 = 3960995840  ≈ 3.67GB。
	unsigned long  ulSectorsperFAT;              //36 0x24~0x27：4字节，每个FAT表占用扇区数，7541（0x 00 00 1D 75）。
	unsigned short usExtFlag;					// 40 0x28~0x29：2字节，标记，此域FAT32 特有。
	unsigned short usFSVersion;					// 42 0x2A~0x2B：2字节，FAT32版本号0.0，FAT32特有。
	unsigned long  ulRootdir1stcluster;         // 44 0x2C~0x2F：4字节，根目录所在第一个簇的簇号，2。（虽然在FAT32文件系统 下，根目录可以存放在数据区的任何位置，但是通常情况下还是起始于2号簇） 
	unsigned short usFSInfoSector;              // 48 0x30~0x31：2字节，FSINFO（文件系统信息扇区）扇区号1，该扇区为操作 系统提供关于空簇总数及下一可用簇的信息。
	unsigned short usBootBackup;				// 50 0x32~0x33：2字节，备份引导扇区的位置。备份引导扇区总是位于文件系统 的6号扇区。 
	unsigned char  ucRes3[12];					// 52 0x34~0x3F：12字节，用于以后FAT 扩展使用。 
	unsigned char  ucDrive;						// 64 0x40~0x40：1字节，与FAT12/16 的定义相同，只不过两者位于启动扇区不同的位置而已。
	unsigned char  ucRes4;						// 65 0x41~0x41：1字节，与FAT12/16 的定义相同，只不过两者位于启动扇区不同的位置而已 。 
	unsigned char  ucExtBootSignature;          // 66 0x42~0x42：1字节，扩展引导标志，0x29。与FAT12/16 的定义相同，只不过 两者位于启动扇区不同的位置而已
	unsigned long  ulVolumeID;					// 67 0x43~0x46：4字节，卷序列号。通常为一个随机值。
	unsigned char  ucVolumeLabel[11];			// 71 0x47~0x51：11字节，卷标（ASCII码），如果建立文件系统的时候指定了卷标，会保存在此。 
	unsigned char  ucSysType[8];				//    0x52~0x59：8字节，文件系统格式的ASCII码，FAT32。 
	unsigned char  ucRes2[420];					// 90 0x5A~0x1FD：90~509共410字节，未使用。该部分没有明确的用途。
	unsigned short usSignature1;				// 510 0x1FE~0x1FF：签名标志“55 AA”。
}TBBootSector, *lpTBBootSector;;


// 短文件名目录项数据结构
struct TBShortDirInfo
{
	char strFilename[8];
	char strExtension[3];
	char attribute;
	char reserved;
	char millisecond;
	unsigned short createTime;
	unsigned short createDate;
	unsigned short accessDate;
	unsigned short highWordCluster;
	unsigned short updateTime;
	unsigned short updateDate;
	unsigned short lowWordCluster;
	unsigned int filesize;
};
typedef TBShortDirInfo* lpTBShortDirInfo;

struct TBDirInfo
{
	CString strName;		//目录或文件名
	BOOL   bReadOnly;		//是否是只读的
	BOOL   bHide;			//是否是隐藏的
	BOOL   bSystem;			//是否是系统的
	BOOL   bVolumn;			//是否是卷标
	BOOL   bDir;			//是否是目录
	BOOL   bArchived;		//是否是存档的
	DWORD  dwStartClusterNum;	//目录或文件起始簇号
	DWORD  dwSize;				//目录或文件的大小(byte)
	CString strDateTime;		//最终修改日期时间
	CString strDisk;            //盘符名
	BOOL    bCheckedChildDir;   //是否已解析该文件夹下子目录，仅针对Dir有效

	TBDirInfo *pChildDir;     //子目录指针
	TBDirInfo *pParentDir;    //父目录指针
	TBDirInfo *pBrotherDir;   //兄弟目录指针

	TBDirInfo()
	{
		Reset();
	}

	void Reset()
	{
		strName = _T("");
		bReadOnly = FALSE;
		bHide     = FALSE;
		bSystem   = FALSE;
		bVolumn   = FALSE;
		bDir      = FALSE;
		bArchived = FALSE;
		dwStartClusterNum = 2;
		dwSize            = FALSE;
		strDateTime		  = _T("");
		strDisk           = _T("");
		bCheckedChildDir  = FALSE;
		pChildDir		  = NULL;
		pParentDir        = NULL;
		pBrotherDir       = NULL;
	}
};
typedef TBDirInfo* lpTBDirInfo;  

struct TBPartitionInfo
{
	DWORD dwByteNumPerSector;   //每扇区字节数
	DWORD dwClusterSectorNum;   //每簇扇区数
	DWORD dwFatSectorStart;     //fat1起始扇区号
	DWORD dwFatSectorNum;       //fat扇区数
	DWORD dwDirStartClusterNum; //根目录起始簇号
	DWORD dwDataSectorStart;    //数据区的起始扇区号
	DWORD dwByteNumPerCluster;  //每簇的字节数
	TBDirInfo *pDirInfo;

	TBPartitionInfo()
	{
		Reset();
	}

	void Reset()
	{
		dwByteNumPerSector = 512;
		dwClusterSectorNum = 8;
		dwFatSectorStart = 32;
		dwFatSectorNum = 2;
		dwDirStartClusterNum = 2;
		dwDataSectorStart = 0xFFFFFFFF;
		dwByteNumPerCluster = 0x1000; //8*512
		pDirInfo = NULL;
	}
};
typedef TBPartitionInfo* lpTBPartitionInfo;  

struct TBPartitionTable
{
	DWORD dwStartSector;    //分区的起始扇区号
	TBPartitionInfo  *pPartitionInfo;
	TBPartitionTable *pNextPT;

	TBPartitionTable()
	{
		Reset();
	}

	void Reset()
	{
		dwStartSector = 63;
		pPartitionInfo = NULL;
		pNextPT = NULL;
	}
};

#endif