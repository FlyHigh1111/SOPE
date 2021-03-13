#include "xmod.h"

int InitializeArguments(int argc, char *argv[], struct Arguments *args)
{

    if (argc < 3 || argc > 6 )
    {
        printf("Usage: xmod [OPTIONS] MODE FILE/DIR\n");
        printf("Usage: xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return 1;
    }

    //counts the number of options introduced in the command line
   /* int num_options = 0;
    int max_options = 3;
    for(size_t k = 1; k <= max_options; k++)
    {
        if(strcmp(argv[k],"-v") == 0 || strcmp(argv[k],"-c") == 0 || strcmp(argv[k],"-R") == 0)
        {
            num_options++;
        }
    }*/
    int num_options=argc-3;

    if(num_options > 0)
    {
        //allocating memory for array of options
        args->options = malloc(num_options * sizeof(char *)); 
        if (args->options != NULL)
        {
            for (int i = 0 ; i < num_options ; i++)
            {
                int number_chars = 2;
                args->options[i] = malloc(number_chars); //each option has only 2 characters (e.g. '-v')
            }
        }  
        //initializes the array of options
        for(size_t j = 0; j < num_options; j++)
        {
            if(strcmp(argv[j+1], "-v") != 0 && strcmp(argv[j+1], "-c") != 0 && strcmp(argv[j+1],"-R") != 0)
            {
                printf("Available options:\n-v\n-c\n-R\n");
                return 1;
            }
            strcpy(args->options[j], argv[j+1]);
        }

    } 

    //verifies mode
    char *mode = argv[num_options+1];
    if (mode[0] == '0') //mode in octal
    { 
        if (strlen(mode) != 4)
        {
            printf("Invalid mode!\n");
            return 1;
        }
        for (int i = 1; i < 4; i++)
        {
            if ((mode[i] < '0') || (mode[i] > '7'))
            {
                printf("Invalid mode!\n");
                return 1;
            }
        }
    }
    else
    {
        char dominio[8];
        strcpy(dominio,"augo");
        if(strchr(dominio, mode[0]) == NULL)
        {
            printf("Invalid mode!\n");
            return 1;
        }
        
    }
    args->mode = mode;

    //verifies if file exists
    FILE *fich=fopen(argv[num_options+2],"r");
    if(fich==NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    fclose(fich);
    args->file_path = argv[num_options+2];

    //prints arguments
    printf("Options:\n");
    for (size_t k = 0; k < num_options; k++)
    {
        printf("%s\n", args->options[k]);
    }
    printf("mode: %s\n", args->mode);
    printf("file_path: %s\n", args->file_path);

    return 0;
}
int GetFilePermissions(const char *pathname){
  int Perm=0;
  struct stat fileattrib;
  int fileMode;

  if (stat(pathname,&fileattrib)==0){
    fileMode = fileattrib.st_mode;
    /* Check owner permissions */
    if ((fileMode & S_IRUSR)) //&& (fileMode & S_IREAD))
      Perm += 256;
    if ((fileMode & S_IWUSR))// && (fileMode & S_IWRITE))
      Perm += 128;
    if ((fileMode & S_IXUSR))// && (fileMode & S_IEXEC))
      Perm += 64;
    if ((fileMode & S_IRGRP) && (fileMode & S_IREAD))
      Perm += 32;
    if ((fileMode & S_IWGRP) && (fileMode & S_IWRITE))
      Perm += 16;
    if ((fileMode & S_IXGRP) && (fileMode & S_IEXEC))
      Perm += 8;
    if ((fileMode & S_IROTH) && (fileMode & S_IREAD))
      Perm += 4;
    if ((fileMode & S_IWOTH) && (fileMode & S_IWRITE))
      Perm += 2;
    if ((fileMode & S_IXOTH) && (fileMode & S_IEXEC))
      Perm += 1;
    return Perm;
  }
  else{
    Perm=-1;
    return Perm;
  }
}
int GetNewPermMask(char *NewMode){
  char users=NewMode[0];
  int NewPerm=0;
  if (strchr("augo",NewMode[0])){
    
    /*if (strchr("-+=",NewMode[0]))
      users='t';
    printf("P1\n");*/  
    switch (users){
      case 'u':
          for(int i=2;i<strlen(NewMode);i++){
            printf("P2\n");  
            if(NewMode[i]=='r')
              NewPerm += 256;
            else
              if(NewMode[i]=='w')
                NewPerm += 128;
              else
                if(NewMode[i]=='x')
                  NewPerm += 64;  
          }
        break;
      case 'g':
          for(int i=2;i<strlen(NewMode);i++){
            if(NewMode[i]=='r')
              NewPerm += 32;
            else
              if(NewMode[i]=='w')
                NewPerm += 16;
              else
                if(NewMode[i]=='x')
                  NewPerm += 8;  
          }
        break;
      case 'o':
          for(int i=2;i<strlen(NewMode);i++){
            if(NewMode[i]=='r')
              NewPerm += 4;
            else
              if(NewMode[i]=='w')
                NewPerm += 2;
              else
                if(NewMode[i]=='x')
                  NewPerm += 1;  
          } 
        break;
      case 'a':
          for(int i=2;i<strlen(NewMode);i++){
            if(NewMode[i]=='r')
              NewPerm += 256 + 32 + 4;
            else
              if(NewMode[i]=='w')
                NewPerm += 128 + 16 + 2;
              else
                if(NewMode[i]=='x')
                  NewPerm += 64 + 8 + 1;  
          }
        break; 
      /*case 't':
          for(int i=1;i<strlen(NewMode);i++){
            if(NewMode[i]=='r')
              NewPerm += 256 + 32 + 4;
            else
              if(NewMode[i]=='w')
                NewPerm += 128 + 16 + 2;
              else
                if(NewMode[i]=='x')
                  NewPerm += 64 + 8 + 1;  
          }
        break; */
      default:
          
        break;     
    }
  }
  printf("NewPerm: %d", NewPerm);
  return NewPerm;
}

int GetNewPermissions(int FormPerm, char *NewMode){
  int NewPerm;
  int NewMask=GetNewPermMask(NewMode);
  char Operator;
  if (strchr("augo",NewMode[0]))
    Operator=NewMode[1];
  else
    Operator=NewMode[0];
printf("NewMask: %d",NewMask);  
  switch(Operator){
    case '+':
        NewPerm = FormPerm | NewMask;
      break; 
    case '-':
        NewPerm = FormPerm & ~NewMask;
      break;
    case '=':
        NewPerm = NewMask;
      break;
    default:
        return -1;
      break;
  }
  return NewPerm;
}

FILE* getLOG_FILENAME()
{
    const char* s;
    s = getenv("LOG_FILENAME");

    if(s == NULL)
    {
      printf("getenv returned NULL\n");
      return NULL;
      
    }
    printf("%s = %s\n","LOG_FILENAME", s);
    FILE* file = fopen(s, "w");
    return file;    
}

int ChangeFilePermissions(const char *pathname, int NewPerm)
{
	int r;
	r = chmod(pathname, NewPerm);
	if (r != 0)
	{
		fprintf(stderr,"Unable to set new permissions on '%s'\n",pathname);
        	exit(1);
	}
	return 0;
}

int WriteOnFile(const char *pathname, char *text)
{
	FILE* file = fopen(pathname, "w");
	if (file == NULL)
	{
		printf("Error.\n");
		exit(1);
	}
	fprintf(file, "%s", text);
	fclose(file);
	return 0;
}

int ReadFile(const char *pathname)
{
	FILE* file = fopen(pathname, "r");
	char line[256];
	if (file == NULL)
	{
		printf("Error.\n");
		exit(1);
	}
	while (fgets(line, sizeof(line), file))
	{
        	printf("%s", line); 
        }
        fclose(file);
	return 0;
}


int main( int argc, char *argv[], char *envp[])  
{
    struct Arguments args;
    clock_t start, end;
    struct tms t;
    long ticks;


    start = times(&t);              //starts counting time
    ticks = sysconf(_SC_CLK_TCK);


    if(InitializeArguments(argc, argv, &args) == 1)
    {
        printf("Something went wrong! Closing...\n");
        return 1;
    }
    
    int ActualPerm=GetFilePermissions(args.file_path);
    printf("Permissoes actuais: %o\n", ActualPerm);
    int NewPerm;


    if (argv[argc-2][0]=='0')
        NewPerm=strtoul(argv[argc-2],NULL,8);
    //printf("nova permissao: %s\n",argv[argc-2]);

    else{  
        NewPerm=GetNewPermissions(ActualPerm,argv[argc-2]);
        printf("nova permissao: %o\n",NewPerm); 
}


      //Altera Permissoes
    chmod(argv[argc-1],NewPerm);



    //Teste para verificar o tempo, como o programa ainda é mt pequeno o tempo era 0
    for (size_t i = 0; i < 10000; i++){
      printf("TESTE\n");
    }
     

    ticks = ticks;
    end = times(&t);

    //file with regists
    FILE* file = getLOG_FILENAME();

    if (file == NULL)
    {
      printf("Error opening file.txt\n");
      return -1;
    }
    else{
      fprintf(file, "%4.2f ms ;  %d\t\n", (double)(end - start)*1000/ticks, getpid());
      printf("Sucess \n");
    }
	return 0;
}

