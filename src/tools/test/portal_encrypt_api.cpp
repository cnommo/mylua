/* $Id: portal_encrypt_api.cpp,v 1.3 2008/01/16 02:13:32 harlylei Exp $
 *    This is unpublished proprietary
 *    source code of Tencent Ltd.
 *    The copyright notice above does not
 *    evidence any actual or intended
 *    publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 *
 *
 *    FILE:     
 *    DESCRIPTION: 
 *         
 *    AUTHOR:
 *          PATER    - Develop for portal
 *    MODIFIED:
 *
 */




#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <openssl/des.h>

#include "portal_encrypt_api.h"
#include "payportal_def_base.h"
#include "portal_base.h"
#include "parsepara.h"
#include "base64.h"
#include "portal_3des.h"
#include "get_portalkey_xml.h"
//#include "debug_log.h"

int Encrypt_Ex(const char *Key, const char *AppIP, const char *pInput3DES, char *pOutputCGIParameter, int iOutputLen, char *pErrMsg)
{
	char szKey[MAX_CGIPARA_LEN];
	int iRet, iLen, iInputLen, iLenAfterEncrypt;
	char szOutTmp[iOutputLen];
	char szOutTmp2[iOutputLen];
	char szInputBuf[iOutputLen];

	memset(pOutputCGIParameter, 0x0, iOutputLen);
	memset(szOutTmp, 0x0, sizeof(szOutTmp));
	memset(szOutTmp2, 0x0, sizeof(szOutTmp2));
	memset(szInputBuf, 0x0, sizeof(szInputBuf));
	
	//pre operator
	char sKey[MAX_CGIPARA_LEN]={0};
	strncpy(sKey, Key, MAX_CGIPARA_LEN);
	if ((iLen = strlen(sKey)) < 24)
	{
		for (int i = iLen;i < 24;i++)
			sKey[i] = '0';
	}
	if ((iRet = Create3DesKey(sKey, szKey, sizeof(szKey), pErrMsg)) != PAYPORTAL_OK)
	{
		sprintf(pErrMsg, "Create3DesKey fail [key=%s]", sKey);
		return iRet;
	}

	iInputLen =strlen(pInput3DES) + strlen("&AppIP=") + strlen(AppIP);
	if (iInputLen> iOutputLen)
	{
		strcpy(pErrMsg, "require more output buffer");
		return ENCRYPE_FAIL;
	}
	
	strcpy(szInputBuf, pInput3DES);
	strcat(szInputBuf, "&AppIP=");
	strcat(szInputBuf, AppIP);

	if ((iRet = Portal_des_encrypt(szKey, (unsigned char *)szInputBuf, (unsigned char *)szOutTmp, iInputLen, iOutputLen, &iLenAfterEncrypt, DES_ENCRYPT,pErrMsg)) != PAYPORTAL_OK)
	{
		strcat(pErrMsg, "Portal_des_encrypt err");
		return iRet;
	}
	if ((iRet = Base64_Encode((const unsigned char *)szOutTmp, iLenAfterEncrypt, szOutTmp2, iOutputLen, &iLenAfterEncrypt))!= 0)
	{
		sprintf(pErrMsg, "Base64_Encode err [Ret=%d]", iRet);
		return ENCRYPE_FAIL;
	}
	encode_hexstring(szOutTmp2, strlen(szOutTmp2), pOutputCGIParameter);
	return PAYPORTAL_OK;
}

int Decrypt_Ex(const char *Key, const char *pInput3DES, char *pOutputCGIParameter, int iOutputLen, char *pErrMsg)
{
      char szKey[MAX_CGIPARA_LEN]={0};
      int  iRet, iLen, iLenAfterDecrypt;
      char szInput3DES[iOutputLen], szOutTmp[iOutputLen];
	  
	  //pre operator
	  char sKey[MAX_CGIPARA_LEN]={0};
	  strncpy(sKey, Key, MAX_CGIPARA_LEN);
	  if ((iLen = strlen(sKey)) < 24)
	  {
		  for (int i = iLen;i < 24;i++)
			  sKey[i] = '0';
	  }
	  if ((iRet = Create3DesKey(sKey, szKey, sizeof(szKey), pErrMsg) != PAYPORTAL_OK))
	  {
		  sprintf(pErrMsg, "Create3DesKey fail [key=%s]", sKey);
		  return iRet;
	  }
 
      //解密前解码
	  memset(szInput3DES, 0x0, sizeof(szInput3DES));
	  decode_hexstring(pInput3DES, szInput3DES, &iLen);
	  
      if ((iRet = Base64_Decode(szInput3DES,  strlen(szInput3DES), (unsigned char *)szOutTmp, iOutputLen, &iLen)) != 0)
	  {
		  sprintf(pErrMsg, "Base64_Decode err[Ret=%d]", iRet);	  	
		  return PORTAL_ENCRYPT_FAIL;
      }	  
	  //进行3DES解密  
      if ((iRet = Portal_des_encrypt(szKey, (const unsigned char *)szOutTmp, (unsigned char *)pOutputCGIParameter,iLen,  iOutputLen, &iLenAfterDecrypt, DES_DECRYPT, pErrMsg)) != PAYPORTAL_OK)
	  {
		  strcat(pErrMsg, "Portal_des_encrypt err");
		  return PORTAL_ENCRYPT_FAIL;
      }
      return PAYPORTAL_OK;
}
