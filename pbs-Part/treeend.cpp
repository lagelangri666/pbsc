
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

//�ɶ����ԭʼ�ļ���Ϣ����դ����Ϣ���ݿ�
bool GetTree(CELL * cells, int nCells, vector < vector <int> > &vecLayerCell, vector < int > &vCellNumInEachLayer)
{
	int nLayers = vecLayerCell.size();

	//	vector<int> cellID_x;
	//	vector<int> cellID_y;
	vector < INT_PAIR > layer_cellID;

	//�ڵ��θ�ֵ
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


	////////�����ֵܽڵ�
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

	///////���Ҹ��ڵ�,���㺢�ӽڵ�/////
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
	
	sprintf(filepath, "%s/%d.txt", FileDir, cell_number); //��ֻ���ļ�������
	
	fstream outFile(filepath, ios_base::out);		// �ļ��������������������ֹ��� ios_base::out��ʾ��д��
	if (!outFile.is_open())
	{
		cerr << "Error:: Fail to open file to write: " << filepath << endl;
		return false;
	}

	outFile << cell->parent_number << " " << cell_number << " " << cell->layer << endl; //��parent�ı�ţ�parent���ڵĲ������뵽txt�ļ���
	outFile.close();

	return true;
}

//���ɽű��ļ�������pbs��ҵ����
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
	fprintf(fpOut, "#PBS -p %d\n", priority); 				// �������ȼ�
	fprintf(fpOut, "#PBS -o %s\n", logInfoDir); 	//��ȷ��Ϣ���
	fprintf(fpOut, "#PBS -e %s\n", logInfoDir); 	//������Ϣ���
	fprintf(fpOut, "#PBS -q high\n");				// ����ҵ����high�Ķ���
	fprintf(fpOut, "#PBS -N %d\n",cellid);
	fprintf(fpOut, "#PBS -l walltime=100:00:00\n\n"); //��������ִ��ʱ��
	fprintf(fpOut, "#PBS -l nodes=%d:ppn=%d\n", nodes, ppn); //����������Դ
	fprintf(fpOut, "cd $PBS_O_WORKDIR\n");
	fprintf(fpOut, "%s", strCommand);
	printf("%s\n",strCommand);
	fclose(fpOut);

	return true;
}

//�ύ�ű��ĺ���
void Submit2PBS(char * scriptFilepath)
{
	char  pbsCommand[1024];

	sprintf(pbsCommand, "qsub %s", scriptFilepath);
	system(pbsCommand);
}


//��qstat�������ҵ���ƺ�״̬��������
void DecodeString(char * pszin, char mark, vector < int > &idindex)
{
	int nStart(0), nEnd(0), nNum(0), nIndex(0); //��ʶ�ַ����Ŀ�ͷ�ͽ�β
	long int		nLength = strlen(pszin);
	char *			p	= &pszin[nLength - 1];  //���������ҵ���ƺ�״̬�ַ���
	char			pszout[10];                 //����ȡ����ɵ���ҵ������

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

//ȡ����ҵ���ƣ����ֲ��֣�

void GetCellFinishedId(vector < string > job_id, vector < int > &idnum,
                       vector <int>&pbsQueue) 
{
	char mark = '-';                         //����'-'������Ϊ������ҵ���Ƶı�ʶ
	char * pszin = new char[20];             //�洢���������ҵ����״̬�ַ���
	vector < int > idindex;                  //�洢����ɵ���ҵ����

	for (int i = 0; i < job_id.size(); i++)
	{
		strcpy(pszin, job_id[i].c_str());
		DecodeString(pszin, mark, idindex);  //��qstat�������ҵ���ƺ�״̬��������

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

//������ҵ
void UpdateCell(CELL * cells, int nCells, int nLayers)
{
	int nFinished = 1;                 //��¼����ɵĽڵ����
	vector<int> pbsQueue(nCells,-1);		   //��¼qsub�ύ����ҵ
	while (nFinished < nCells)         //��������δ��ɵ���ҵ
	{
		vector <int> cells_id;           //��¼״̬ΪC����ҵ��
		while (cells_id.size() == 0)
		{
		    sleep(10);
			vector < string > job_id_state;
			job_id_state = GetRunningJobs();   //��qstat��ȡ������ɵ���ҵ���ƺ�״̬�ַ�
			vector <int> idnum;              //����ɵĽڵ������
			GetCellFinishedId(job_id_state, idnum,pbsQueue);  //�õ�����ɽڵ��id���ڸ���
			vector < string > ().swap(job_id_state);
			cells_id = idnum;			//����ɵĽڵ����뵽vector��
			vector < int > ().swap(idnum);
		}

		for (int i = 0; i < cells_id.size(); i++)   //���õ�������ɽڵ�id���뵽�ڵ����и��¸��ڵ�
		{
			CELL *cell = cells + cells_id[i];
			if (cell->parent_number != NULL) //������ڵ㲻��
			{
				if (cells[cell->parent_number].number_finished_childen != cell->number_brothers )
				{
					cells[cell->parent_number].number_finished_childen++; //���¸��ڵ���Ϣ
				}
				else
				{
					if (cells[cell->parent_number].flag == 0) //���ڵ�û��д��txt
					{
						//�����ڵ���뵽�ļ���
					    WriteTxt(cells + cell->parent_number, cell->parent_number, EXT_PARAM_DIR);
						//���ɽű��ļ�
						char strCommand[1024] ={0};
						//��ִ�г���������޸�
						//sprintf(strCommand, "%s ../%d.txt", EXT_COM_NAME, cell->parent_number); //�޸�����
						int dir = cells[cell->parent_number].ID / 500;
						int file = (cells[cell->parent_number].ID % 500)%500;
						sprintf(strCommand, "../testfor1/%d/./testfor%d",dir,file); //����ָ��
					    //END OF ��ִ�г���������޸�
						CreatePbsScript(cells[cell->parent_number].pbs_script_filepath, 
										PBS_LOG_DIR, strCommand, cell->parent_number,cells[cell->parent_number].layer,
										 nLayers);	//�������ڸ��ڵ�Ľű�
					    //�ύ��pbs����������Ӷ���
						Submit2PBS(cells[cell->parent_number].pbs_script_filepath); //����۽ڵ��ύ��PBS
						cells[cell->parent_number].flag = 1; //��������ҵ�ű���ҵ�ű��Ϊ1
					}
				}
             
			}
			nFinished++;
		} //end for ������ɵĽڵ�ȫ�����������ѭ��
		vector <int> ().swap(cells_id);
		printf("11111111 nFin 254 %d\n", nFinished);
	}//end while ��ҵȫ������

	printf("11111111 nFin 201 %d\n", nFinished);

}

//�ύ�ײ�Ľڵ���ҵ����
void SubmitLayer0ToPBS( CELL * cells, int nCells,int nLayers)
{
	
	for (int i = 0; i < nCells; i++)
		if (cells[i].layer == 0)
		{
			//�����㷨��������ļ�
			WriteTxt(cells + i, i, EXT_PARAM_DIR);
			char strCommand[1024] ={0};
			//��ִ�г���������޸�
			//sprintf(strCommand, "%s ../txtnode/%d.txt", EXT_COM_NAME, i);
			int dir = cells[i].ID / 500;  //dir���ڼ�¼�ļ����ڵ��ļ���
	        int file = (cells[i].ID % 500) % 500; //file���ڼ�¼�ļ���
			sprintf(strCommand, "../testfor1/%d/./testfor%d",dir, file); //�޸�����
			//END OF ��ִ�г���������޸�
			CreatePbsScript(cells[i].pbs_script_filepath, PBS_LOG_DIR, strCommand, i, cells[i].layer,nLayers);
			Submit2PBS(cells[i].pbs_script_filepath);
			cells[i].flag = 1;
		}
		else if (cells[i].child_number == 0)
		{
			//�����㷨��������ļ�
			WriteTxt(cells + i, i, EXT_PARAM_DIR);
			char strCommand[1024] ={0};
			//��ִ�г���������޸�
			//sprintf(strCommand, "%s ../txtnode/%d.txt", EXT_COM_NAME, i);
			int dir = cells[i].ID / 500;  //dir���ڼ�¼�ļ����ڵ��ļ���
	        int file = (cells[i].ID % 500) %500; //file���ڼ�¼�ļ���
			sprintf(strCommand, "../testfor1/%d/./testfor%d",dir, file); //�޸�����
			//END OF ��ִ�г���������޸�
			CreatePbsScript(cells[i].pbs_script_filepath, PBS_LOG_DIR, strCommand, i, cells[i].layer,nLayers);
			Submit2PBS(cells[i].pbs_script_filepath);
			cells[i].flag = 1;
			//pbsQueue.push_back(i);
		}
}

//����pbs�ύ��ҵ�����½ڵ�
void PBS_Run(CELL * cells, int nCells,int nLayers)
{
	//char nameStream[MAX_RUNNING_TASK_NUMBER];
	SubmitLayer0ToPBS( cells, nCells,nLayers); 	//�ύ�ײ�����	����pbsQueue
	UpdateCell(cells, nCells, nLayers);                      //������ҵ�ڵ�
}


int main(int argc, char * argv[])
{
	string			strLine;						//ÿһ��
	istringstream	inStream;						//������
	int 			s1, s2, s3;
	//�ֲ���Ϣ�ļ�
	char			layersFilepath[] = "RoutingLayers.txt";
	ifstream inLayersFile(layersFilepath, ios_base::in);

	if (!inLayersFile.is_open())
	{
		cerr << "Error:: Fail to open file for read: " << layersFilepath << endl;
		return - 1;
	}

	//��������:�ܲ���
	int 			nCells = 0; 					//��դ����
	int 			nLayers = 0;
	char			tempStr[256] ={0};
	getline(inLayersFile, strLine); 				//���ļ��е��ַ�����ֵ��strLine
	inStream.str(strLine);							//��ȡstrLine
	inStream >> tempStr >> nLayers; 				//Ȼ����ֶθ���tempStr��nLayers��������
	inStream.clear();
	//����ֲ���Ϣ
	vector < int > vCellNumInEachLayer; 			//ÿһ���դ����
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
		inStream >> s1; 							//�����һ������
		vCellNumInEachLayer.push_back(s1);

		vector < int > vecLine;

		for (int i = 0; i < s1; i++)
		{
			inStream >> s2; 						//����դ����
			vecLine.push_back(s2);
		}

		vecLayerCell.push_back(vecLine);

		inStream.clear();
	}

	inLayersFile.close();

	//test:
	cout << "number of layers: " << nLayers << endl;

	//�����ļ�
	char flowFilepath[] = "RoutingFlowOut.txt";
	ifstream inFlowFile(flowFilepath, ios_base::in);

	if (!inFlowFile.is_open())
	{
		cerr << "Error:: Fail to open file for read: " << flowFilepath << endl;
		return - 1;
	}

	//��������:��դ����
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

	//����������Ϣ
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


