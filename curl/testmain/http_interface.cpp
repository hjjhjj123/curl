#include "http_interface.h"
#include <curl/curl.h>
#include "stdio.h"
#include <cstring>

/* ����Ӧд��ָ��buffer�Ļص����� */
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

	// ��ӡ�ڲ���Ϣ
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// ����http���͵���������ΪJSON
	plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");

	// �����ݴ���1024�ֽ�ʱ�����Զ�������ͷ����Expect:100-continue,����������𣬵ò���Ŀ�ķ���������Ӧ.������ͷ�� �ֶ�����Expect:Ϊ��
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// ���ڶ��߳�ʱ��ȫ����,�Է���Ϊ��ʱ���õ��µ�crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// ����ɽ����Ժ�ǿ�ȶϿ����ӣ���������
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL����ִ�е�������������������δִ���ֱ꣬���˳�
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	// ���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// ����ҪPOST��JSON���� pass in a pointer to the data - libcurl will not copy
	//if (!str_data.empty())// û������Ҳ�����������ݣ�����POST����
	{
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
		curl_off_t upload_size = str_data.size();   // size of the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, upload_size);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
	}

	// ��ȡ��Ӧ������
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//���http���ص�״̬��
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

	// ��ӡ�ڲ���Ϣ
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// ����http���͵���������ΪJSON
	plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");

	// �����ݴ���1024�ֽ�ʱ�����Զ�������ͷ����Expect:100-continue,����������𣬵ò���Ŀ�ķ���������Ӧ.������ͷ�� �ֶ�����Expect:Ϊ��
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// ���ڶ��߳�ʱ��ȫ����,�Է���Ϊ��ʱ���õ��µ�crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// ����ɽ����Ժ�ǿ�ȶϿ����ӣ���������
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL����ִ�е�������������������δִ���ֱ꣬���˳�
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	// ���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);


	// ��ȡ��Ӧ������
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//���http���ص�״̬��
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

	// ��ӡ�ڲ���Ϣ
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	code = curl_easy_setopt(curl, CURLOPT_URL, url_complete.c_str());
	if(code != CURLE_OK)
	{
		printf("curl_easy_setopt CURLOPT_URL failed, %s!",curl_easy_strerror(code));
		goto ENDER;
	}

	// ����http���͵���������ΪJSON
	//plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");
	plist = curl_slist_append(plist, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");

	// �����ݴ���1024�ֽ�ʱ�����Զ�������ͷ����Expect:100-continue,����������𣬵ò���Ŀ�ķ���������Ӧ.������ͷ�� �ֶ�����Expect:Ϊ��
	plist = curl_slist_append(plist, "Expect:");

	if(NULL == plist)
	{
		printf("curl_slist_append failed, %s!",curl_easy_strerror(code));
		code = CURLE_FAILED_INIT;
		goto ENDER;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

	// ���ڶ��߳�ʱ��ȫ����,�Է���Ϊ��ʱ���õ��µ�crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// ����ɽ����Ժ�ǿ�ȶϿ����ӣ���������
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// cURL����ִ�е�������������������δִ���ֱ꣬���˳�
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	// ���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// ����ҪPOST��JSON���� pass in a pointer to the data - libcurl will not copy
	//if (!str_data.empty())// û������Ҳ�����������ݣ�����POST����
	{
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
		curl_off_t upload_size = str_data.size();   // size of the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, upload_size);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());
	}

	// ��ȡ��Ӧ������
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//���http���ص�״̬��
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
	// ���ڶ��߳�ʱ��ȫ����,�Է���Ϊ��ʱ���õ��µ�crash
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// ����ɽ����Ժ�ǿ�ȶϿ����ӣ���������
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

	// ���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


	// ��ȡ��Ӧ������
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&str_res);

	// Perform the request, res will get the return code
	code = curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		printf("curl_easy_perform failed, code = %d, %s", code, curl_easy_strerror(code));
		goto ENDER;
	}

	//���http���ص�״̬��
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
