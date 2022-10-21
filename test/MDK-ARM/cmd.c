#include <string.h>
#include "cmd.h"
#include "usart.h"
static CMD_LIST command_list = {NULL, 0};  // 全局命令列表，保存已注册命令集合
/*
* 函数介绍： 命令注册函数 每新添加一个命令，就添加到命令列表中
* 输入参数： reg_cmds 待注册命令结构体数组
*            length   数组个数
* 输出参数： 无
* 返回值 ：  无
* 备    注： length 不得超过 MAX_CMDS_COUNT  
*/
void register_cmds( CMD_Name_Func reg_cmds[], int length )
{
    int i;
    if ( length > MAX_CMDS_COUNT )
    {
        return;
    }
 
    for ( i = 0; i < length; i++ )
    {
        if ( command_list.num < MAX_CMDS_COUNT ) // 命令列表未满
        {
            strcpy( command_list.cmdNames[command_list.num].cmd_name, reg_cmds[i].cmd_name );       //将字符串命令拷贝到列表中
            command_list.cmdNames[command_list.num].cmd_functions = reg_cmds[i].cmd_functions;          //将命令对应的函数存储在列表中
            command_list.num++;                                                                     // 数量值默认为0，每添加一个命令，数量加1.             
        }
    }
}
/*
* 函数介绍： 命令匹配执行函数
* 输入参数： cmdStr 待匹配命令字符串
* 输出参数： 无
* 返回值 ：  无
* 备    注： cmdStr 长度不得超过 MAX_CMD_NAME_LENGTH
*/
void match_cmd( char *cmdStr )
{
    int i;
 
    if ( strlen( cmdStr ) > MAX_CMD_LENGTH )
    {
        return;
    }
 
    for ( i = 0; i < command_list.num; i++ )	                                   // 遍历命令列表
    {
        if ( strcmp( command_list.cmdNames[i].cmd_name, cmdStr ) == 0 )          //比较接收到的命令字符串 和 列表中存储的命令字符串是否相等，如果相等就调用命令字符串对应的函数。
        {
            command_list.cmdNames[i].cmd_functions();
        }
    }
}
