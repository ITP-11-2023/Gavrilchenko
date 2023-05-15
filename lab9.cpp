#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <conio.h>
#include <iostream>
#include <queue>

using namespace std;

int id=0;
int metka=0;

struct page
{
	int status;
	double memory;
	int processid;
	int counter;
	double badmemory;
};

struct process
{
	int id;
	int memory;
};

void vivod(page *massiv,int kolvo);
double allbadmemory(page *massiv,int kolvo);
void mainprint(page *massiv,double allmemory,int kolvo);
int svoboda(page *massiv,int kolvo);
void deletepage(page *massiv,int index);
void deleteprocess(page *massiv,int kolvo);
void add(page *massiv,int kolvo,process tmp,double gig);

int main()
{
	setlocale(LC_ALL,"Rus");
	int i, kolvo, control;
	process tmp;
	double allmemory,gig;
	page *massiv = new page[kolvo];

	printf("Введите кол-во памяти:\n");
	scanf("%lf",&allmemory);
	printf("Введите кол-во страниц:\n");
	scanf("%d",&kolvo);
	system("cls");

	gig = allmemory/kolvo;

	for(i=0;i<kolvo;i++)
	{
		massiv[i].memory=allmemory/kolvo;
		massiv[i].status=0;
		massiv[i].counter=0;
		massiv[i].badmemory=0;
		massiv[i].processid=-1;
	}
	do
	{
		mainprint(massiv,allmemory,kolvo);
		vivod(massiv,kolvo);
 		printf("1.Добавить процесс\n");
 		printf("2.Удалить заданную страницу\n");
 		printf("3.Удалить заданный процесс\n");
 		printf("4.Циклическое обращение к страницам\n");
 		printf("0.Выход\n");
		scanf("%d",&control);

		switch(control)
		{
			case 1:
			{
				printf("Введите память процесса\n");
				scanf("%d",&tmp.memory);
				add(massiv,kolvo,tmp,gig);
				printf("\n");
				mainprint(massiv,allmemory,kolvo);
				printf("\n");
				vivod(massiv,kolvo);

				break;
			}
			case 2:
			{
				int index;

				printf("Введите номер страницы для удаления\n");
				scanf("%d",&index);
				deletepage(massiv,index);
				printf("\n");
				mainprint(massiv,allmemory,kolvo);
				printf("\n");
 				vivod(massiv,kolvo);

				break;
			}
			case 3:
			{
				deleteprocess(massiv,kolvo);
				printf("\n");
				mainprint(massiv,allmemory,kolvo);
				printf("\n");
				vivod(massiv,kolvo);

				break;
			}
			case 4:
			{
				int rand1 =rand()%10+1;
				int kolich=0;

				printf("\nКоличество обращений %d\n",rand1);

				while(kolich!=rand1)
				{
					massiv[rand() % kolvo].counter++;
					kolich++;
				}

				printf("\n");
				mainprint(massiv,allmemory,kolvo);
				printf("\n");
				vivod(massiv,kolvo);

				break;
			}

			case 0:return(0);
		}
	}
	
	while(control!=0);
	getch();
	return(0);
}

void vivod(page *massiv,int kolvo)
{
	for(int i=0;i<kolvo;i++)
	{
		printf("Cтраница №%d ",i+1);
		printf("Счетчик обращений %d ",massiv[i].counter);
		printf("%.1lf ",massiv[i].badmemory);

		if(massiv[i].processid==-1)printf("Страница не занята\n");
		else printf("Id процесса :%d\n",massiv[i].processid);
	}
}

double allbadmemory(page *massiv,int kolvo)
{
	int i;
	double allbadmemorycount1;

	for(i=0;i<kolvo;i++)
	allbadmemorycount1+=massiv[i].badmemory;

	printf("Объем заполненной памяти: %lf\n",allbadmemorycount1);

	return(allbadmemorycount1);
}

void mainprint(page *massiv,double allmemory,int kolvo)
{
	int i,fig=0;

	for(i=0;i<kolvo;i++)
	{
		if(massiv[i].status==0)
		fig++;
	}

	printf("__________Стата_____________\n");
	printf("Объем свободной памяти :%.1lf\n",allmemory, allbadmemory(massiv,kolvo));
	printf("Число свободных страниц:%d\n",fig);
	printf("Размер страницы :%.1lf\n",allmemory/kolvo);
	printf("_____________________________\n\n\n");
}

int svoboda(page *massiv,int kolvo)
{
	int vern=0,i;

	for(i=0;i<kolvo;i++)
	{
		if(massiv[i].status==0)
		vern++;
	}

	return vern;
}

void deletepage(page *massiv,int index)
{
	massiv[index-1].status=0;
	massiv[index-1].badmemory=0;
	massiv[index-1].processid=-1;
	massiv[index-1].counter++;
}

void deleteprocess(page *massiv,int kolvo)
{
	int index,i;

	printf("Введите id процесса\n");
	scanf("%d",&index);

	for(i=0;i<kolvo;i++)
	{
		if(massiv[i].processid==index)
		{
			massiv[i].status=0;
			massiv[i].badmemory=0;
			massiv[i].processid=-1;
			massiv[i].counter++;
		}
	}
}

void add(page *massiv,int kolvo,process tmp,double gig)
{
	int i;
	double pam = gig;

	if(tmp.memory>pam)
	{
		while(tmp.memory!=0)
		{
			if(svoboda(massiv,kolvo)>0)
			{
				for(int i=0;i<kolvo;i++)
				{
 					if(massiv[i].status==0)
 					{
						if(tmp.memory>massiv[i].memory)
						{
							massiv[i].status=1;
							massiv[i].badmemory=massiv[i].memory;
							massiv[i].processid=id;
							massiv[i].counter++;
							tmp.memory-
							massiv[i].memory;

							break;
						}
						else
						{
							massiv[i].status=1;
							massiv[i].badmemory=tmp.memory;
							massiv[i].processid=id;
							massiv[i].counter++;
							tmp.memory=0;

							break;
						}
					}
				}
			}
			else
			{
				if(tmp.memory>pam)
				{
					int j;
					int er=0; 
					int min=massiv[0].counter;

					for(j=1;j<kolvo;j++)
					{
						if(massiv[j].counter<min) 
						{
							min=massiv[j].counter;
							er=j;
						}
					}
					deletepage(massiv,er+1);
					massiv[er].status=1;
					massiv[er].badmemory=massiv[i].memory;
					massiv[er].processid=id;
					tmp.memory-=massiv[er].memory;
				}
				else
				{
					int er=0,j;
					int min=massiv[0].counter;

					for(j=1;j<kolvo;j++)
					{
						if(massiv[j].counter<min) 
						{
							min=massiv[j].counter;
							er=j;
						}
					}
					deletepage(massiv,er+1);
					massiv[er].status=1;
					massiv[er].badmemory=tmp.memory;
					massiv[er].processid=id;
					tmp.memory=0;
				}
			}
		}
		id++;
	}

	else 
	{
		if(svoboda(massiv,kolvo)>0)
		{
			for(int i=0;i<kolvo;i++)
			{
				if(massiv[i].status==0)
				{
					massiv[i].status=1;
					massiv[i].badmemory=tmp.memory;
					massiv[i].processid=id;
					massiv[i].counter++;
					//q.push(i);
					id++;

					break;
				}
			}
		}
		else
		{
			int er=9999,j,min;// = q.front();
			min=massiv[0].counter;
			er=0;

			for(j=1;j<kolvo;j++)
			{
				if(massiv[j].counter<min) 
				{
					min=massiv[j].counter;
					er=j;
				}
			}
			deletepage(massiv,er+1);
			//printf("3.%d\n\n\n",er);
			//q.pop();
			//q.push(er);
			massiv[er].status=1;
			massiv[er].badmemory=tmp.memory;
			massiv[er].processid=id;
			id++;
		}
	}	
}
