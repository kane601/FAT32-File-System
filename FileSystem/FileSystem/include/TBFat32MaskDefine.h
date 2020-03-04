
/**
*
* Copyright (C) 2002-2017成都易我科技有限公司
* All rights reserved.
*
* @file   TBFat32MaskDefine.h
* @author qiaozhousan
* @date   2017-05-15
*
* @brief  Check or offset parameters are defined for parser FAT32 file system
*
*/

#ifndef _TBFat32MaskDefine_H__
#define _TBFat32MaskDefine_H__

//文件分配表中每个簇项可取表项值及含义
#define  CLUSTER_ATTRIBUTE_NOTUSED       0x00000000   //未使用簇
#define  CLUSTER_ATTRIBUTE_FILEEOF       0x0FFFFFFF   //文件结束

#define  CLUSTER_ATTRIBUTE_RESERVE0      0xFFFFFFF0   //保留簇
#define  CLUSTER_ATTRIBUTE_RESERVE1      0xFFFFFFF1  
#define  CLUSTER_ATTRIBUTE_RESERVE2      0xFFFFFFF2   
#define  CLUSTER_ATTRIBUTE_RESERVE3      0xFFFFFFF3   
#define  CLUSTER_ATTRIBUTE_RESERVE4      0xFFFFFFF4   
#define  CLUSTER_ATTRIBUTE_RESERVE5      0xFFFFFFF5   
#define  CLUSTER_ATTRIBUTE_RESERVE6      0xFFFFFFF6  

#define  CLUSTER_ATTRIBUTE_BAD           0xFFFFFFF7   //坏簇
#define  CLUSTER_ATTRIBUTE_EOF8          0xFFFFFFF8   //文件结束簇
#define  CLUSTER_ATTRIBUTE_EOF9          0xFFFFFFF9 
#define  CLUSTER_ATTRIBUTE_EOFA          0xFFFFFFFA   
#define  CLUSTER_ATTRIBUTE_EOFB          0xFFFFFFFB 
#define  CLUSTER_ATTRIBUTE_EOFC          0xFFFFFFFC   
#define  CLUSTER_ATTRIBUTE_EOFD          0xFFFFFFFD  
#define  CLUSTER_ATTRIBUTE_EOFE          0xFFFFFFFE   
#define  CLUSTER_ATTRIBUTE_EOFF          0xFFFFFFFF  

//文件目录项特殊判断参数
#define  FDT_END              0x00      //结束项
#define  FDT_VOLUME           0x08      //卷标项
#define  FDT_LONG             0x0F      //长文件名项
#define  FDT_CUR_DIR          0x2E      //当前目录
#define  FDT_DELETE           0xE5      //删除项
#define  FDT_PARENT_DIR       0x2E2E    //上一级目录

//BPB偏移参数
#define  OFFSET_SECTOR_NUM          0x0B    //扇区字节数 偏移0BH 0CH
#define  OFFSET_CLUSTER_NUM         0x0D    //每簇扇区数 偏移0x0D
#define  OFFSET_RESERVE_CLUSTER_NUM 0x0E    //保留扇区数 偏移0EH 0FH
#define  OFFSET_FAT_SECTOR_NUM      0x24    //FAT表占用扇区数 偏移24H
#define  OFFSET_ROOT_CLUSTER_NUM    0x2C    //根目录入口簇号  偏移2CH

//文件目录项偏移参数
#define  OFFSET_ATTRIBUTE       0x0B    //文件目录项属性
#define  OFFSET_TIME_LOW        0x16    //时间低字节
#define  OFFSET_TIME_HIGH       0x17    //时间高字节
#define  OFFSET_DATE_LOW        0x18    //日期低字节
#define  OFFSET_DATE_HIGH       0x19    //日期高字节

#define  OFFSET_START_SECTOR_HIGH1    0x14 //文件起始簇号高16字节中低8位
#define  OFFSET_START_SECTOR_HIGH2    0x15 //文件起始簇号高16字节中高8位
#define  OFFSET_START_SECTOR_LOW1     0x1A //文件起始簇号低16字节中低8位
#define  OFFSET_START_SECTOR_LOW2     0x1B //文件起始簇号低16字节中高8位

#define  OFFSET_FILE_SIZE1            0x1C //文件大小0-7位
#define  OFFSET_FILE_SIZE2            0x1D //文件大小8-15位
#define  OFFSET_FILE_SIZE3            0x1E //文件大小16-23位
#define  OFFSET_FILE_SIZE4            0x1F //文件大小24-31位

//长文件目录项
#define  OFFSET_LONG_NAME1            0x01 //文件名unicode移位 取10字节
#define  OFFSET_LONG_NAME2            0x0E //文件名unicode移位 取12字节
#define  OFFSET_LONG_NAME3            0x1C //文件名unicode移位 取4字节

#define  LONG_DIR_END                 0x40 //最后一个长文件名目录项

//文件目录项属性字节
#define  FDT_ATTRI_READONLY    0x01
#define  FDT_ATTRI_HIDE        0x02
#define  FDT_ATTRI_SYSTEM      0x04
#define  FDT_ATTRI_VOLUMN      0x08
#define  FDT_ATTRI_DIR         0x10
#define  FDT_ATTRI_ARCHIVE     0x20

//短文件目录项时文件名/扩展名大小写属性
#define  FILE_NAME_UPPER        0x00 //文件名大写 
#define  FILE_NAME_LOEWR        0x08 //文件名小写 
#define  FILE_EXTENSION_LOEWR   0x10 //扩展名小写 

//公共
#define  SECTOR_BUF_LEN         512        //扇区字节数
#define  VALUE_ZERO             0x00       //置零
#define  FILE_LOW_END           0xFFFFFFFF //低序位寻址尾
#define  DIR_LEN                32         //文件目录项长度
#define  SNAME_LEN              8          //短文件项文件名长度
#define  SEXT_LEN               3          //短文件项扩展名长度


//删除指针
#define TBSAFEDELETEPTR(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 
//删除数组
#define TBSAFEDELETEARRAY(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

#endif