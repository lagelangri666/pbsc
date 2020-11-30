#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "libqstat.h"
using namespace std;

typedef pair<int, int> INT_PAIR;

const char PBS_SCRIPT_DIR[] = "../shnode";	//�ű��ļ���Ŀ¼
const char PBS_LOG_DIR[] = "../pbs_log";	//��־�ļ�Ŀ¼
const char EXT_COM_NAME[] = "./TEST";		//Ҫ���е��㷨������
const char EXT_PARAM_DIR[] = "../txtnode";		//�㷨��������������ļ���

typedef struct
{
	int from;
	int to;
	int direction;
}FLOW;

typedef struct
{
	int ID;				//��ǰ�ڵ������
	int layer;				//���ڲ�ţ���0��ʼ
	int parentID;			//����ڵ������
	int parent_number;	//����ڵ�ı��
	int direction;			//���򣬲�ʹ��
//	vector <int> vector_brothers_cell;	//�����ƣ�ѡ����ʵ����ݽṹ�����߲���¼����Ϣ
	int number_brothers;	//�ֵܽڵ����
//	int number_parent;
	char pbs_job_ID[1024];	//pbs����ʱ�����
	char pbs_script_filepath[256];	//�����Ӧ��PBS�ű��ļ�ȫ·��
//	int task_ID;
//	int parent_task_ID;
	int number_finished_childen;	//pbs����ʱ����ɵĺ��ӽڵ���
	int flag;
	int child_number;
}CELL;

//���ɽű��ļ�
bool CreatePbsScript(const char* pbsFilepath, const char* logInfoDir, const char* strCommand, int cellid);

//�ύpbs����ʼ����
//ֻ��Ӧ�ֲ��һ�۵Ľڵ�
void PBS_Run(CELL* cells, int nCells, int argc, char** argv);
