/**
*
* Copyright (C) 2002-2017�ɶ����ҿƼ����޹�˾
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
	BYTE  JMPCode[3];							// 0  0x00~0x02��3�ֽڣ���תָ�
	unsigned char  ucOEMName[8];				// 3  0x03~0x0A��8�ֽڣ��ļ�ϵͳ��־�Ͱ汾�ţ�����ΪMSDOC5.0
	WORD           wBytesPerSector;				// 11 0x0B~0x0C��2�ֽڣ�ÿ�����ֽ�����512��0X02 00���� 
	unsigned char  ucSectorsPerCluster;         // 13 0x0D~0x0D��1�ֽڣ�ÿ����������8��0x08����
	unsigned short usReservedSectors;			// 14 0x0E~0x0F��2�ֽڣ�������������38��0x00 26��������FAT1��ʼ��ַΪ38������
	unsigned char  ucFATCount;					// 16 0x10~0x10��1�ֽڣ�FAT�������2��
	unsigned short usRootEntries;				// 17 0x11~0x12��2�ֽڣ�FAT32�������0��FAT12/FAT16Ϊ��Ŀ¼��Ŀ¼�ĸ�����
	unsigned short usSectors;					//    0x13~0x14��2�ֽڣ�FAT32�������0��FAT12/FAT16Ϊ���������� 
	unsigned char  ucMedia;						// 21 0x15~0x15��1�ֽڣ����ִ洢���ʣ�0xF8��׼ֵ�����ƶ��洢���ʣ����õ� 0xF0�� 
	unsigned short usFATSectors;				// 22 0x16~0x17: 2�ֽڣ�FAT32����Ϊ0��FAT12/FAT16Ϊһ��FAT ����ռ����������
	unsigned short usSectorsPerTrack;			//    0x18~0x19��2�ֽڣ�ÿ�ŵ���������ֻ�����С�������״�����ɴ�ͷ������ÿ �ָ�Ϊ���ɴŵ����Ĵ洢������Ч��63��0x00 3F���� 
	unsigned short usHeads;						//    0x1A~0x1B��2�ֽڣ���ͷ����ֻ������Ľ��ʲ���Ч��255��0x00 FF����
	unsigned long  ulHiddenSectors;				// 28 0x1C~0x1F��4�ֽڣ�EBR����֮ǰ�����ص���������8192��0x00 00 20 00������MBR�е�ַ0x1C6��ʼ��4���ֽ���ֵ��ȡ�
	unsigned long  ulSectorsHuge;				//    0x20~0x23��4�ֽڣ��ļ�ϵͳ����������7736320��0x 00 76 0C 00����7736320 *  512 = 3960995840  �� 3.67GB��
	unsigned long  ulSectorsperFAT;              //36 0x24~0x27��4�ֽڣ�ÿ��FAT��ռ����������7541��0x 00 00 1D 75����
	unsigned short usExtFlag;					// 40 0x28~0x29��2�ֽڣ���ǣ�����FAT32 ���С�
	unsigned short usFSVersion;					// 42 0x2A~0x2B��2�ֽڣ�FAT32�汾��0.0��FAT32���С�
	unsigned long  ulRootdir1stcluster;         // 44 0x2C~0x2F��4�ֽڣ���Ŀ¼���ڵ�һ���صĴغţ�2������Ȼ��FAT32�ļ�ϵͳ �£���Ŀ¼���Դ�������������κ�λ�ã�����ͨ������»�����ʼ��2�Ŵأ� 
	unsigned short usFSInfoSector;              // 48 0x30~0x31��2�ֽڣ�FSINFO���ļ�ϵͳ��Ϣ������������1��������Ϊ���� ϵͳ�ṩ���ڿմ���������һ���ôص���Ϣ��
	unsigned short usBootBackup;				// 50 0x32~0x33��2�ֽڣ���������������λ�á�����������������λ���ļ�ϵͳ ��6�������� 
	unsigned char  ucRes3[12];					// 52 0x34~0x3F��12�ֽڣ������Ժ�FAT ��չʹ�á� 
	unsigned char  ucDrive;						// 64 0x40~0x40��1�ֽڣ���FAT12/16 �Ķ�����ͬ��ֻ��������λ������������ͬ��λ�ö��ѡ�
	unsigned char  ucRes4;						// 65 0x41~0x41��1�ֽڣ���FAT12/16 �Ķ�����ͬ��ֻ��������λ������������ͬ��λ�ö��� �� 
	unsigned char  ucExtBootSignature;          // 66 0x42~0x42��1�ֽڣ���չ������־��0x29����FAT12/16 �Ķ�����ͬ��ֻ���� ����λ������������ͬ��λ�ö���
	unsigned long  ulVolumeID;					// 67 0x43~0x46��4�ֽڣ������кš�ͨ��Ϊһ�����ֵ��
	unsigned char  ucVolumeLabel[11];			// 71 0x47~0x51��11�ֽڣ���꣨ASCII�룩����������ļ�ϵͳ��ʱ��ָ���˾�꣬�ᱣ���ڴˡ� 
	unsigned char  ucSysType[8];				//    0x52~0x59��8�ֽڣ��ļ�ϵͳ��ʽ��ASCII�룬FAT32�� 
	unsigned char  ucRes2[420];					// 90 0x5A~0x1FD��90~509��410�ֽڣ�δʹ�á��ò���û����ȷ����;��
	unsigned short usSignature1;				// 510 0x1FE~0x1FF��ǩ����־��55 AA����
}TBBootSector, *lpTBBootSector;;


// ���ļ���Ŀ¼�����ݽṹ
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
	CString strName;		//Ŀ¼���ļ���
	BOOL   bReadOnly;		//�Ƿ���ֻ����
	BOOL   bHide;			//�Ƿ������ص�
	BOOL   bSystem;			//�Ƿ���ϵͳ��
	BOOL   bVolumn;			//�Ƿ��Ǿ��
	BOOL   bDir;			//�Ƿ���Ŀ¼
	BOOL   bArchived;		//�Ƿ��Ǵ浵��
	DWORD  dwStartClusterNum;	//Ŀ¼���ļ���ʼ�غ�
	DWORD  dwSize;				//Ŀ¼���ļ��Ĵ�С(byte)
	CString strDateTime;		//�����޸�����ʱ��
	CString strDisk;            //�̷���
	BOOL    bCheckedChildDir;   //�Ƿ��ѽ������ļ�������Ŀ¼�������Dir��Ч

	TBDirInfo *pChildDir;     //��Ŀ¼ָ��
	TBDirInfo *pParentDir;    //��Ŀ¼ָ��
	TBDirInfo *pBrotherDir;   //�ֵ�Ŀ¼ָ��

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
	DWORD dwByteNumPerSector;   //ÿ�����ֽ���
	DWORD dwClusterSectorNum;   //ÿ��������
	DWORD dwFatSectorStart;     //fat1��ʼ������
	DWORD dwFatSectorNum;       //fat������
	DWORD dwDirStartClusterNum; //��Ŀ¼��ʼ�غ�
	DWORD dwDataSectorStart;    //����������ʼ������
	DWORD dwByteNumPerCluster;  //ÿ�ص��ֽ���
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
	DWORD dwStartSector;    //��������ʼ������
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