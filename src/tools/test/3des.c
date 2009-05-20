/* 
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

#include "payportal_def_base.h"

void XorEncrypt(char *pInput, int iLen)
{
    int iLenPwd = 0, iPos = 0, j = 0;
    const char *pPwd = "dj@#kl$15*";
	
    iLenPwd = strlen(pPwd);
    while (iPos < iLen)
	{
       *(pInput + iPos) = *(pInput + iPos)^pPwd[j>=iLenPwd?j=0:j++];/*异或*/
       iPos++;
    }
}




static int StringKeyToDesKey(const char *pKey, des_cblock key1, des_cblock key2, des_cblock key3)
{
  char szNumeric[4], *p = szNumeric;//bugfix haitaochen 20080118 缓冲区不够,缺少结束符
  int iNumCount = 0, iDone = -1;

  memset (szNumeric, 0x0, sizeof (szNumeric));
  memset (key1, 0x0, sizeof (des_cblock));
  memset (key2, 0x0, sizeof (des_cblock));
  memset (key3, 0x0, sizeof (des_cblock));

  while (*pKey)
  {

    if (isdigit (*pKey))
	{
	  iNumCount ++;
	  if (iNumCount > 3)
	  {
	      return -1;
	  }
	  *p++ = *pKey;
	}
    else if(*pKey == ',')
	{
	      iNumCount = 0;
	      iDone++;

	     switch (iDone / 8)
		 {
	     	case 0:
		      key1[iDone % 8] = atoi(szNumeric);
		      break;
		    case 1:
		      key2[iDone % 8] = atoi(szNumeric);
		      break;
		    case 2:
		      key3[iDone % 8] = atoi(szNumeric);
		      break;

		 }
	      memset (szNumeric, 0x0, sizeof (szNumeric));
	      p = szNumeric;

	}

    pKey++;
  }
  if (iDone == 22){
      key3[7] = atoi(szNumeric);
  }
  return PAYPORTAL_OK;
}


#ifdef NEW_SSL_VERSION

//3//DES加密函数
int Portal_des_encrypt(const char *pKey, const unsigned char *pInput, unsigned char *pOutput,  int iInputLen, int iOutputLen, int *iLenAfterEncrypt, int enc, char *pErrMsg)
{
  DES_cblock key1, key2, key3;
  DES_cblock blockInput, blockOutput;
  DES_key_schedule sched1,sched2,sched3;
  int iBlockCount = 0;
  const unsigned char *pInputPos;
  unsigned char  *pOutputPos; 



  //检查输出缓冲是否足够   
  iBlockCount = iInputLen / sizeof(des_cblock);
  if (iInputLen % sizeof(des_cblock) >0)
     iBlockCount++;

  if (iBlockCount > iOutputLen/sizeof(des_cblock)){
    strcpy(pErrMsg, "Output string lenghth is not enough\n");    
    return ENCRYPE_ERR_OUTPUT_NOT_ENOUGH;
  }
  *iLenAfterEncrypt = iBlockCount * sizeof(des_cblock);
  
  memset(pOutput, 0x0, iOutputLen);
  
  if (StringKeyToDesKey(pKey, key1, key2, key3) != PAYPORTAL_OK){
        sprintf(pErrMsg, "Bad format of key[%s]\n", pKey);
        return ENCRYPE_ERR_BAD_PARITY;
  }
  
    switch(DES_set_key_checked(&key1, &sched1))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity1\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak1\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }
    switch(DES_set_key_checked(&key2, &sched2))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity2\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak2\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }

    switch(DES_set_key_checked(&key3, &sched3))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity3\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak3\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }
	
 //每次只加密一块(unsigned char [8])
  pInputPos = pInput;
  pOutputPos = pOutput; 
  while(iBlockCount >0){

    memcpy(blockInput, pInputPos, sizeof(des_cblock));
    pInputPos += sizeof(des_cblock);
    
    DES_ecb3_encrypt(&blockInput, &blockOutput, &sched1, &sched2, &sched3, enc);
    memcpy(pOutputPos, blockOutput, sizeof(des_cblock)); 
    pOutputPos +=  sizeof(des_cblock); 
    iBlockCount--;
  }

  return PAYPORTAL_OK;
}

#else




//3//DES加密函数
int Portal_des_encrypt(const char *pKey, const unsigned char *pInput, unsigned char *pOutput,  int iInputLen, int iOutputLen, int *iLenAfterEncrypt, int enc, char *pErrMsg)
{
  des_cblock key1, key2, key3;
  des_cblock blockInput, blockOutput;
  des_key_schedule sched1,sched2,sched3;
  int iBlockCount = 0;
  const unsigned char *pInputPos;
  unsigned char  *pOutputPos; 



  //检查输出缓冲是否足够   
  iBlockCount = iInputLen / sizeof(des_cblock);
  if (iInputLen % sizeof(des_cblock) >0)
     iBlockCount++;

  if (iBlockCount > iOutputLen/sizeof(des_cblock)){
    strcpy(pErrMsg, "Output string lenghth is not enough\n");    
    return ENCRYPE_ERR_OUTPUT_NOT_ENOUGH;
  }
  *iLenAfterEncrypt = iBlockCount * sizeof(des_cblock);
  
  memset(pOutput, 0x0, iOutputLen);
  
  if (StringKeyToDesKey(pKey, key1, key2, key3) != PAYPORTAL_OK){
        sprintf(pErrMsg, "Bad format of key[%s]\n", pKey);
        return ENCRYPE_ERR_BAD_PARITY;
  }
  
    switch(des_set_key_checked(&key1, sched1))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity1\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak1\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }
    switch(des_set_key_checked(&key2, sched2))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity2\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak2\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }

    switch(des_set_key_checked(&key3, sched3))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity3\n");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak3\n");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }
	
 //每次只加密一块(unsigned char [8])
  pInputPos = pInput;
  pOutputPos = pOutput; 
  while(iBlockCount >0){

    memcpy(blockInput, pInputPos, sizeof(des_cblock));
    pInputPos += sizeof(des_cblock);
    
    des_ecb3_encrypt(&blockInput, &blockOutput, sched1, sched2, sched3, enc);
    
    memcpy(pOutputPos, blockOutput, sizeof(des_cblock)); 
	
    pOutputPos +=  sizeof(des_cblock);
    
    iBlockCount--;
#ifdef ENCRYPT_DEBUG 	
    printf("blockInput=%c%c%c%c%c%c%c%c pOutputPos=%d %d %d %d %d %d %d %d\n",blockInput[0],blockInput[1],blockInput[2],blockInput[3],blockInput[4],blockInput[5],blockInput[6],blockInput[7] , (char)blockOutput[0], (char)blockOutput[1], (char)blockOutput[2], (char)blockOutput[3], (char)blockOutput[4], (char)blockOutput[5], (char)blockOutput[6], (char)blockOutput[7]);  
#endif
  }

  return PAYPORTAL_OK;
}


#endif

int Create3DesKey(const char *pKey, char *pByteKey, int iLen, char *pErrMsg)
{
    des_cblock key1, key2, key3;
    des_key_schedule sched;

    memset (key1, 0x0, sizeof (des_cblock));
    memset (key2, 0x0, sizeof (des_cblock));
    memset (key3, 0x0, sizeof (des_cblock));

    des_string_to_key(pKey, &key1);
    des_string_to_key(pKey + 8, &key2);
    des_string_to_key(pKey + 16, &key3);

    switch(des_set_key_checked(&key1, sched))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity1");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak1");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }
    switch(des_set_key_checked(&key2, sched))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity2");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak2");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }

    switch(des_set_key_checked(&key3, sched))
    {
        case -1:
            strcpy(pErrMsg, "Bad parity3");
            return ENCRYPE_ERR_BAD_PARITY;
            break;
    
        case -2:
            strcpy(pErrMsg, "Key is weak3");
            return ENCRYPE_ERR_WEAK_KEY;
            break;
    }

    snprintf(pByteKey, iLen, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", key1[0],key1[1],key1[2],key1[3],key1[4],key1[5],key1[6],key1[7],key2[0],key2[1],key2[2],key2[3],key2[4],key2[5],key2[6],key2[7],key3[0],key3[1],key3[2],key3[3],key3[4],key3[5],key3[6],key3[7]);
	
    return PAYPORTAL_OK;
}

