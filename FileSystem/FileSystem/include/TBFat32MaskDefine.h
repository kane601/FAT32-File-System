
/**
*
* Copyright (C) 2002-2017�ɶ����ҿƼ����޹�˾
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

//�ļ��������ÿ�������ȡ����ֵ������
#define  CLUSTER_ATTRIBUTE_NOTUSED       0x00000000   //δʹ�ô�
#define  CLUSTER_ATTRIBUTE_FILEEOF       0x0FFFFFFF   //�ļ�����

#define  CLUSTER_ATTRIBUTE_RESERVE0      0xFFFFFFF0   //������
#define  CLUSTER_ATTRIBUTE_RESERVE1      0xFFFFFFF1  
#define  CLUSTER_ATTRIBUTE_RESERVE2      0xFFFFFFF2   
#define  CLUSTER_ATTRIBUTE_RESERVE3      0xFFFFFFF3   
#define  CLUSTER_ATTRIBUTE_RESERVE4      0xFFFFFFF4   
#define  CLUSTER_ATTRIBUTE_RESERVE5      0xFFFFFFF5   
#define  CLUSTER_ATTRIBUTE_RESERVE6      0xFFFFFFF6  

#define  CLUSTER_ATTRIBUTE_BAD           0xFFFFFFF7   //����
#define  CLUSTER_ATTRIBUTE_EOF8          0xFFFFFFF8   //�ļ�������
#define  CLUSTER_ATTRIBUTE_EOF9          0xFFFFFFF9 
#define  CLUSTER_ATTRIBUTE_EOFA          0xFFFFFFFA   
#define  CLUSTER_ATTRIBUTE_EOFB          0xFFFFFFFB 
#define  CLUSTER_ATTRIBUTE_EOFC          0xFFFFFFFC   
#define  CLUSTER_ATTRIBUTE_EOFD          0xFFFFFFFD  
#define  CLUSTER_ATTRIBUTE_EOFE          0xFFFFFFFE   
#define  CLUSTER_ATTRIBUTE_EOFF          0xFFFFFFFF  

//�ļ�Ŀ¼�������жϲ���
#define  FDT_END              0x00      //������
#define  FDT_VOLUME           0x08      //�����
#define  FDT_LONG             0x0F      //���ļ�����
#define  FDT_CUR_DIR          0x2E      //��ǰĿ¼
#define  FDT_DELETE           0xE5      //ɾ����
#define  FDT_PARENT_DIR       0x2E2E    //��һ��Ŀ¼

//BPBƫ�Ʋ���
#define  OFFSET_SECTOR_NUM          0x0B    //�����ֽ��� ƫ��0BH 0CH
#define  OFFSET_CLUSTER_NUM         0x0D    //ÿ�������� ƫ��0x0D
#define  OFFSET_RESERVE_CLUSTER_NUM 0x0E    //���������� ƫ��0EH 0FH
#define  OFFSET_FAT_SECTOR_NUM      0x24    //FAT��ռ�������� ƫ��24H
#define  OFFSET_ROOT_CLUSTER_NUM    0x2C    //��Ŀ¼��ڴغ�  ƫ��2CH

//�ļ�Ŀ¼��ƫ�Ʋ���
#define  OFFSET_ATTRIBUTE       0x0B    //�ļ�Ŀ¼������
#define  OFFSET_TIME_LOW        0x16    //ʱ����ֽ�
#define  OFFSET_TIME_HIGH       0x17    //ʱ����ֽ�
#define  OFFSET_DATE_LOW        0x18    //���ڵ��ֽ�
#define  OFFSET_DATE_HIGH       0x19    //���ڸ��ֽ�

#define  OFFSET_START_SECTOR_HIGH1    0x14 //�ļ���ʼ�غŸ�16�ֽ��е�8λ
#define  OFFSET_START_SECTOR_HIGH2    0x15 //�ļ���ʼ�غŸ�16�ֽ��и�8λ
#define  OFFSET_START_SECTOR_LOW1     0x1A //�ļ���ʼ�غŵ�16�ֽ��е�8λ
#define  OFFSET_START_SECTOR_LOW2     0x1B //�ļ���ʼ�غŵ�16�ֽ��и�8λ

#define  OFFSET_FILE_SIZE1            0x1C //�ļ���С0-7λ
#define  OFFSET_FILE_SIZE2            0x1D //�ļ���С8-15λ
#define  OFFSET_FILE_SIZE3            0x1E //�ļ���С16-23λ
#define  OFFSET_FILE_SIZE4            0x1F //�ļ���С24-31λ

//���ļ�Ŀ¼��
#define  OFFSET_LONG_NAME1            0x01 //�ļ���unicode��λ ȡ10�ֽ�
#define  OFFSET_LONG_NAME2            0x0E //�ļ���unicode��λ ȡ12�ֽ�
#define  OFFSET_LONG_NAME3            0x1C //�ļ���unicode��λ ȡ4�ֽ�

#define  LONG_DIR_END                 0x40 //���һ�����ļ���Ŀ¼��

//�ļ�Ŀ¼�������ֽ�
#define  FDT_ATTRI_READONLY    0x01
#define  FDT_ATTRI_HIDE        0x02
#define  FDT_ATTRI_SYSTEM      0x04
#define  FDT_ATTRI_VOLUMN      0x08
#define  FDT_ATTRI_DIR         0x10
#define  FDT_ATTRI_ARCHIVE     0x20

//���ļ�Ŀ¼��ʱ�ļ���/��չ����Сд����
#define  FILE_NAME_UPPER        0x00 //�ļ�����д 
#define  FILE_NAME_LOEWR        0x08 //�ļ���Сд 
#define  FILE_EXTENSION_LOEWR   0x10 //��չ��Сд 

//����
#define  SECTOR_BUF_LEN         512        //�����ֽ���
#define  VALUE_ZERO             0x00       //����
#define  FILE_LOW_END           0xFFFFFFFF //����λѰַβ
#define  DIR_LEN                32         //�ļ�Ŀ¼���
#define  SNAME_LEN              8          //���ļ����ļ�������
#define  SEXT_LEN               3          //���ļ�����չ������


//ɾ��ָ��
#define TBSAFEDELETEPTR(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 
//ɾ������
#define TBSAFEDELETEARRAY(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

#endif