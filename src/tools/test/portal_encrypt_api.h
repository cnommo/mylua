/*
 * 	portal_encrypt.h: 加密API
 * 
 *      需要的库:libcrypt.a,libtrpc_sys.a
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
	char szServiceCode[MAX_PROFILE_LEN];      //业务码
	char szKey[MAX_PROFILE_LEN];                    //加密key
	char szAppIP[MAX_PROFILE_LEN];                //请求端IP

	char szSessionType  [MAX_PROFILE_LEN];           //客户端登陆类型
	char szAppServerType[MAX_PROFILE_LEN];  //业务端服务类型(server/cgi)
	char szAppServerIP[MAX_PROFILE_LEN];      //业务端server IP
	int iAppServerPort;                                       //业务端server 端口

	char szAccessMode[MAX_PROFILE_LEN];           //业务前台的接入方式
	char szQQAcctAppID[MAX_PROFILE_LEN];         //业务在个人帐户侧分配的编码
	char szAppVirtualHost[MAX_PROFILE_LEN];       //业务后台虚拟主机
	char szAppPrcingCgi[MAX_PROFILE_LEN];         //业务后台批价CGI
	char szAppProvideCgi[MAX_PROFILE_LEN];       //业务后台发货CGI   
	char szServiceType[MAX_PROFILE_LEN];       //业务的类型 once ：单笔支付 month：包月服务
	char szVnetAppID[MAX_PROFILE_LEN];       //Vnet的APPID 

	int iPayAmtMax;       //批价上限
	int iPayAmtMin;       //批价下限
	int iPayAmtMonth;       //包月金额

}SHMKey;

extern int Decrypt_Ex(const char *key,const char *pInput3DES,  char *pOutputCGIParameter, int iOutputLen, char *pErrMsg);

#ifdef __cplusplus
}
#endif

#endif
