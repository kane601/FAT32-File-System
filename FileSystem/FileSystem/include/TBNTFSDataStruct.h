/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
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
	UCHAR s_ntfs[8];             // "NTFS     " 标志
	// 0x0B
	UCHAR bytesPerSec[2];        //  0x0200　 扇区大小，512B
	UCHAR SecsPerClu;            //  0x08　　 每簇扇区数，4KB
	UCHAR rsvSecs[2];            // 　　　　　保留扇区
	UCHAR noUse01[5];            //
	// 0x15
	UCHAR driveDscrp;            //  0xF8     磁盘介质 -- 硬盘
	UCHAR noUse02[2];            //
	// 0x18
	UCHAR SecsPerTrack[2];       // 　0x003F  每道扇区数 63
	UCHAR Headers[2];            //　 0x00FF  磁头数
	UCHAR secsHide[4];           // 　0x3F　　隐藏扇区
	UCHAR noUse03[8];            //
	// 0x28
	UCHAR allSecsNum[8];         // 卷总扇区数, 高位在前, 低位在后
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