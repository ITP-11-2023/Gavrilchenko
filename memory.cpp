#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

struct processes
{
	char name[20];//��� ��������
	int begin;//������ ������ ������� ������� �������� � ������
	int size;//������ ��������
};

void showMemory(int *memory,int mc,int *op,int *bp);
//memory-������ ������,mc-���������� ���������� ������,op-��������������� �������,bp-����������������� �������
void addProcess(struct processes *process,int *procQ,int *memory,int mc,int *op,int *bp);
//process-������ ���������,procQ-���������� ���������
void delProcess(struct processes *process,int *procQ,int *memory,int mc);
void showProcesses(struct processes *process,int procQ);

int main()
{
	system("chcp 1251 && cls");
	struct processes process[100]; //������ ���������
	int op=0,bp=0; //op-��������������� �������,bp-����������������� �������
	int mc,i; //memory cells-���������� ���������� ������, item-����� ����
	puts("������� ���� ������ ������������?");
	scanf("%d",&mc);
	int memory[mc]; //������ - "������"
	int processQuantity=0; //���������� ���������
	for(i=0;i<mc;i++)
		memory[i]=0; //��������� ������
	int item=0;
	while(item!=4)
	{
		system("cls");
		showMemory(memory,mc,&op,&bp);
		puts("�������� ����� ����:");
		puts("1. �������� �������.");
		puts("2. ������� �������.");
		puts("3. �������� �������.");
		puts("4. �����.");
		printf("��� �����: ");
		scanf("%d",&item);
		switch(item)
		{
			case 1: 
				addProcess(process,&processQuantity,memory,mc,&op,&bp);
				break;
			case 2:
				if(processQuantity)
					delProcess(process,&processQuantity,memory,mc);
				else
					puts("��� ������� � ���������.");
				break;
			case 3:	
				if(processQuantity)
					showProcesses(process,processQuantity);
				else
					puts("��� ������� � ���������.");
				break;
			case 4:
				puts("��� ���������� ������� ����� �������");
				break;
			default:
				puts("������ ������ ���.");
		}
		getch();
	}
	return 0;
}

void showMemory(int *memory,int mc,int *op,int *bp)
{
	
	SetConsoleOutputCP(866);
	int i,om=0;//om-occupied memory-������� ������
	int maxFreeBlock=0,block=0;//������������ ��������� ����, ���� ������
	for(i=0;i<mc;i++)
		{
			if(*(memory+i)==0)
			{
				block++;
				printf("\xB0");
			}	
			else
			{
				SetConsoleTextAttribute(console,*(memory+i));
				block=0;
				om+=1;
				printf("\xB2");
				SetConsoleTextAttribute(console,7);
			}	
			if(block>maxFreeBlock)
				maxFreeBlock=block;	
		}
		
	SetConsoleOutputCP(1251);
	printf("\n������ ������: %d ����\n",mc);
	printf("������ ��������� ������: %d ����\n",mc-om);
	printf("������ ����������� ���������� ����� ������: %d ����\n",maxFreeBlock);
	printf("���������� ��������: %d\n",*op+*bp);
	if(*op+*bp==0) 
		printf("�� ��� ��� �������� �� ����...\n\n");
	else
		printf("������� ��������������� ��������: %d%%\n\n",*op*100/(*op+*bp));
}
	
void addProcess(struct processes *process,int *procQ,int *memory,int mc,int *op,int *bp)
{
	puts("___________________________���������� ��������_____________________________________");
	puts("������� ��� ��������");
	fflush(stdin);
	gets(process[*procQ].name);
	puts("������� ������ ��������");
	scanf("%d",&process[*procQ].size);
	int maxFreeBlock=0,block=0,i,l;
	int end=0;//��� ������ ����� ������� �����
	int resbeg;//������ ������� �����
	for(i=0;i<=mc;i++)
	{
		if(memory[i]==0 && block!=process[*procQ].size)
		{
			block++;
		}
		else
		{
			
			if(block==process[*procQ].size)
			{
				i=mc+1;
				resbeg = end-block;
				printf("%d   ",resbeg);
				process[*procQ].begin = resbeg;
				for(l=resbeg; l<resbeg+process[*procQ].size; l++)
					memory[l]=*procQ+1;
				*op+=1;
			}
			block=0;
		}
		end++;
	}
	if(i==mc+1)
	{
		puts("������������ ���������� �����");
		(*bp)++;//���������� ����������������� �������� ������������� �� 1	
		process[*procQ].begin=-1;
	}
	*procQ+=1;
}

void delProcess(struct processes *process,int *procQ,int *memory,int mc)
{
	puts("������� ����� ��������, ������� �� ������ �������.");
	int number,i;
	scanf("%d",&number);
	for(i=number-1;i<*procQ-1;i++)
		*(process+i)=*(process+i+1);
	(*procQ)-=1;
	for(i=0;i<mc;i++)
		if(*(memory+i)>=number)
		{
			if(*(memory+i)==number)
				*(memory+i)=0;	
		}
}

void showProcesses(struct processes *process,int procQ)
{
	puts("______________________��������________________________");
	int i=0;
	puts("| #  | ��� ��������  | ������, ���� | ������� ������ |");
	for(i=0;i<procQ;i++)
	{
		if((process+i)->begin!=-1)
			printf("| %-3i|%15s|%8i bytes|%7i..%-7i|\n",i+1,process[i].name,process[i].size,process[i].begin,process[i].begin-1+process[i].size);
		else
			printf("| %-3i|%15s|%8i bytes|  ��� � ������  |\n",i+1,process[i].name,process[i].size);
	}
}
