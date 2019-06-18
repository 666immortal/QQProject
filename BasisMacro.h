#ifndef _BASIS_MACRO_
#define _BASIS_MACRO_

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define SUCCESSFUL 0
#define FAILURE    1

// 消息的最大长度
#define DETAILS_LEN    200
// 名称的最大长度
#define NAME_MAX_LEN   20
// 密码的最大长度
#define PWD_MAX_LEN    20
// 在线人数最大数目
#define USER_MAX_NUM 10

typedef int Status;

// 字符串结构体
typedef char* string;

#endif