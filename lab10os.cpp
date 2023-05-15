#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <Windows.h>

//»меем 1 инод с 8 €чейками блока файла, 16 €чейками косвенной адресации, 16^2 €чейками 2-й косв. адрес и 16^3 €чейками 3-й косв. адрес.
struct hashTable //Ёто не хэш таблица а просто способ организации файлововй системы
{
	int inodeSource; //дл€ проверки, в какой директории находитс€ файл(каталог)
	int inodeNum; // номер €чейки
	char fileName[30]; 
	int inodeMem; //пам€ть €чейки
	int inodeSell[4376]; //€чейка адресации
	int inodeSellOcc; //количество зан€того места в €чейке
	char text[500]; //текст файла
};

void addCatalog(struct hashTable Path[100],int *filesNum,int currentLocation);
void openDirectory(int *filesNum,struct hashTable Path[100],double *memory,int clusterSize,int nessResponse);
void delDirectory(int *filesNum,struct hashTable Path[100],int currentLocation,double *memory);
void createTextFile(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory);
void editText(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory);
void updateInodeNums(struct hashTable Path[100],int *filesNum,int inodeNumber); //обновл€ет номера
int searchValue(struct hashTable Path[100],int *filesNum,char *fileSearch); //хэш функци€
void transferFromHD(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory);
void transferToHD(struct hashTable Path[100],int *filesNum);
void defragmentation(struct hashTable Path[100]); //делает дефрагментацию файловой системы
double fragmentationFunc(struct hashTable Path[100],int *filesNum); //возвращает фрагментацию внутри файловой системы

int main()
{
	struct hashTable Path[100]; //количество элементов
	double memory,clusterSize,fragmentation = 0;
	int filesNum=1,filesNumText = 0,k,i,j,nessResponse;
	bool flag = 0,cycle = 1;
	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	while(cycle == 1)
	{
		puts("Enter cluster size(qkbytes):");
		if( scanf("%lf",&clusterSize) != 1 )
		{
			fflush(stdin);
			cycle = 1;
			puts("Incorrect input!");
		} 
		else
			if(clusterSize <= 0) 
			{
				cycle = 1;
				puts("Incorrect input!");
			}
			else
				cycle = 0;
	}
	Path[0].inodeSellOcc = 0;
	Path[0].inodeMem = 0;
	memory = ((8.0+16.0+16.0*16.0+16.0*16.0*16.0)*clusterSize)/(1024.0);
	while(k!=10)
	{
		//for(i=0;i<15;i++) printf(" %d - %d\n",i,Path[0].inodeSell[i]);    дл€ отладки
		fragmentation = fragmentationFunc(Path,&filesNum);
		int currentLocation = 0;
		puts("-----------------------------");
		printf("| Fragmentation: %10.3f |\n",fragmentation);
		puts("-----------------------------");
		printf("| Free Space: %10.3f mb |\n",memory);
		puts("-----------------------------");
		printf("| Files:                 %d  |\n",filesNum-1);
		puts("-----------------------------");
		printf("|            home           |\n");
		puts("-----------------------------");
		j=1;
		for(i=1;i<filesNum;i++)
		{
			if(Path[i].inodeSource == currentLocation) //≈сли это текстовый то крашу его в желтый
			{
				if( strstr(Path[i].fileName,".txt") != NULL ) SetConsoleTextAttribute(hConsole, (WORD) (0 | 14));
				printf("%d) %s\n",j,Path[i].fileName);
				SetConsoleTextAttribute(hConsole, (WORD) (0 | 15));
				j++;
			}
		}
		puts("-----------------------------");
		printf("|            Menu           |\n");
		puts("-----------------------------");
		puts("1) Open folder");
		puts("2) Create new folder");
		puts("3) Find file or folder");
		puts("4) Delete file or folder");
		puts("5) Create new text file");
		puts("6) Edit the text file");
		puts("7) Read files from HDD");
		puts("8) Write files to HDD");
		puts("9) Defragmentation");
		puts("10) Exit");
		puts("------------------------------");
	    if( scanf("%d",&k) != 1) k = 20; // если количество просканированных символов не равно еденице то "неверный ввод"
		switch (k)
		{
			case 1:
				nessResponse = 0;
				openDirectory(&filesNum,Path,&memory,clusterSize,nessResponse);
				break;
			case 2:
				addCatalog(Path, &filesNum,currentLocation);
				break;
			case 3:
				nessResponse = -1;
				openDirectory(&filesNum,Path,&memory,clusterSize,nessResponse);
				break;
			case 4:
				delDirectory(&filesNum,Path,currentLocation,&memory);
				break;
			case 5:
				createTextFile(Path,&filesNum,currentLocation,clusterSize,&memory);
				break;
			case 6:
				editText(Path,&filesNum,currentLocation,clusterSize,&memory);
				break;
			case 7:
				transferFromHD(Path,&filesNum,currentLocation,clusterSize,&memory);
				break;
			case 8:
				transferToHD(Path,&filesNum);
				break;
			case 9:
				defragmentation(Path);
				break;
			case 10:
				break;
			default:
				puts("Incorrect input!");
				break;
		}
		fflush(stdin);
	}
	return (0);
}

void addCatalog(struct hashTable Path[100],int *filesNum,int currentLocation)
{
	int i;
	bool flag = 1,charCheck = 1;
	double fragmentation;
	puts("Enter the name of the forlder: ");
	fflush(stdin);
	gets(Path[*filesNum].fileName);
	for(i=0;i<30 && charCheck == 1;i++)
	{
		if( Path[*filesNum].fileName[i] == ':' || Path[*filesNum].fileName[i] == '/' || Path[*filesNum].fileName[i] == '\\' || Path[*filesNum].fileName[i] == '*' || Path[*filesNum].fileName[i] == '?' || Path[*filesNum].fileName[i] == '"' || Path[*filesNum].fileName[i] == '<' || Path[*filesNum].fileName[i] == '>' || Path[*filesNum].fileName[i] == '|' ) charCheck = 0;
	}
	if( strstr(Path[*filesNum].fileName,".txt") != NULL ) charCheck = 0;
	if(charCheck == 1) //если ввод не содержит недопустимых символов, то добавл€ем дирректорию
	{
		for(i=0;i<*filesNum;i++)
		{
			if(strcmp(Path[*filesNum].fileName,Path[i].fileName) == 0 && Path[i].inodeSource == currentLocation) flag = 0;
		}
		if(flag == 1)
		{
			Path[*filesNum].inodeNum = *filesNum;
			Path[*filesNum].inodeSource = currentLocation;//BST функци€
			Path[*filesNum].inodeSellOcc = 0;
			Path[*filesNum].inodeMem = 0;
			*filesNum=*filesNum + 1;
		}
		else
			puts("ERROR: You cannot create two identical folders in the same directory!");
	}
	else 
		puts("ERROR: Unacceptable symbols");
}

void openDirectory(int *filesNum,struct hashTable Path[100],double *memory,int clusterSize,int nessResponse)
{
	int j,i,k,k1=0,currentLocation = 0,sourceFile,WTC = 0;
	char location[30],directory[30],directoryTmp[30];
	bool flag = 0,flag2,ONF = 1;
	double fragmentation;
	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	puts("Enter folders name:");
	fflush(stdin);
	gets(directory);
	for(i=0;i<*filesNum;i++)
	{
		if(	strcmp(directory,Path[i].fileName) == 0 )
		{
				flag = 1;
		}
	}
	if(flag == 0)
	{
		puts("ERROR: This folder does not exist!");	
		nessResponse = 0;
	}
	if( nessResponse == -1 ) nessResponse = searchValue(Path,filesNum,directory);
	while( k!=10 && flag == 1 && k1 != 2 )
	{
		if(nessResponse != 0 )
		{
			ONF = 0;
			sourceFile = Path[nessResponse].inodeSource;
			currentLocation = Path[nessResponse].inodeNum ;
			strcpy(location,Path[nessResponse].fileName );
		}
		for(i=*filesNum-1;i>0 && ONF == 1;i--)
		{
			if(strcmp(directory,Path[i].fileName) == 0 && currentLocation != Path[i].inodeNum && WTC == 0 && currentLocation == Path[i].inodeSource)
			{
				sourceFile = Path[i].inodeSource;
				currentLocation = Path[i].inodeNum ;
				strcpy(location,Path[i].fileName );
				i = 0;
			}
		}
		if(WTC != 0 && ONF == 1)
		{
			sourceFile = Path[WTC].inodeSource;
			currentLocation = Path[WTC].inodeNum ;
			strcpy(location,Path[WTC].fileName );
			WTC = 0;
		}
		nessResponse = 0;
		ONF = 1;
		//for(i=0;i<15;i++) printf(" %d - %d\n",i,Path[0].inodeSell[i]);    дл€ отладки
		fragmentation = fragmentationFunc(Path,filesNum);
		puts("-----------------------------");
		printf("| Fragmentation: %10.3f |\n",fragmentation);
		puts("-----------------------------");
		printf("| Free Space: %10.3f mb |\n",*memory);
		puts("-----------------------------");
		printf("| Files:                 %d  |\n",*filesNum-1);
		puts("-----------------------------");
		printf("|%17.17s          |\n",location);
		puts("-----------------------------");
		if( strstr(Path[currentLocation].fileName,".txt") == NULL )
		{
			j=1;
			for(i=0;i<*filesNum;i++)
			{
				if(Path[i].inodeSource == currentLocation)
				{
					if( strstr(Path[i].fileName,".txt") != NULL ) SetConsoleTextAttribute(hConsole, (WORD) (0 | 14));
					printf("%d) %s\n",j,Path[i].fileName);
					SetConsoleTextAttribute(hConsole, (WORD) (0 | 15));
					j++;
				}
			}
			puts("-----------------------------");
			printf("|            Menu           |\n");
			puts("-----------------------------");
			puts("1) Open folder");
			puts("2) Create new folder");
			puts("3) Find file or folder");
			puts("4) Delete file or folder");
			puts("5) Create new text file");
			puts("6) Edit the text file");
			puts("7) Save files from a hard disk to a virtual file");
			puts("8) Write files to a hard disk from a virtual file");
			puts("9) Back");
			puts("10) Back to home");
			puts("------------------------------");
			fflush(stdin);
			if( scanf("%d",&k) != 1) k = 20;
			if(k == 9 && sourceFile == 0) k = 10;
			switch (k)
			{
				case 1:
					puts("Enter folders name:");
					fflush(stdin);
					gets(directoryTmp);
					flag2 = 0;
					for(i=0;i<*filesNum && flag2 == 0;i++)
					{
						if(	strcmp(directoryTmp,Path[i].fileName) == 0)
						{
							strcpy(directory,directoryTmp);
							flag2 = 1;
						}
					}
					if(flag2 == 0) puts("ERROR: This folder does not exist!");
					break;
				case 2:
					addCatalog(Path, filesNum,currentLocation);
					ONF = 0;
					break;
				case 3:
					puts("Enter folders name:");
					fflush(stdin);
					gets(directoryTmp);
					flag2 = 0;
					for(i=0;i<*filesNum && flag2 == 0;i++)
					{
						if(	strcmp(directoryTmp,Path[i].fileName) == 0)
						{
							flag2 = 1;
						}
					}
					if(flag2 == 0)
					{
						puts("ERROR: This folder does not exist!");
						nessResponse = 0;
					}
					else 
						nessResponse = searchValue(Path,filesNum,directory);
					break;
				case 4:
					delDirectory(filesNum,Path,currentLocation,memory);
					break;
				case 5:
					createTextFile(Path,filesNum,currentLocation,clusterSize,memory);
					break;
				case 6:
					editText(Path,filesNum,currentLocation,clusterSize,memory);
					break;
				case 7:
					transferFromHD(Path,filesNum,currentLocation,clusterSize,memory);
					break;
				case 8:
					transferToHD(Path,filesNum);
					break;
				case 9:
					WTC = sourceFile;
					break;
				case 10:
					break;
				default:
					puts("Incorrect input!");
					break;
			}
		}
		else
		{
			puts(Path[currentLocation].text);
			puts("-----------------------------");
			printf("|            Menu           |\n");
			puts("-----------------------------");
			puts("1) Back");
			puts("------------------------------");
			if( scanf("%d",&k1) != 1) k = 20;
			if(k1 == 1 && sourceFile == 0) k1 = 2;
			switch (k1)
			{
				case 1:
					WTC = sourceFile;
					break;
				case 2:
					break;
				default:
					puts("Incorrect input!");
					break;
			}
		}
	}
}

void delDirectory(int *filesNum,struct hashTable Path[100],int currentLocation,double *memory)
{
	char delFile[30];
	int i,j,fileSource,inodeNumber,nessResponse,count = 0;
	bool flag = 1,flag2 = 0,flag3 = 1;
	
	puts("Enter file or folder name:");
	fflush(stdin);
	gets(delFile);
	for(i=1;i<*filesNum && flag2 == 0;i++)
	{
		if(	strcmp(delFile,Path[i].fileName) == 0 )
		{
				flag2 = 1;
		}
	}
	if(flag2 == 0) puts("ERROR: File not found");
	else
		if(strcmp(delFile,Path[currentLocation].fileName) == 0) puts("ERROR: You cannot delete a folder if you are in it!");
		else
		{
			nessResponse = searchValue(Path,filesNum,delFile);
			for(i=1;i<*filesNum;i++)
			{
				if( strcmp(delFile,Path[i].fileName) == 0 && i == nessResponse)
				{
					Path[i].inodeSource = -10;
					i = *filesNum;
				}
			}
			for(i=1;i<*filesNum;i++)
			{
				fileSource = i;
				while(fileSource > 0)
				{
					fileSource = Path[fileSource].inodeSource;
					if(fileSource == -10 && Path[i].inodeSource != -10)
					{
						Path[i].inodeSource = -10;
						i = 0;
					}
				}
			}
			*memory = *memory*1024;
			for(i=1;i<*filesNum;i++)
			{
				if(Path[i].inodeSource == -10)
				{
					inodeNumber = i;
					*memory = *memory + Path[i].inodeMem;
					Path[0].inodeMem = Path[0].inodeMem - Path[i].inodeMem;
					updateInodeNums(Path,filesNum,inodeNumber);
				}
			}
			*memory = *memory/1024;
			for(i=1;i<*filesNum;i++)
			{
				if(Path[i].inodeSource == -10)
				{
					Path[i] = Path[i+1];
					for(j=0;j<Path[0].inodeSellOcc;j++)
					{
						if(Path[0].inodeSell[j] == i+1) Path[0].inodeSell[j] = i;
					}
					Path[i+1].inodeSource = -10;
					if( i == *filesNum - 1 && count > 0 && flag3 == 1) 
					{
						flag3 = 0;
						*filesNum = *filesNum + 1;
					}
					i=0;
					*filesNum = *filesNum - 1;
					count++;
				}
			}
		}
}

void createTextFile(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory)
{
	int i,j,k,numSell;
	bool flag = 1,charCheck = 1;
	puts("Enter the name of a text file: ");
	fflush(stdin);
	gets(Path[*filesNum].fileName);
	
	for(i=0;i<30 && charCheck == 1;i++)
	{
		if( Path[*filesNum].fileName[i] == ':' || Path[*filesNum].fileName[i] == '/' || Path[*filesNum].fileName[i] == '\\' || Path[*filesNum].fileName[i] == '*' || Path[*filesNum].fileName[i] == '?' || Path[*filesNum].fileName[i] == '"' || Path[*filesNum].fileName[i] == '<' || Path[*filesNum].fileName[i] == '>' || Path[*filesNum].fileName[i] == '|' ) charCheck = 0;
	}
	if( charCheck == 1 )
	{
		strcat(Path[*filesNum].fileName,".txt");
		for(i=0;i<*filesNum;i++)
		{
			if(strcmp(Path[*filesNum].fileName,Path[i].fileName) == 0 && Path[i].inodeSource == currentLocation) flag = 0;
		}
		if(flag == 0) puts("ERROR: You cannot create two identical files in the same directory!");
		else
		{
			Path[*filesNum].inodeNum = *filesNum;
			Path[*filesNum].inodeSource = currentLocation;
			*filesNum=*filesNum + 1;
			puts("Do you want to fill the file ?");
			puts("1) Yes");
			puts("2) No ");
			scanf("%d",&k);
			switch (k)
			{
				case 1:
					puts("Enter information: ");
					fflush(stdin);
					gets(Path[*filesNum-1].text);
					Path[*filesNum-1].inodeMem = (strlen(Path[*filesNum-1].text)*4);
					*memory = *memory*1024;
					*memory = *memory - Path[*filesNum-1].inodeMem;
					*memory = *memory/1024;
					Path[0].inodeMem = Path[0].inodeMem + Path[*filesNum-1].inodeMem;
					Path[*filesNum-1].inodeSellOcc = Path[*filesNum-1].inodeMem/clusterSize;
					if( Path[*filesNum-1].inodeMem % clusterSize != 0) Path[*filesNum-1].inodeSellOcc++;
					numSell = Path[*filesNum-1].inodeSellOcc;
					Path[0].inodeSellOcc = Path[0].inodeSellOcc + numSell;
					j=0;
					i=0;
					while(j<numSell)
					{
						if(Path[0].inodeSell[i] == 0)
						{
							Path[0].inodeSell[i] = *filesNum-1;
							j++;
						}
						i++;
					}
					break;
				default:
					Path[*filesNum-1].inodeMem = 0;
					Path[*filesNum-1].inodeSellOcc = 0;
					break;
			}
		}
	}
	else
		puts("ERROR: Unacceptable symbols");
}

void editText(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory)
{
	char textName[30];
	int inodeNumber, textNumber,numSell,numSellOld,i,j,nessResponse;
	bool flag = 0;
	
	puts("Enter the name of a text file: ");
	fflush(stdin);
	gets(textName);
	
	for(i=0;i<*filesNum;i++)
	{
		if(	strcmp(textName,Path[i].fileName) == 0 )
		{
				flag = 1;
		}
	}
	if(flag == 0) puts("ERROR: This text file does not exist!");
	else
	{
		nessResponse = searchValue(Path,filesNum,textName);
		for(i=0;i<*filesNum;i++) 
		{
			if(strcmp(textName,Path[i].fileName) == 0 && i == nessResponse)
			{
				inodeNumber = i;	
				i=*filesNum;
			}
		}
		puts("Old information: ");
		puts(Path[inodeNumber].text);
		numSellOld = Path[inodeNumber].inodeSellOcc;
		Path[inodeNumber].inodeSellOcc = Path[inodeNumber].inodeSellOcc - (Path[inodeNumber].inodeMem/clusterSize);
		if( Path[inodeNumber].inodeMem % clusterSize != 0) Path[inodeNumber].inodeSellOcc--;
		*memory = *memory*1024;
		*memory = *memory + Path[inodeNumber].inodeMem;
		*memory = *memory/1024;
		Path[0].inodeMem = Path[0].inodeMem - Path[inodeNumber].inodeMem;
		Path[inodeNumber].inodeMem = 0;
		memset(Path[inodeNumber].text,0,sizeof(Path[inodeNumber].text));
		
		puts("Enter new information: ");
		fflush(stdin);
		gets(Path[inodeNumber].text);
		Path[inodeNumber].inodeMem = ((strlen(Path[inodeNumber].text))*4);
		*memory = *memory*1024;
		*memory = *memory - Path[inodeNumber].inodeMem;
		*memory = *memory/1024;
		Path[0].inodeMem = Path[0].inodeMem + Path[inodeNumber].inodeMem;
		Path[inodeNumber].inodeSellOcc = Path[inodeNumber].inodeSellOcc + (Path[inodeNumber].inodeMem/clusterSize);
		if( Path[inodeNumber].inodeMem % clusterSize != 0) Path[inodeNumber].inodeSellOcc++;
		numSell = Path[inodeNumber].inodeSellOcc;
		if(numSell > numSellOld)
		{
			Path[0].inodeSellOcc = Path[0].inodeSellOcc - numSellOld;
			Path[0].inodeSellOcc = Path[0].inodeSellOcc + numSell;
		}
		updateInodeNums(Path,filesNum,inodeNumber);
		j=0;
		i=0;
		while(j<numSell)
		{
			if(Path[0].inodeSell[i] == 0)
			{
				Path[0].inodeSell[i] = inodeNumber;
				j++;
			}
			i++;
		}
	}
}
void updateInodeNums(struct hashTable Path[100],int *filesNum,int inodeNumber)
{
	int i,j;

	for(j=0;j<Path[0].inodeSellOcc;j++)
	{
		if(Path[0].inodeSell[j] == inodeNumber) Path[0].inodeSell[j] = 0;
	}
}
int searchValue(struct hashTable Path[100],int *filesNum,char *fileSearch) // хэш-функци€
{
	int i,nessResponse,numFile,sourceNumFile,numFilePrev = 0,k=0;
	char locationRes[30],pathFile[30];
	bool cycle = TRUE;
	for(i=1;i<*filesNum;i++)
	{
		if(strcmp(fileSearch,Path[i].fileName) == 0 && strlen(Path[i].fileName) > 0)
		{
			sourceNumFile = Path[i].inodeSource;
			numFile = i;
			strcpy(locationRes,fileSearch);
			while(sourceNumFile != 0)
			{
				strcpy(pathFile,Path[sourceNumFile].fileName);
				strcat(pathFile,"/");
				strcat(pathFile,locationRes);
				strcpy(locationRes,pathFile);
				sourceNumFile = Path[sourceNumFile].inodeSource;
			}
			strcpy(locationRes,"home/");
			if( Path[numFile].inodeSource == 0) strcat(locationRes,fileSearch);
			else 
				strcat(locationRes,pathFile);
			if(strcmp(fileSearch,Path[numFile].fileName) == 0);
			{
				printf("Adress: %d Location: ",numFile);
				puts(locationRes);	
			}
		}
	}
	while(cycle != FALSE)
	{
		puts("Enter address: ");
		if( scanf("%d",&nessResponse) != 1)
		{
			fflush(stdin);
			puts("Incorrect input!");
		} 
		else
			if(nessResponse > *filesNum || nessResponse <= 0 || strcmp(fileSearch,Path[nessResponse].fileName) != 0) 
			{
				puts("Incorrect input!");
			}
			else
			{
				cycle = FALSE;
			}
	}
	return (nessResponse);
}

void transferFromHD(struct hashTable Path[100],int *filesNum,int currentLocation,int clusterSize,double *memory)
{
	int i,j,k,numSell;
	bool flag = 1;
	FILE *f1;
	puts("Enter the directory of the text file: ");
	fflush(stdin);
	gets(Path[*filesNum].fileName);
	
	for(i=0;i<*filesNum;i++)
	{
		if(strcmp(Path[*filesNum].fileName,Path[i].fileName) == 0 && Path[i].inodeSource == currentLocation) flag = 0;
	}
	if(flag == 0) puts("ERROR: You cannot create two identical files in the same directory!");
	else
	{
		if((f1= fopen(Path[*filesNum].fileName, "r"))==NULL)
	    {
	        perror("ERROR: ");
	        fflush(stdin);
	    }
	    else
	    {
	    	Path[*filesNum].inodeNum = *filesNum;
			Path[*filesNum].inodeSource = currentLocation;
			*filesNum=*filesNum + 1;
			
			fgets(Path[*filesNum-1].text,500,f1);
			fclose(f1);
					
			Path[*filesNum-1].inodeMem = (strlen(Path[*filesNum-1].text)*4);
			*memory = *memory*1024;
			*memory = *memory - Path[*filesNum-1].inodeMem;
			*memory = *memory/1024;
			Path[0].inodeMem = Path[0].inodeMem + Path[*filesNum-1].inodeMem;
			Path[*filesNum-1].inodeSellOcc = Path[*filesNum-1].inodeSellOcc + (Path[*filesNum-1].inodeMem/clusterSize);
			if( Path[*filesNum-1].inodeMem % clusterSize != 0) Path[*filesNum-1].inodeSellOcc++;
			numSell = Path[*filesNum-1].inodeSellOcc;
			Path[0].inodeSellOcc = Path[0].inodeSellOcc + numSell;
			j=0;
			i=0;
			while(j<numSell)
			{
				if(Path[0].inodeSell[i] == 0)
				{
					Path[0].inodeSell[i] = *filesNum-1;
					j++;
				}
				i++;
			}
		}
	}
}

void transferToHD(struct hashTable Path[100],int *filesNum)
{
	char transFile[30];
	int i,numFile;
	bool flag = 0;
	
	puts("Enter the name of the text file: ");
	fflush(stdin);
	gets(transFile);
	
	for(i=0;i<*filesNum && flag == 0;i++)
	{
		if(	strcmp(transFile,Path[i].fileName) == 0 )
		{
				flag = 1;
				numFile = i;
		}
	}
	if(flag == 0) puts("ERROR: This file does not exist!");
	else
	{
		FILE *f1;
		f1 = fopen(transFile,"w");
		fprintf(f1,"%s",Path[numFile].text);
		fclose(f1);
	}
}

void defragmentation(struct hashTable Path[100])
{
	int tmp,j;
	for(j=0;j<Path[0].inodeSellOcc;j++)
	{
		if(Path[0].inodeSell[j]>Path[0].inodeSell[j+1])
		{
			tmp = Path[0].inodeSell[j+1];
			Path[0].inodeSell[j+1] = Path[0].inodeSell[j];
			Path[0].inodeSell[j] = tmp;
			j = -1;
		}
	}
	for(j=0;j<Path[0].inodeSellOcc;j++)
	{
		if(Path[0].inodeSell[j]<Path[0].inodeSell[j+1] && Path[0].inodeSell[j] == 0)
		{
			tmp = Path[0].inodeSell[j+1];
			Path[0].inodeSell[j+1] = Path[0].inodeSell[j];
			Path[0].inodeSell[j] = tmp;
			j = -1;
		}
	}
}

double fragmentationFunc(struct hashTable Path[100],int *filesNum)
{
	int Num,i,j,p;
	bool flag = 0;
	double fragmentation, fragTmp;
	Num=0;
	fragTmp=0;
	for(i=1;i<*filesNum;i++)
	{
		p = 0;
		flag = 1;
		Num = 0;
		for(j=0;j<=Path[0].inodeSellOcc;j++)
		{
			if(Path[i].inodeSellOcc > 0)
			{
				if( Num == Path[i].inodeSellOcc )
				{
					fragTmp = fragTmp + p - Num;
					j = Path[0].inodeSellOcc;
				}
				if(Path[0].inodeSell[j] == i)
				{
					Num = Num + 1;
					if(flag == 1)
					{
						flag = 0;
						p = 0;
					}
				}
			}
			p++;
		}
	}
	fragmentation = (fragTmp/4376)*100;
	return (fragmentation);
}
