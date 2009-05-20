#ifndef _BASE64_H
#define _BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------
���ܣ���һ�����ݱ����BASE64
������input ---�����������
input_len --- ����������ݿ鳤�ȡ�
output --- ������Ž���Ļ�����
output_siz ---������output�ĳ���
result_length --- ���������ݿ鳤�ȡ�
����ֵ��0 �ɹ�
        -1ʧ��
ע�����������ݳ���Ϊ��((input_len+2)/3)*4  
---------------------------------------------------------------*/
int Base64_Encode(const unsigned char* input, int input_len, char* output, int output_size, int* result_length);


/*-------------------------------------------------------------
���ܣ���һ��BASE64��������ݽ���
������input ---�����������
��ʽ��
input_len --- ����������ݿ鳤�ȡ�
output --- ������Ž�������ݵĻ�����
output_size ---������output�ĳ���
result_length --- ���������ݿ鳤�ȡ�
����ֵ��0 �ɹ�
        -1ʧ��
ע������ÿ��Group�п��ܺ���1��3�ֽڡ� ���Խ��������ݳ��Ȳ�һ����
    ���ÿ��Group�ж���3���ֽ����ݣ�����������ݳ���Ϊ��
	(input_len*3)/4.
---------------------------------------------------------------*/
int Base64_Decode(const char* input, int input_len, unsigned char* output, int output_size, int* result_length);

#ifdef __cplusplus
}
#endif

#endif
