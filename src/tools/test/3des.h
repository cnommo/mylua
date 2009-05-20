/* $Id: portal_3des.h,v 1.2 2008/01/16 02:13:32 harlylei Exp $
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


#ifndef _PORTAL_3DES_H
#define _PORTAL_3DES_H

#ifdef __cplusplus
extern "C" {
#endif

extern int Portal_des_encrypt(const char *pKey, const unsigned char *pInput, unsigned char *pOutput,  int iInputLen, int iOutputLen, int *iLenAfterEncrypt, int enc , char *pErrMsg);
extern void XorEncrypt(char *pInput, int iLen);
extern int Create3DesKey(const char *pKey, char *pByteKey, int iLen, char *pErrMsg);

#ifdef __cplusplus
}
#endif

#endif

