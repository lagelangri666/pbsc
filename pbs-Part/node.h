#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "libqstat.h"
using namespace std;

typedef pair<int, int> INT_PAIR;

const char PBS_SCRIPT_DIR[] = "../shnode";	//脚本文件根目录
const char PBS_LOG_DIR[] = "../pbs_log";	//日志文件目录
const char EXT_COM_NAME[] = "./TEST";		//要运行的算法命令行
const char EXT_PARAM_DIR[] = "../txtnode";		//算法所需参数的所在文件夹

typedef struct
{
	int from;
	int to;
	int direction;
}FLOW;

typedef struct
{
	int ID;				//当前节点的名称
	int layer;				//所在层号，从0开始
	int parentID;			//流向节点的名称
	int parent_number;	//流向节点的编号
	int direction;			//流向，不使用
//	vector <int> vector_brothers_cell;	//待完善，选择合适的数据结构，或者不记录该信息
	int number_brothers;	//兄弟节点个数
//	int number_parent;
	char pbs_job_ID[1024];	//pbs运行时任务号
	char pbs_script_filepath[256];	//任务对应的PBS脚本文件全路径
//	int task_ID;
//	int parent_task_ID;
	int number_finished_childen;	//pbs运行时已完成的孩子节点数
	int flag;
	int child_number;
}CELL;

//生成脚本文件
bool CreatePbsScript(const char* pbsFilepath, const char* logInfoDir, const char* strCommand, int cellid);

//提交pbs，开始运行
//只适应分层且汇聚的节点
void PBS_Run(CELL* cells, int nCells, int argc, char** argv);
