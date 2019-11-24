#ifndef WORDANALYZER_C_CONST_H
#define WORDANALYZER_C_CONST_H

//单词最大个数
#define WORDSIZE 0x1000
//待处理文件的最大长度
#define MAXLENGTH 0x1000
//每一项转换为char之后的最大长度
#define TOKENLENGTH 0x3000
#define TEMPREGISTER 8
//标签的数量
#define LABELSIZE 100
//每个标签的长度
#define LABELLENGTH 9
//记录保存环境时指令位置的栈大小
#define ENV_STACK_LENGTH 100

#endif //WORDANALYZER_C_CONST_H