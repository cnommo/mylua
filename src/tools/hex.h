#ifndef _HEX_H_
#define _HEX_H_

#ifdef __cplusplus
extern "C" {
#endif

int String_Encode(const char* s, char** sEncoded);
int String_Decode(const char* s, char** sDecoded);
int HexString_Encode(const char* s, char** sEncoded);
int HexString_Decode(const char* s, char** sDecoded);

#ifdef __cplusplus
}
#endif

#endif
