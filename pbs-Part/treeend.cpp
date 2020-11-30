
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "node.h"

//#include <unistd.h>
using namespace std;

#define MAX_RUNNING_TASK_NUMBER 50000

//由读入的原始文件信息建立栅格信息数据库
bool GetTree(CELL * cells, int nCells, vector < vector <int> > &vecLayerCell, vector < int > &vCellNumInEachLayer)
{
	int nLayers = vecLayerCell.size();

	//	vector<int> cellID_x;
	//	vector<int> cellID_y;
	vector < INT_PAIR > layer_cellID;

	//节点层次赋值
	for (int iCell = 0; iCell < nCells; iCell++)
	{
		bool isFound = false;
		for (int iLayer = 0; iLayer < nLayers && !isFound; iLayer++)
			for (int j = 0; j < vCellNumInEachLayer[iLayer]; j++)
				if (vecLayerCell[iLayer][j] == cells[iCell].ID)
				{
					cells[iCell].layer	= iLayer;
					isFound = true;
					break;
				}

		if (!isFound)
		{
			cerr << "Error:: cell " << iCell << " is not found in layer file." << endl;
			return false;
		}
	}


	////////查找兄弟节点
	for (int iCell = 0; iCell < nCells; iCell++)
	{
		if (cells[iCell].ID != cells[iCell].parentID) 
		{
		   for (int jCell = iCell + 1; jCell < nCells; jCell++)
		  {
			  if (cells[iCell].parentID == cells[jCell].parentID)
			 {
				cells[iCell].number_brothers++;
				cells[jCell].number_brothers++;
				
			 }
		  }
		}
	
	}

	///////查找父节点,计算孩子节点/////
	for (int iCell = 0; iCell < nCells; iCell++)
	{
		for (int jCell = 0; jCell < nCells; jCell++)
		{

			if (cells[iCell].parentID == cells[jCell].ID)
			{
				cells[iCell].parent_number = jCell;
				cells[jCell].child_number++;
			}
		}		
	}		  

	return true;
}


bool WriteTxt(CELL * cell, int cell_number, const char * FileDir)
{
	char filepath[512] ={0};
	
	sprintf(filepath, "%s/%d.txt", FileDir, cell_number); //这只是文件名集合
	
	fstream outFile(filepath, ios_base::out);		// 文件流，具有输入和输出两种功能 ios_base::out表示打开写入
	if (!outFile.is_open())
	{
		cerr << "Error:: Fail to open file to write: " << filepath << endl;
		return false;
	}

	outFile << cell->parent_number << " " << cell_number << " " << cell->layer << endl; //将parent的编号，parent所在的层数计入到txt文件中
	outFile.close();

	return true;
}

//生成脚本文件，用于pbs作业调度
bool CreatePbsScript(const char * pbsFilepath, const char *logInfoDir, const char *strCommand, int cellid, 
                     const int layer,int nLayers)
{
	FILE *fpOut = fopen(pbsFilepath, "w");

	if (fpOut == NULL)
	{
		cerr << "Error:: Fail to open file for write: " << pbsFilepath << endl;
		return false;
	}
    int nodes =1;
	int ppn= 1;
	int priority = nLayers -layer;
	fprintf(fpOut, "#!/bin/bash\n");
	fprintf(fpOut, "#PBS -p %d\n", priority); 				// 任务优先级
	fprintf(fpOut, "#PBS -o %s\n", logInfoDir); 	//正确信息输出
	fprintf(fpOut, "#PBS -e %s\n", logInfoDir); 	//错误信息输出
	fprintf(fpOut, "#PBS -q high\n");				// 将作业加入high的队列
	fprintf(fpOut, "#PBS -N %d\n",cellid);
	fprintf(fpOut, "#PBS -l walltime=100:00:00\n\n"); //请求任务执行时间
	fprintf(fpOut, "#PBS -l nodes=%d:ppn=%d\n", nodes, ppn); //请求任务资源
	fprintf(fpOut, "cd $PBS_O_WORKDIR\n");
	fprintf(fpOut, "%s", strCommand);
	printf("%s\n",strCommand);
	fclose(fpOut);

	return true;
}

//提交脚本的函数
void Submit2PBS(char * scriptFilepath)
{
	char  pbsCommand[1024];

	sprintf(pbsCommand, "qsub %s", scriptFilepath);
	system(pbsCommand);
}


//将qstat输出的作业名称和状态解析出来
void DecodeString(char * pszin, char mark, vector < int > &idindex)
{
	int nStart(0), nEnd(0), nNum(0), nIndex(0); //标识字符串的开头和结尾
	long int		nLength = strlen(pszin);
	char *			p	= &pszin[nLength - 1];  //待处理的作业名称和状态字符串
	char			pszout[10];                 //用于取出完成的作业的名称

	if (nLength > 0)
	{
		for (; nEnd < nLength; nEnd++)
		{
			if (* (pszin + nEnd) == mark)
			{
				if (*p == 'C')
				{
					for (; nStart < nEnd; nStart++)
					{
						pszout[nIndex++]= * (pszin + nStart);
					}

					int id	= atoi(pszout);

					idindex.push_back(id);

					//pszout[nNum][nIndex] = '\0';
					nIndex = 0;
					nEnd++;
					break;
				}
				else 
					break;
			}

		}

	}

}

//取得作业名称（数字部分）

void GetCellFinishedId(vector < string > job_id, vector < int > &idnum,
                       vector <int>&pbsQueue) 
{
	char mark = '-';                         //利用'-'符号作为分离作业名称的标识
	char * pszin = new char[20];             //存储待分离的作业名称状态字符串
	vector < int > idindex;                  //存储已完成的作业名称

	for (int i = 0; i < job_id.size(); i++)
	{
		strcpy(pszin, job_id[i].c_str());
		DecodeString(pszin, mark, idindex);  //将qstat输出的作业名称和状态解析出来

	}

	if (idindex.size() != 0)
	{
		for (int i=0; i<idindex.size(); i++)
		{
			if (pbsQueue[idindex[i]] != idindex[i])
			{
			    pbsQueue[idindex[i]] = idindex[i];
				idnum.push_back(idindex[i]);
			}
		}				
	}
	vector < int > ().swap(idindex);

}

//更新作业
void UpdateCell(CELL * cells, int nCells, int nLayers)
{
	int nFinished = 1;                 //记录已完成的节点个数
	vector<int> pbsQueue(nCells,-1);		   //记录qsub提交的作业
	while (nFinished < nCells)         //当还存在未完成的作业
	{
		vector <int> cells_id;           //记录状态为C的作业号
		while (cells_id.size() == 0)
		{
		    sleep(10);
			vector < string > job_id_state;
			job_id_state = GetRunningJobs();   //从qstat中取出已完成的作业名称和状态字符
			vector <int> idnum;              //已完成的节点的数字
			GetCellFinishedId(job_id_state, idnum,pbsQueue);  //得到已完成节点的id用于更新
			vector < string > ().swap(job_id_state);
			cells_id = idnum;			//将完成的节点输入到vector中
			vector < int > ().swap(idnum);
		}

		for (int i = 0; i < cells_id.size(); i++)   //将得到的已完成节点id加入到节点树中更新父节点
		{
			CELL *cell = cells + cells_id[i];
			if (cell->parent_number != NULL) //如果父节点不空
			{
				if (cells[cell->parent_number].number_finished_childen != cell->number_brothers )
				{
					cells[cell->parent_number].number_finished_childen++; //更新父节点信息
				}
				else
				{
					if (cells[cell->parent_number].flag == 0) //若节点没有写入txt
					{
						//将父节点计入到文件中
					    WriteTxt(cells + cell->parent_number, cell->parent_number, EXT_PARAM_DIR);
						//生成脚本文件
						char strCommand[1024] ={0};
						//可执行程序命令，待修改
						//sprintf(strCommand, "%s ../%d.txt", EXT_COM_NAME, cell->parent_number); //修改命令
						int dir = cells[cell->parent_number].ID / 500;
						int file = (cells[cell->parent_number].ID % 500)%500;
						sprintf(strCommand, "../testfor1/%d/./testfor%d",dir,file); //测试指令
					    //END OF 可执行程序命令，待修改
						CreatePbsScript(cells[cell->parent_number].pbs_script_filepath, 
										PBS_LOG_DIR, strCommand, cell->parent_number,cells[cell->parent_number].layer,
										 nLayers);	//建立关于父节点的脚本
					    //提交到pbs，并加入监视队列
						Submit2PBS(cells[cell->parent_number].pbs_script_filepath); //将汇聚节点提交到PBS
						cells[cell->parent_number].flag = 1; //生成完作业脚本作业号标记为1
					}
				}
             
			}
			nFinished++;
		} //end for 当已完成的节点全部加载完结束循环
		vector <int> ().swap(cells_id);
		printf("11111111 nFin 254 %d\n", nFinished);
	}//end while 作业全部结束

	printf("11111111 nFin 201 %d\n", nFinished);

}

//提交底层的节点作业任务
void SubmitLayer0ToPBS( CELL * cells, int nCells,int nLayers)
{
	
	for (int i = 0; i < nCells; i++)
		if (cells[i].layer == 0)
		{
			//生成算法所需参数文件
			WriteTxt(cells + i, i, EXT_PARAM_DIR);
			char strCommand[1024] ={0};
			//可执行程序命令，待修改
			//sprintf(strCommand, "%s ../txtnode/%d.txt", EXT_COM_NAME, i);
			int dir = cells[i].ID / 500;  //dir用于记录文件所在的文件夹
	        int file = (cells[i].ID % 500) % 500; //file用于记录文件名
			sprintf(strCommand, "../testfor1/%d/./testfor%d",dir, file); //修改命令
			//END OF 可执行程序命令，待修改
			CreatePbsScript(cells[i].pbs_script_filepath, PBS_LOG_DIR, strCommand, i, cells[i].layer,nLayers);
			Submit2PBS(cells[i].pbs_script_filepath);
			cells[i].flag = 1;
		}
		else if (cells[i].child_number == 0)
		{
			//生成算法所需参数文件
			WriteTxt(cells + i, i, EXT_PARAM_DIR);
			char strCommand[1024] ={0};
			//可执行程序命令，待修改
			//sprintf(strCommand, "%s ../txtnode/%d.txt", EXT_COM_NAME, i);
			int dir = cells[i].ID / 500;  //dir用于记录文件所在的文件夹
	        int file = (cells[i].ID % 500) %500; //file用于记录文件名
			sprintf(strCommand, "../testfor1/%d/./testfor%d",dir, file); //修改命令
			//END OF 可执行程序命令，待修改
			CreatePbsScript(cells[i].pbs_script_filepath, PBS_LOG_DIR, strCommand, i, cells[i].layer,nLayers);
			Submit2PBS(cells[i].pbs_script_filepath);
			cells[i].flag = 1;
			//pbsQueue.push_back(i);
		}
}

//启动pbs提交作业并更新节点
void PBS_Run(CELL * cells, int nCells,int nLayers)
{
	//char nameStream[MAX_RUNNING_TASK_NUMBER];
	SubmitLayer0ToPBS( cells, nCells,nLayers); 	//提交底层任务	返回pbsQueue
	UpdateCell(cells, nCells, nLayers);                      //更新作业节点
}


int main(int argc, char * argv[])
{
	string			strLine;						//每一行
	istringstream	inStream;						//输入流
	int 			s1, s2, s3;
	//分层信息文件
	char			layersFilepath[] = "RoutingLayers.txt";
	ifstream inLayersFile(layersFilepath, ios_base::in);

	if (!inLayersFile.is_open())
	{
		cerr << "Error:: Fail to open file for read: " << layersFilepath << endl;
		return - 1;
	}

	//读入首行:总层数
	int 			nCells = 0; 					//总栅格数
	int 			nLayers = 0;
	char			tempStr[256] ={0};
	getline(inLayersFile, strLine); 				//将文件中的字符串赋值给strLine
	inStream.str(strLine);							//读取strLine
	inStream >> tempStr >> nLayers; 				//然后将其分段附给tempStr和nLayers（层数）
	inStream.clear();
	//读入分层信息
	vector < int > vCellNumInEachLayer; 			//每一层的栅格数
	vector < vector <int> > vecLayerCell;

	for (int iLayer = 0; iLayer < nLayers; iLayer++)
	{
		if (!getline(inLayersFile, strLine))
		{
			cerr << "Error:: Insufficient size in file: " << layersFilepath << endl;
			return - 1;
		}

		//test:
		//cout<<iLayer<<":"<<strLine<<endl;
		inStream.str(strLine);
		inStream >> s1; 							//读入第一个数字
		vCellNumInEachLayer.push_back(s1);

		vector < int > vecLine;

		for (int i = 0; i < s1; i++)
		{
			inStream >> s2; 						//读入栅格编号
			vecLine.push_back(s2);
		}

		vecLayerCell.push_back(vecLine);

		inStream.clear();
	}

	inLayersFile.close();

	//test:
	cout << "number of layers: " << nLayers << endl;

	//流向文件
	char flowFilepath[] = "RoutingFlowOut.txt";
	ifstream inFlowFile(flowFilepath, ios_base::in);

	if (!inFlowFile.is_open())
	{
		cerr << "Error:: Fail to open file for read: " << flowFilepath << endl;
		return - 1;
	}

	//读入首行:总栅格数
	getline(inFlowFile, strLine);
	inStream.str(strLine);
	inStream >> tempStr >> nCells;
	inStream.clear();

	//test:
	cout << "number of cells: " << nCells << endl;

	CELL * cells = new CELL[nCells];

	if (NULL == cells)
	{
		cerr << "Error:: Fail to allocate memory for 'cells'" << endl;
		return - 1;
	}

	//读入流向信息
	for (int iFlow = 0; iFlow < nCells; iFlow++)
	{
		if (!getline(inFlowFile, strLine))
		{
			cerr << "Error:: Insufficient size in file: " << flowFilepath << endl;
			return - 1;
		}

		inStream.str(strLine);
		inStream >> s1 >> s2 >> s3;
		inStream.clear();

		cells[iFlow].ID 	= s1;
		cells[iFlow].parentID = s2;
		cells[iFlow].direction = s3;
		cells[iFlow].number_brothers = 0;
		cells[iFlow].number_finished_childen = 0;
		cells[iFlow].child_number = 0;
		sprintf(cells[iFlow].pbs_script_filepath, "%s/%d.sh", PBS_SCRIPT_DIR, iFlow);
	}
	printf("111111111111111111\n");
	GetTree(cells, nCells, vecLayerCell, vCellNumInEachLayer);
	printf("222222222222222222\n");
	PBS_Run(cells, nCells,nLayers);

	return 0;
}


