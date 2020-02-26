#include "stdio.h"
#include <cstring>
#include "http_interface.h"
#include "./include/curl/curl.h"
#include <string>
using namespace std;
//#define CURL_GET_TEST
//#define CURL_POST_TEST
#define CURL_POST_FILE
int main()
{
#ifdef CURL_GET_TEST	
	//get 请求
	printf("[curl get test] start ...\n");
    string str_get_req = "www.baidu.com";
    string str_get_res = "";
    int ret = http_get_request(str_get_req,str_get_res);
    printf("CURLE_OK %d ret %d str_res %s",CURLE_OK,ret,str_get_res.c_str());
    if(CURLE_OK != ret)
    {
        const char * cmsg = curl_easy_strerror((CURLcode)ret);
        printf("Auth Failed, send_req_and_get_res ret:%d, %s\n", ret, cmsg?cmsg:"");
        return ret;
    }
	printf("[curl get test] end ...\n");
#endif
#ifdef CURL_POST_TEST	
	//post 请求  测试请求csdn登录
	string str_post_req = "https://passport.csdn.net/v1/register/pc/login/doLogin";
	string str_post_res = "";
	string str_post_data = "{\"loginType\":\"1\",\"pwdOrVerifyCode\":\"weqeqwe\",\"userIdentification\":\"23235123523\"}";
	printf("[curl post test] start ...\n");
    int ret = http_post_request(str_post_req, str_post_data, str_post_res);
    printf("CURLE_OK %d ret %d str_res %s",CURLE_OK,ret,str_post_res.c_str());
    if(CURLE_OK != ret)
    {
        const char * cmsg = curl_easy_strerror((CURLcode)ret);
        printf("Auth Failed, send_req_and_get_res ret:%d, %s\n", ret, cmsg?cmsg:"");
        return ret;
    }
	printf("[curl post test] end ...\n");
#endif
#ifdef CURL_POST_FILE	
	//post 请求  应该要编写对应的服务端?????
	string str_req = "ftp://192.168.1.129/";
	string str_res ="";
	char object_name[32] = "test2020";
	char file_name[256] = "test2020";
	int ret = http_post_file_request(str_req, object_name, file_name, str_res);
	char szRespUTF8[1024] = {0};
	char szRespGB[1024] = {0};
	strcpy(szRespUTF8, str_res.c_str());
	UTF8_To_GB2312_Upload(szRespUTF8, strlen(szRespUTF8), szRespGB, sizeof(szRespGB));

	printf("http upload pic res: %s", szRespGB);

	if(CURLE_OK != ret)
	{
		const char * cmsg = curl_easy_strerror((CURLcode)ret);
		printf("http upload pic Failed, ret:%d, %s\n", ret, cmsg?cmsg:"");
		if(ret==CURLE_FAILED_INIT || ret==CURLE_COULDNT_RESOLVE_PROXY || ret==CURLE_COULDNT_RESOLVE_HOST ||
			ret==CURLE_COULDNT_CONNECT || ret==CURLE_HTTP_RETURNED_ERROR || ret==CURLE_OPERATION_TIMEDOUT ||
			ret==CURLE_SEND_ERROR || ret==CURLE_RECV_ERROR || ret==CURLE_SEND_FAIL_REWIND || ret==CURLE_LOGIN_DENIED)
		{
			ret = -9998;	//一直重试
		}
		else if(ret==CURLE_PARTIAL_FILE || ret==CURLE_FTP_COULDNT_RETR_FILE || ret==CURLE_FILE_COULDNT_READ_FILE)
		{
			ret = -9999;	//文件问题，3次
		}
		return ret;
	}
#endif


}

