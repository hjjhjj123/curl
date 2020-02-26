#ifndef __HTTP_INTERFACE_H__
#define __HTTP_INTERFACE_H__
#include <string>
using namespace std;
/* 错误类型 */
#define FAILURE              -1   /* 错误 */
#define SUCCESS               0   /* 成功 */

int http_post_request(const string &url_complete, const string str_data, string &str_res);
int http_get_request(const string &url_complete, string &str_res);
int http_post_formdata_request(const string &url_complete, const string str_data, string &str_res);
int http_post_file_request(const string &url_complete, const char *name, const char *path, string &str_res);



int GB2312_To_UTF8_Upload(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
int UTF8_To_GB2312_Upload(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
#endif  //__UPLOAD_H__

