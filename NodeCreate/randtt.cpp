#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <memory.h>
#include <stdio.h>
using namespace std;

#define NUMBER_NODES_DEFAULT 10000  
#define NUMBER_MAX_CHILDREN 8

// �ڵ������ݵ�����
// �������ڵ�����
typedef struct BinaryTreeNode
{
	int id;
	int nChild;
	int prior;		//���ڵ�
	int next[NUMBER_MAX_CHILDREN];	//�ӽڵ�	
}NODE;


int main(int argc, char *argv[])
{
	if (argc<2)
		cout<<"Tips:: The default node number is set to "<<NUMBER_NODES_DEFAULT<<endl;

	srand(time(NULL));  //���������
	
	int nNodes = NUMBER_NODES_DEFAULT;			//Ҫ���ɵĽڵ��������ΪĬ��ֵ
	if (argc>2)
		nNodes = atoi(argv[2]);

	NODE *nodes = new NODE[nNodes];				//һ�δ��������еĽڵ�
	if (nodes==NULL)
	{
		cerr<<"Error:: Fail to allocate memory for nodes ("<<nNodes*sizeof(NODE)<<" bytes)"<<endl;
		return -1;
	}
	
	//root�ڵ�
	nodes[0].id = 0;
	nodes[0].prior = 0;
	nodes[0].nChild = rand()%NUMBER_MAX_CHILDREN+1;
	int nLayers = 0; 						//�Ѿ������Ĳ���
	int nCreatedNodes=1; 					//�Ѿ������Ľڵ�������1��ʼ����Ҳ����һ���ڵ�Ҫ���õ�λ�ã���0��ʼ��
	vector <int> vecIdFirstNodeInnLayers;	//ÿһ��ĵ�һ���ڵ��id��
	vecIdFirstNodeInnLayers.push_back(0);

	while (nCreatedNodes<nNodes)
	{
		nLayers++;
		vecIdFirstNodeInnLayers.push_back(nCreatedNodes);
		int nNodesInPreviousnLayers=nCreatedNodes-vecIdFirstNodeInnLayers[nLayers-1];		//��һ���ж��ٽڵ���
		
		int nChildInCurrentnLayers = 0;
		
		for (int iNode=0; iNode<nNodesInPreviousnLayers; iNode++)
		{
			if (nCreatedNodes>=nNodes) break;
			int parentId=vecIdFirstNodeInnLayers[nLayers-1]+iNode;		//����һ����ȡ��һ�����ڵ�
			for (int iChild=0; iChild<nodes[parentId].nChild; iChild++)
			{
				nodes[nCreatedNodes].id = nCreatedNodes;
				nodes[nCreatedNodes].prior = parentId;
				nodes[nCreatedNodes].nChild = rand()%NUMBER_MAX_CHILDREN+1;
				nCreatedNodes++;

				if (nCreatedNodes>=nNodes) break;
			}
		}
	}

	//test:
	printf("Nodes in a Layer: ");
	for (int i=0; i<vecIdFirstNodeInnLayers.size(); i++)
		printf("%d ", vecIdFirstNodeInnLayers.at(i));
	printf("\n");

	//FILE *fpLayers = fopen(argv[1], "w");
	FILE* fpLayers = fopen("RoutingLayers.txt", "w");
	FILE* fpFlow = fopen("RoutingFlowOut.txt", "w");
	if (fpLayers==NULL || fpFlow==NULL)
	{
		cout<<"Error:: Fail to open file for write"<<endl;
		delete [] nodes;
		if (fpLayers != NULL) fclose(fpLayers);
		if (fpFlow != NULL) fclose(fpFlow);
		return 0;
	}

	for (int iLayer = nLayers; iLayer >= 0; iLayer--)
	{
		int nNodesInPreviousnLayers;
		if (iLayer == nLayers)
			nNodesInPreviousnLayers = nCreatedNodes - vecIdFirstNodeInnLayers[nLayers];
		else
			nNodesInPreviousnLayers = vecIdFirstNodeInnLayers[iLayer + 1] - vecIdFirstNodeInnLayers[iLayer];
		fprintf(fpLayers, "%d \t", nNodesInPreviousnLayers);

		for (int iNode = 0; iNode < nNodesInPreviousnLayers; iNode++)
		{
			fprintf(fpLayers, "%8d\t", nodes[vecIdFirstNodeInnLayers[iLayer] + iNode].id);
			fprintf(fpFlow, "%d\t%d\t%d\n", nodes[vecIdFirstNodeInnLayers[iLayer] + iNode].id, nodes[vecIdFirstNodeInnLayers[iLayer] + iNode].prior, 32);
		}

		fprintf(fpLayers, "\n");
	}

	fclose(fpLayers);
	fclose(fpFlow);
	delete [] nodes;

	return 0;		
}


