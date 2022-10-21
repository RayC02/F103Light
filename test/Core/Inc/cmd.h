#ifndef __CMD_H_
#define __CMD_H_
#include "main.h"
#include "car.h"
#define 	ARRAY_SIZE(x)	(sizeof(x) / (sizeof((x)[0])))      //用来计算结构体数组中，数组的个数。个数=结构体总长度/单个数组长度
#define		MAX_CMD_LENGTH		15	               							// 最大命令名长度
#define		MAX_CMDS_COUNT		20	                						// 最大命令数
typedef void ( *functions )(void);  	                    // 命令操作函数指针类型
																														//命令结构体类型 用于存储字符串命令和对应函数
typedef struct
{
    char cmd_name[MAX_CMD_LENGTH + 1];        							// 命令名 字符串末尾系统会自动添加结束符'/0'       sizeof("name")大小为 10
    functions cmd_functions;			  	        							// 命令操作函数     sizeof(func) 大小为 2
}CMD_Name_Func;
																														// 命令列表结构体类型  用于存储字符串命令数组
typedef struct
{
    CMD_Name_Func cmdNames[MAX_CMDS_COUNT];                 // 存储字符串命令 和对应函数
    int num;	                                              // 命令数组个数
}CMD_LIST; 
void register_cmds( CMD_Name_Func reg_cmds[], int num );
void match_cmd( char *str );
#endif
