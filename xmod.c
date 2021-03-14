#include "xmod.h"

static void signal_func(int);

int InitializeArguments(int argc, char *argv[], struct Arguments *args)
{
    if (argc < 3 || argc > 6 )
    {
        fprintf(stdout, "Usage: xmod [OPTIONS] MODE FILE/DIR\n");
        fprintf(stdout, "Usage: xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return -1;
    }

    int num_options = argc-3;

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
                fprintf(stdout, "Available options:\n   -v\n  -c\n  -R\n");
                return -1;
            }
            strcpy(args->options[j], argv[j+1]);
        }

    } 

    //verifies mode
    char *mode = argv[num_options + 1];
	bool mode_is_octal = false;
    if (mode[0] == '0') //mode in octal
    { 
        if (strlen(mode) != 4)
        {
            fprintf(stdout, "Invalid mode!\n");
            return -1;
        }
        for (int i = 1; i < 4; i++)
        {
            if ((mode[i] < '0') || (mode[i] > '7'))
            {
                fprintf(stdout, "Invalid mode!\n");
                return -1;
            }
        }
		mode_is_octal = true;
    }
    else
    {
        char dominio[8];
        strcpy(dominio,"augo");
        if(strchr(dominio, mode[0]) == NULL)
        {
            fprintf(stdout, "Invalid mode!\n");
            return -1;
        }
        
    }
    args->mode = mode;
	args->mode_is_octal = mode_is_octal;

    //verifies if file exists
    FILE *file = fopen(argv[num_options + 2],"r");
    if(file == NULL)
    {
        fprintf(stdout, "Error opening file!\n");
        return -1;
    }
    fclose(file);
    args->file_path = argv[num_options + 2];

    //prints arguments
    fprintf(stdout, "Options:\n");
    for (size_t k = 0; k < num_options; k++)
    {
        fprintf(stdout, "%s\n", args->options[k]);
    }
    fprintf(stdout, "mode: %s\n", args->mode);
	fprintf(stdout, "mode is octal: %d\n", args->mode_is_octal);
    fprintf(stdout, "file_path: %s\n", args->file_path);

    return 0;
}

int GetFilePermissions(const char *pathname)
{
  int perm = 0;
  struct stat fileattrib;
  int file_mode;

  if (stat(pathname, &fileattrib) == 0)
  {
    file_mode = fileattrib.st_mode;
    // Check owner permissions 
    if ((file_mode & S_IRUSR)) //&& (fileMode & S_IREAD))
      perm += 256;
    if ((file_mode & S_IWUSR))// && (fileMode & S_IWRITE))
      perm += 128;
    if ((file_mode & S_IXUSR))// && (fileMode & S_IEXEC))
      perm += 64;
    if (file_mode & S_IRGRP) //&& (fileMode & S_IREAD))
      perm += 32;
    if (file_mode & S_IWGRP) //&& (fileMode & S_IWRITE))
      perm += 16;
    if (file_mode & S_IXGRP) //&& (fileMode & S_IEXEC))
      perm += 8;
    if (file_mode & S_IROTH) //&& (fileMode & S_IREAD))
      perm += 4;
    if (file_mode & S_IWOTH) //&& (fileMode & S_IWRITE))
      perm += 2;
    if (file_mode & S_IXOTH) //&& (fileMode & S_IEXEC))
      perm += 1;
  }
  else
    perm = -1; 

 return perm;
}

int GetNewPermMask(char *new_mode)
{
  char users = new_mode[0];
  int new_perm = 0;
  if (strchr("augo",new_mode[0]))
  { 
    /*if (strchr("-+=",NewMode[0]))
      users='t';
    printf("P1\n");*/  
    switch (users)
	{
      case 'u':
          for(int i = 2; i < strlen(new_mode) ; i++)
          {
            if(new_mode[i] == 'r')
              new_perm += 256;
            else if(new_mode[i] == 'w')
              new_perm += 128;
            else if(new_mode[i] == 'x')
              new_perm += 64;  
          }
        break;
      case 'g':
          for(int i = 2; i < strlen(new_mode); i++)
          {
            if(new_mode[i]=='r')
              new_perm += 32;
            else if(new_mode[i]=='w')
              new_perm += 16;
            else if(new_mode[i]=='x')
             new_perm += 8;  
          }
        break;
      case 'o':
          for(int i = 2; i< strlen(new_mode); i++)
          {
            if(new_mode[i] == 'r')
              new_perm += 4;
            else if(new_mode[i]=='w')
               new_perm += 2;
            else if(new_mode[i]=='x')
              new_perm += 1;  
          } 
        break;
      case 'a':
          for(int i = 2; i < strlen(new_mode); i++)
          {
            if(new_mode[i] == 'r')
              new_perm += 256 + 32 + 4;
            else if(new_mode[i]=='w')
              new_perm += 128 + 16 + 2;
            else if(new_mode[i]=='x')
              new_perm += 64 + 8 + 1;  
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
  
  //printf("NewPerm: %d", NewPerm);
  return new_perm;
}

int GetNewPermissions(int form_perm, char *new_mode)
{
	int new_perm;
	int new_mask = GetNewPermMask(new_mode);
	char operator;

	if (strchr("augo",new_mode[0]))
    	operator=new_mode[1];
  	else
    	operator=new_mode[0];

	//printf("NewMask: %d",new_mask);  
  	switch(operator)
	{
    	case '+':
        	new_perm = form_perm | new_mask;
      		break; 
    	case '-':
        	new_perm = form_perm & ~new_mask;
      		break;
    	case '=':
        	new_perm = new_mask;
      		break;
    	default:
        	return -1;
  	}
  return new_perm;
}


FILE* GetRegistsFile()
{
    const char* s;
    s = getenv("LOG_FILENAME");

    if(s == NULL)
    {
      printf("getenv returned NULL\n");
      return NULL;
      
    }
    FILE* file = fopen(s, "w");
    return file;    
}

//signal handler
static void signal_func(int signo){
  char term;
  fprintf(stdout, "\n%d\t\n", getpid()/*, agrs.file_path,*/); //tem de se dar print do file_path do nº de ficheiros encontrados e o nº de ficheiros modificados
  fprintf(stdout,"Exit or continue program? (E/C)");
  scanf("%c", &term);
  if(term == 'C' || term == 'c'){
    
  }
  else if(term == 'E' || term == 'e'){
    exit(EXIT_SUCCESS);
  }
  else{
    printf("Error! Please enter a valid character.\n");
    exit(EXIT_FAILURE);
  }
}


int main( int argc, char *argv[], char *envp[])  
{
    struct Arguments args;
    clock_t start, end;
    struct tms t;
    long ticks;

    //check if CTRL + C was pressed
    signal(SIGINT, signal_func);


    //starts counting time
    start = times(&t);
    ticks = sysconf(_SC_CLK_TCK);

    fprintf(stdout, "\n------------- Introduced arguments -------------\n");
    if(InitializeArguments(argc, argv, &args) == 1)
    {
        fprintf(stdout, "Something went wrong! Closing...\n");
        return -1;
    }
    fprintf(stdout, "\n----------------- Permissions ------------------\n");
    int actual_perm = GetFilePermissions(args.file_path);
    fprintf(stdout, "Actual permissions: %o\n", actual_perm);
    int new_perm;
    if (args.mode[0] == '0')
      new_perm = strtoul(args.mode, NULL, 8);
    else
    {  
      new_perm = GetNewPermissions(actual_perm, args.mode);
      fprintf(stdout, "New permissions: %o\n", new_perm); 
    }

    //infinte cicle to check CTRL + C signal
    for( ; ;);

    //changes the permissions
    int c = chmod(args.mode, new_perm);
    if(c == -1)
    {
      fprintf(stdout, "Something went wrong! Closing...\n");
      return -1; 
    } 

    //file with regists
    FILE* file = GetRegistsFile();

    if (file == NULL)
    {
      fprintf(stdout, "Error opening file.txt\n");
      return -1;
    }

    ticks = ticks;
    end = times(&t);

    //puts informations on the regists file
    fprintf(file, "%4.2f ms ;  %d\t\n", (double)(end - start)*1000/ticks, getpid());

    fclose(file);
    fprintf(stdout, "\nSuccess \n");
	return 0;
}

