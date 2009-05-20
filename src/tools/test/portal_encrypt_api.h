/*
 * 	portal_encrypt.h: ����API
 * 
 *      ��Ҫ�Ŀ�:libcrypt.a,libtrpc_sys.a
 * 
 * 
 */

#ifndef _PORTAL_ENCRYPT_API2_H_
#define _PORTAL_ENCRYPT_API2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "payportal_def_base.h"

typedef struct _SHMKey
{
	char szServiceCode[MAX_PROFILE_LEN];      //ҵ����
	char szKey[MAX_PROFILE_LEN];                    //����key
	char szAppIP[MAX_PROFILE_LEN];                //�����IP

	char szSessionType  [MAX_PROFILE_LEN];           //�ͻ��˵�½����
	char szAppServerType[MAX_PROFILE_LEN];  //ҵ��˷�������(server/cgi)
	char szAppServerIP[MAX_PROFILE_LEN];      //ҵ���server IP
	int iAppServerPort;                                       //ҵ���server �˿�

	char szAccessMode[MAX_PROFILE_LEN];           //ҵ��ǰ̨�Ľ��뷽ʽ
	char szQQAcctAppID[MAX_PROFILE_LEN];         //ҵ���ڸ����ʻ������ı���
	char szAppVirtualHost[MAX_PROFILE_LEN];       //ҵ���̨��������
	char szAppPrcingCgi[MAX_PROFILE_LEN];         //ҵ���̨����CGI
	char szAppProvideCgi[MAX_PROFILE_LEN];       //ҵ���̨����CGI   
	char szServiceType[MAX_PROFILE_LEN];       //ҵ������� once ������֧�� month�����·���
	char szVnetAppID[MAX_PROFILE_LEN];       //Vnet��APPID 

	int iPayAmtMax;       //��������
	int iPayAmtMin;       //��������
	int iPayAmtMonth;       //���½��

}SHMKey;

extern int Decrypt_Ex(const char *key,const char *pInput3DES,  char *pOutputCGIParameter, int iOutputLen, char *pErrMsg);

#ifdef __cplusplus
}
#endif

#endif
