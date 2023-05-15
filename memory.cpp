#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

struct processes
{
	char name[20];//имя процесса
	int begin;//первая ячейка которую процесс занимает в памяти
	int size;//размер процесса
};

void showMemory(int *memory,int mc,int *op,int *bp);
//memory-массив памяти,mc-количество выделяемой памяти,op-удовлетворенные запросы,bp-неудовлетворенные запросы
void addProcess(struct processes *process,int *procQ,int *memory,int mc,int *op,int *bp);
//process-массив процессов,procQ-количество процессов
void delProcess(struct processes *process,int *procQ,int *memory,int mc);
void showProcesses(struct processes *process,int procQ);

int main()
{
	system("chcp 1251 && cls");
	struct processes process[100]; //массив процессов
	int op=0,bp=0; //op-удовлетворенные запросы,bp-неудовлетворенные запросы
	int mc,i; //memory cells-количество выделяемой памяти, item-пункт меню
	puts("Сколько байт памяти использовать?");
	scanf("%d",&mc);
	int memory[mc]; //массив - "память"
	int processQuantity=0; //количество процессов
	for(i=0;i<mc;i++)
		memory[i]=0; //обнуление памяти
	int item=0;
	while(item!=4)
	{
		system("cls");
		showMemory(memory,mc,&op,&bp);
		puts("Выберите пункт меню:");
		puts("1. Добавить процесс.");
		puts("2. Удалить процесс.");
		puts("3. Показать процесс.");
		puts("4. Выход.");
		printf("Ваш выбор: ");
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
					puts("Нет записей о процессах.");
				break;
			case 3:	
				if(processQuantity)
					showProcesses(process,processQuantity);
				else
					puts("Нет записей о процессах.");
				break;
			case 4:
				puts("Для завершения нажмите любую клавишу");
				break;
			default:
				puts("Такого пункта нет.");
		}
		getch();
	}
	return 0;
}

void showMemory(int *memory,int mc,int *op,int *bp)
{
	
	SetConsoleOutputCP(866);
	int i,om=0;//om-occupied memory-занятая память
	int maxFreeBlock=0,block=0;//максимальный свободный блок, блок памяти
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
	printf("\nРазмер памяти: %d байт\n",mc);
	printf("Размер свободной памяти: %d байт\n",mc-om);
	printf("Размер наибольщего свободного блока памяти: %d байт\n",maxFreeBlock);
	printf("Количество запросов: %d\n",*op+*bp);
	if(*op+*bp==0) 
		printf("До сих пор запросов не было...\n\n");
	else
		printf("Процент удовлетворенных запросов: %d%%\n\n",*op*100/(*op+*bp));
}
	
void addProcess(struct processes *process,int *procQ,int *memory,int mc,int *op,int *bp)
{
	puts("___________________________ДОБАВЛЕНИЕ ПРОЦЕССА_____________________________________");
	puts("Введите имя процесса");
	fflush(stdin);
	gets(process[*procQ].name);
	puts("Введите размер процесса");
	scanf("%d",&process[*procQ].size);
	int maxFreeBlock=0,block=0,i,l;
	int end=0;//для поиска конца пустого блока
	int resbeg;//начало пустого блока
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
		puts("Недостаточно свободного места");
		(*bp)++;//количество неудовлетворенных запросов увеличивается на 1	
		process[*procQ].begin=-1;
	}
	*procQ+=1;
}

void delProcess(struct processes *process,int *procQ,int *memory,int mc)
{
	puts("Введите номер процесса, который вы хотите удалить.");
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
	puts("______________________ПРОЦЕССЫ________________________");
	int i=0;
	puts("| #  | Имя Процесса  | Размер, байт | Область памяти |");
	for(i=0;i<procQ;i++)
	{
		if((process+i)->begin!=-1)
			printf("| %-3i|%15s|%8i bytes|%7i..%-7i|\n",i+1,process[i].name,process[i].size,process[i].begin,process[i].begin-1+process[i].size);
		else
			printf("| %-3i|%15s|%8i bytes|  Нет в памяти  |\n",i+1,process[i].name,process[i].size);
	}
}
