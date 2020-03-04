/**
*
* Copyright (C) 2002-2017�ɶ����ҿƼ����޹�˾
* All rights reserved.
*
* @file   TBNTFSDataStruct.h
* @author qiaozhousan
* @date   2017-06-06
*
* @brief  Data structures are used to parse NTFS file systems
*
*/

#ifndef _TBNTFSDataStruct_H__
#define _TBNTFSDataStruct_H__

typedef struct NTFS_BPB
{
	UCHAR jmpCmd[3];
	UCHAR s_ntfs[8];             // "NTFS     " ��־
	// 0x0B
	UCHAR bytesPerSec[2];        //  0x0200�� ������С��512B
	UCHAR SecsPerClu;            //  0x08���� ÿ����������4KB
	UCHAR rsvSecs[2];            // ������������������
	UCHAR noUse01[5];            //
	// 0x15
	UCHAR driveDscrp;            //  0xF8     ���̽��� -- Ӳ��
	UCHAR noUse02[2];            //
	// 0x18
	UCHAR SecsPerTrack[2];       // ��0x003F  ÿ�������� 63
	UCHAR Headers[2];            //�� 0x00FF  ��ͷ��
	UCHAR secsHide[4];           // ��0x3F������������
	UCHAR noUse03[8];            //
	// 0x28
	UCHAR allSecsNum[8];         // ����������, ��λ��ǰ, ��λ�ں�
	// 0x30

	UCHAR MFT_startClu[8];
	UCHAR MFTMirr_startClu[8];
	//0x40
	UCHAR cluPerMFT[4];
	UCHAR cluPerIdx[4];
	//0x48
	UCHAR SerialNum[8];
	UCHAR checkSum[8];

}NtfsBpb, *lpNtfsBpb;

#endif