#include<iostream>
#include<cstdlib>
#include<ctime>
#include<vector>
#include <stdlib.h>
#include <memory.h>
#include <fstream>
#include <sstream>
#include <string>
#define NUMBER_MAX_RUNNING 30  //���ҵʱ�䵥λ s
#define NUMBER_MAX_CELLS 1000000  //���ڵ����
#define NUMBER_MAX_FILE 1000   //ÿ���ļ�������ļ�����
#include<Windows.h>
using namespace std;
//clock_t start, endd;
bool CreateFile(int dir)
{
	int cppFile = NUMBER_MAX_CELLS / NUMBER_MAX_FILE;  //ÿ���ļ��е�cpp�ļ�����
	for (int j = 0; j < cppFile; j++)
	{
		char pbsFilepath[256];
		sprintf(pbsFilepath, "D:/c++vs/testfor1/%d/%d.cpp", dir, j);
		int time;                                     //��ҵ����ʱ��
		time = rand() % NUMBER_MAX_RUNNING + 1;       //�����������ʱ�䣬1-300s
		FILE* fpOut = fopen(pbsFilepath, "w");
		if (fpOut == NULL)
		{
			cerr << "Error:: Fail to open file for write: " << pbsFilepath << endl;
			return false;
		}
		fprintf(fpOut, "#include<iostream>\n");
		fprintf(fpOut, "#include<ctime>\n");
		fprintf(fpOut, "#include<cstdlib>\n");
		fprintf(fpOut, "using namespace std;\n");
		fprintf(fpOut, "int main()\n");
		fprintf(fpOut, "{\n");
		fprintf(fpOut, "	clock_t start, ennd;\n");
		fprintf(fpOut, "	start = clock();\n");
		fprintf(fpOut, "	sleep(%d);\n", time);
		fprintf(fpOut, "	ennd = clock();\n");
		fprintf(fpOut, "	double endtime = (double)(ennd - start) / CLOCKS_PER_SEC;\n");
		fprintf(fpOut, "	cout << \"Total time : \" << endtime << endl;		//sΪ��λ\n");
		fprintf(fpOut, "	system(\"pause\");\n");
		fprintf(fpOut, "	return 0;\n");
		fprintf(fpOut, "}\n");
	}
}
int main(int argc, char *argv[]) 
{
	cout << "������Ҫ�������ļ��и���" << endl;
	cout << "ÿ���ļ��д�Ÿ��ļ�" << endl;
	int dir;
	dir = atoi(argv[1]);
	CreateFile(dir);
	cout << dir << endl;
	return 0;
}