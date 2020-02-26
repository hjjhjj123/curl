#include "http_interface.h"
#include <curl/curl.h>
#include "stdio.h"
#include <cstring>

/* 将响应写入指定buffer的回调函数 */
static size_t callback_write(char *ptr, int size, size_t nmemb, void *userdata)
{
	if (userdata && ptr)
	{
		string *buffer = static_cast<string *>(userdata);
		(*buffer).append((char *)ptr, size * nmemb);
	}

	return (size * nmemb);
}

int http_post_request(const string &url_complete, const string str_data, string &str_res)
{
	CURLcode   code = CURLE_FAILED_INIT;
	CURL       *curl = NULL;
	curl_slist *plist = NULL;
	long int http_rsp_code = -1;

	curl = curl_easy_init();
	if(curl == NULL)
	{
		printf("curl_easy_init == NULL!");
		return CURLE_FAILED_INIT ;
	}

	// 打印内部消息
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// 设置http发送的内容类型为JSON
	plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");

	// 当数据大于1024字节时，会自动在请求头加入Expect:100-continue,导致请求挂起，得不到目的服务器的响应.在请求头部 手动设置Expect:为空
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// 用于多线程时安全设置,以防因为超时设置导致的crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// 在完成交互以后强迫断开连接，不能重用
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL允许执行的最长秒数，如果秒内数据未执行完，直接退出
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	// 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// 设置要POST的JSON数据 pass in a pointer to the data - libcurl will not copy
	//if (!str_data.empty())// 没有内容也必须设置数据，否则POST阻塞
	{
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
		curl_off_t upload_size = str_data.size();   // size of the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, upload_size);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
	}

	// 获取响应的数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//获得http返回的状态码
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_rsp_code);
	if (http_rsp_code != 200)
	{
		printf("RESPONSE_CODE: %ld!\n",http_rsp_code);
		code = CURLE_HTTP_RETURNED_ERROR;
	}

ENDER:
	curl_easy_cleanup(curl);
	if (plist)
	{
		curl_slist_free_all(plist);
		plist = NULL;
	}
	curl = NULL;

	return code;
}

int http_get_request(const string &url_complete, string &str_res)
{
	CURLcode   code = CURLE_FAILED_INIT;
	CURL       *curl = NULL;
	curl_slist *plist = NULL;
	long int http_rsp_code = -1;

	curl = curl_easy_init();
	if(curl == NULL)
	{
		printf("curl_easy_init == NULL!");
		return CURLE_FAILED_INIT ;
	}

	// 打印内部消息
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// 设置http发送的内容类型为JSON
	plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");

	// 当数据大于1024字节时，会自动在请求头加入Expect:100-continue,导致请求挂起，得不到目的服务器的响应.在请求头部 手动设置Expect:为空
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// 用于多线程时安全设置,以防因为超时设置导致的crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// 在完成交互以后强迫断开连接，不能重用
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL允许执行的最长秒数，如果秒内数据未执行完，直接退出
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	// 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);


	// 获取响应的数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//获得http返回的状态码
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_rsp_code);
	if (http_rsp_code != 200)
	{
		printf("RESPONSE_CODE: %ld!\n",http_rsp_code);
		code = CURLE_HTTP_RETURNED_ERROR;
	}

ENDER:
	curl_easy_cleanup(curl);
	if (plist)
	{
		curl_slist_free_all(plist);
		plist = NULL;
	}
	curl = NULL;

	return code;
}

int http_post_formdata_request(const string &url_complete, const string str_data, string &str_res)
{
	CURLcode   code = CURLE_FAILED_INIT;
	CURL       *curl = NULL;
	curl_slist *plist = NULL;
	long int http_rsp_code = -1;

	curl = curl_easy_init();
	if(curl == NULL)
	{
		printf("curl_easy_init == NULL!");
		return CURLE_FAILED_INIT ;
	}

	// 打印内部消息
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// 设置http发送的内容类型为JSON
	//plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");
	plist = curl_slist_append(plist, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");

	// 当数据大于1024字节时，会自动在请求头加入Expect:100-continue,导致请求挂起，得不到目的服务器的响应.在请求头部 手动设置Expect:为空
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// 用于多线程时安全设置,以防因为超时设置导致的crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// 在完成交互以后强迫断开连接，不能重用
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL允许执行的最长秒数，如果秒内数据未执行完，直接退出
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	// 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// 设置要POST的JSON数据 pass in a pointer to the data - libcurl will not copy
	//if (!str_data.empty())// 没有内容也必须设置数据，否则POST阻塞
	{
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
		curl_off_t upload_size = str_data.size();   // size of the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, upload_size);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
	}

	// 获取响应的数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//获得http返回的状态码
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_rsp_code);
	if (http_rsp_code != 200)
	{
		printf("RESPONSE_CODE: %ld!\n",http_rsp_code);
		code = CURLE_HTTP_RETURNED_ERROR;
	}

ENDER:
	curl_easy_cleanup(curl);
	if (plist)
	{
		curl_slist_free_all(plist);
		plist = NULL;
	}
	curl = NULL;

	return code;
}

int http_post_file_request(const string &url_complete, const char *name, const char *path, string &str_res)
{
	CURLcode   code = CURLE_FAILED_INIT;
	CURL       *curl = NULL;	
	long int http_rsp_code = -1;
	struct curl_httppost *formpost = NULL;
  	struct curl_httppost *lastptr = NULL;

	curl = curl_easy_init();
	if(curl == NULL)
	{
		printf("curl_easy_init == NULL!");
		return CURLE_FAILED_INIT ;
	}

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}
	//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "user",CURLFORM_COPYCONTENTS,"houjunjie",CURLFORM_END);
	//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "password",CURLFORM_COPYCONTENTS,"hjj",CURLFORM_END);
	//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME,  "file", CURLFORM_FILE, path,CURLFORM_FILENAME, name, CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file",CURLFORM_FILE, path,CURLFORM_END);
	// 用于多线程时安全设置,以防因为超时设置导致的crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// 在完成交互以后强迫断开连接，不能重用
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


	// 获取响应的数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//获得http返回的状态码
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_rsp_code);
	if (http_rsp_code != 200)
	{
		printf("RESPONSE_CODE: %ld!\n",http_rsp_code);
		code = CURLE_HTTP_RETURNED_ERROR;
	}

ENDER:
	curl_easy_cleanup(curl);

	if(formpost)
	{
		curl_formfree(formpost);
	}

	curl = NULL;

	return code;
}


#include "iconv.h"

int GB2312_To_UTF8_Upload(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	int iRet = SUCCESS;
	char **pin = &inbuf;
	char **pout = &outbuf;
	iconv_t cd;
	size_t iconsize;

	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	cd = iconv_open("UTF-8", "GBK");
	if(cd == (iconv_t)-1) 
	{
		perror("iconv_open GB2312->UTF-8 failed");
		strcpy(outbuf, inbuf);
		return FAILURE;
	}

	memset(outbuf, 0, outlen);
	iconsize = iconv(cd, pin, &inlen, pout, &outlen);
	if(iconsize == -1)
	{
		perror("iconv failed:\n");
		strcpy(outbuf, inbuf);
		iRet = FAILURE;
	}

	iconv_close(cd);

	return iRet;
}

int UTF8_To_GB2312_Upload(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	int iRet = SUCCESS;
	char **pin = &inbuf;
	char **pout = &outbuf;
	iconv_t cd;
	size_t iconsize;

	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	cd = iconv_open("GBK","UTF-8");
	if(cd == (iconv_t)-1) 
	{
		perror("iconv_open UTF-8->GB2312 failed");
		strcpy(outbuf, inbuf);
		return FAILURE;
	}


	memset(outbuf, 0, outlen);
	iconsize = iconv(cd, pin, &inlen, pout, &outlen);
	if(iconsize == -1)
	{
		perror("iconv failed:\n");
		strcpy(outbuf, inbuf);
		iRet = FAILURE;
	}

	iconv_close(cd);

	return iRet;
}
