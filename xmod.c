#include "./xmod.h"


struct Arguments args;
int nfmod = 0;
int nftot = 0;


pid_t filhos[10];
int nfilhos=0;

static void signal_func(int);

void PrintManual()
{
	fprintf(stdout, "Usage: ./xmod.o [OPTIONS] MODE FILE/DIR\n");
	fprintf(stdout, "   or: xmod [OPTIONS] OCTAL-MODE FILE/DIR\n\n");
	fprintf(stdout, "   -c: like verbose but report only when a change is made\n");
	fprintf(stdout, "   -v: output a diagnostic for every file processed\n");
	fprintf(stdout, "   -R: change files and directories recursively\n\n");
	fprintf(stdout, "MODE is of the form '<u|g|o|a><-|+|=><rwx>' or '0[0-7][0-7][0-7]'\n");
	exit(0);
}

void PrintError(int error)
{
	fprintf(stderr, "xmod: ");
	switch (error)
	{
		case 1:
			fprintf(stderr, "missing operand");
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			fprintf(stderr, "invalid mode");
			break;
		case 5:
			fprintf(stderr, "no such file or directory");
			break;
		case 6:
			fprintf(stderr, "no LOG_FILENAME environment variable");
			break;
		case 7:
			fprintf(stderr, "unable to get file permissions");
			break;
		case 8:
			fprintf(stderr, "unable to set file permissions");
			perror("");
			break;
		default:
        {
			switch (errno)
			{
      			case ENOENT:
        			fprintf(stderr, "the file does not exist.");
      				break;
      			case EACCES: 
        			fprintf(stderr, "search permission is denied on a component of the path prefix.");
      				break;
      			case EFAULT:
        			fprintf(stderr, "pathname points outside your accessible address space.");
      				break;
      			case EIO:
        			fprintf(stderr, "an I/O error occurred.");
      				break;
      			case ELOOP:
        			fprintf(stderr, "too many symbolic links were encountered in resolving pathname.");
      				break;
      			case ENAMETOOLONG:
        			fprintf(stderr, "pathname is too long.");
      				break;
      			case ENOMEM:
        			fprintf(stderr, "insufficient kernel memory was available.");
      				break;
      			case ENOTDIR:
        			fprintf(stderr, "a component of the path prefix is not a directory.");
      				break;
      			case EPERM:
        			fprintf(stderr, "the effective UID does not match the owner of the file, and the process is not privileged, or\nThe file is marked immutable or append-only.");
      				break;
      			case EROFS:
        			fprintf(stderr, "the named file resides on a read-only filesystem.\n");
      				break;
      			case EBADF:
        			fprintf(stderr, "the file descriptor fd is not valid.\n");
      				break;
      			default:
        			fprintf(stderr, "an error ocurred.\n");
      				break;
    		}  

		}

	}
	
	fprintf(stderr, "\nTry './xmod.o --help' for more information.\n");
	exit(1);
}


FILE* GetRegistsFile()
{
    const char* s;
    s = getenv("LOG_FILENAME");

    if(s == NULL)
		PrintError(6);

    FILE* file = fopen(s, "w+");
    return file;    
}

void InitializeArguments(int argc, char *argv[], struct Arguments *args)
{
    if (argc < 2 || argc > 6 )
		PrintError(1);

	if(strcmp(argv[1], "--help") == 0)
	{
		PrintManual();
	}

    int num_options = argc - 3;
	args->option_v = false;
	args->option_c = false;
	args->option_R = false;

    if(num_options >= 1 && num_options <= 3)
	{
		for(size_t j = 0; j < num_options; j++)
        {
            if(strcmp(argv[j+1], "-v") != 0 && strcmp(argv[j+1], "-c") != 0 && strcmp(argv[j+1],"-R") != 0) 
			{
				PrintError(1);
			}
            if(strcmp(argv[j+1], "-v") == 0)
			{
				args->option_v = true;
			} 
			if(strcmp(argv[j+1], "-c") == 0)
			{
				args->option_c = true;
			}
			if(strcmp(argv[j+1], "-R") == 0) 
			{
				args->option_R = true;
			}
        } 

	}
	else
		PrintError(1);

    //verifies mode
    char *mode = argv[num_options + 1];
    if (mode[0] == '0') //mode in octal
    { 
        if (strlen(mode) != 4)
            PrintError(4);

        for (int i = 1; i < 4; i++)
        {
            if ((mode[i] < '0') || (mode[i] > '7'))
                PrintError(4);
        }
		args->mode_is_octal = true;
		args->mode_octal = strtoul(mode, NULL, 8);
    }
    else
    {
        char dominio[8];
        strcpy(dominio,"augo");
        if(strchr(dominio, mode[0]) == NULL)
            PrintError(4);
        
        if(strchr("+-=", mode[1]) == NULL) 
            PrintError(4);
         
        for(int i=2; i<strlen(mode);i++)
		{
          if(strchr("rwx", mode[i]) == NULL)
            PrintError(4);
        }
		args->mode_is_octal = false;
		args->mode = mode;
        
    }

    //verifies if file exists
    FILE *file = fopen(argv[num_options + 2],"r");
    if(file == NULL)
    {
        PrintError(5);
    }
    fclose(file);
    args->path_name = argv[num_options + 2];

    return;
}

int GetFilePermissions(const char *path_name)
{
  int perm = 0;
  struct stat fileattrib;
  int file_mode;

  if (stat(path_name, &fileattrib) == 0)
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
	PrintError(7);

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

void oct_to_mode(int octal, char *mode)
{
  char snum[4]; //alteramos 3->4
  snprintf(snum, sizeof(snum), "%o", octal);
  int octal_n = atoi(snum);
  int temp;
  for (int i = 0; i != 3; i++) 
  {
    if (i == 0) temp = octal_n/100;
    else if (i == 1) temp = (octal_n/10) - ((octal_n/100)*10);
    else if (i == 2) temp = octal_n - ((octal_n/10)*10);

    if ( temp >= 4 ){
      temp -= 4;
      mode[i*3] = 'r';
    }
    else mode[i*3] = '-';
    if (temp >= 2){
      temp -= 2;
      mode[i*3+1] = 'w';
    }
    else mode[i*3+1] = '-';
    if (temp == 1){
      mode[i*3+2] = 'x';
    }
    else mode[i*3+2] = '-';
  }
}


void ChangePermissions(const struct Arguments *args, char *path){
	int actual_perm = GetFilePermissions(path);
    int new_perm;
    if (args->mode_is_octal == true)
      new_perm = args->mode_octal;
    else
    {  
      new_perm = GetNewPermissions(actual_perm, args->mode);
    }
    //changes the permissions
    int c = chmod(path, new_perm);
    if(c == -1)
		PrintError(8);
    //-v or -c implementation
    char mode[10] = "";
    if (actual_perm == new_perm && args->option_v) {   
        oct_to_mode(new_perm, mode);
        fprintf(stdout, "mode of '%s' retained as 0%o (%s)\n", path, new_perm, mode);
        nftot++;
    }
    else if (actual_perm != new_perm && (args->option_c || args->option_v)) 
	{
		oct_to_mode(actual_perm, mode);
        fprintf(stdout, "mode of '%s' changed from 0%o (%s) ", path, actual_perm, mode);
        oct_to_mode(new_perm, mode);
        fprintf(stdout, "to 0%o (%s)\n", new_perm, mode);
        nfmod++;
        nftot++;
    }
	return;

}

void ProcessRecursive(int argc, char *argv[], char *envp[], const struct Arguments *args)
{
	DIR *dir = opendir(args->path_name); //opens directory
	struct dirent *file;

	while ((file = readdir(dir)) != NULL)
	{
		struct stat path_stat;
		char *path_ = malloc(sizeof(args->path_name) + sizeof('/') + sizeof(file->d_name));
		sprintf(path_, "%s/%s", args->path_name, file->d_name);
		stat(path_, &path_stat);


		//if is a regular file ("base case")
		if (S_ISREG(path_stat.st_mode)) 
		{
			ChangePermissions(args, path_);
		}
        //verififies if is a directory and ignores the parent directory (..) and the current directory (.)
		else if (S_ISDIR(path_stat.st_mode) && strcmp(file->d_name, "..") != 0 && strcmp(file->d_name, ".") != 0)
		{
			ChangePermissions(args, path_);
			char *newargv[argc+1];
			newargv[0] = "./xmod";
			for (size_t i = 1; i <= argc-3; i++)
			{
				if(args->option_c)
					newargv[i] = "-c";
				else if(args->option_v)
					newargv[i] = "-v";
				else if(args->option_R)
					newargv[i] = "-R";
			}
			if(args->mode_is_octal)
				sprintf(newargv[argc-2],"%d",args->mode_octal);
			else if(!args->mode_is_octal)
				newargv[argc-2] = args->mode;
			
			newargv[argc-1] = path_;

			newargv[argc] = NULL;

			
			//pid_t child_pid = fork();
			int status;
			if ( (filhos[nfilhos++] = fork()) == 0)
			{

        setpgid(getpid(), getpid());
        //Initialize_child_handler();
        

        /*
        for (int i = 0; i < 10; i++)
        {  sleep(1); printf("%d %d\n", getpid(), i); } */
        sleep(10);
				if(execve("./xmod", newargv, envp) == -1)
					perror("execve");

			}
			else
			{
        
        signal(SIGINT, signal_func);

				while(wait(&status)>0) ; //makes the parent waits for his child to finish
			}
		}
		free(path_);
	}
  
}

//signal handler;
static void signal_func(int signo) {
 size_t i;
  char *buffer = NULL;
  size_t n;

  switch (signo) {

    case SIGINT:

      fprintf(stdout,"\n%d ; %s ; %d ; %d\n", getpid(), args.path_name, nftot, nfmod);

      do {

        printf("Exit or continue program? (E/C)");
        getline(&buffer,&n,stdin);

        if(strncasecmp(buffer, "E", 1)==0) {

            for (i = 0; i < nfilhos; i++) {
              kill(filhos[i], SIGKILL);
            }

          
          }

      } while(strncasecmp(buffer, "C", 1)!=0 && strncasecmp(buffer, "E", 1)!=0);

      break;
    }



}

static void exit_handler(int signo){
  exit(0);
}


void Initialize_main_handler(){
    struct sigaction sig, usr1, tstp, cont;

    sig.sa_handler = signal_func;
    sig.sa_flags = 0;
    sigemptyset(&(sig.sa_mask));
    sigaddset(&(sig.sa_mask), SIGINT);
    sigaddset(&(sig.sa_mask), SIGUSR1);
    sigaddset(&(sig.sa_mask), SIGUSR2);
    sigaddset(&(sig.sa_mask), SIGTSTP);
    sigaddset(&(sig.sa_mask), SIGCONT);
    //check if CTRL + C was pressed
    sigaction(SIGINT,&sig, NULL);

    usr1.sa_handler = exit_handler;
    usr1.sa_flags = 0;
    sigemptyset(&(usr1.sa_mask));
    sigaction(SIGUSR1, &usr1, NULL);

    tstp.sa_handler = SIG_IGN;
    tstp.sa_flags = 0;
    sigemptyset(&(tstp.sa_mask));
    sigaction(SIGTSTP, &tstp, NULL);


    cont.sa_handler = SIG_IGN;
    cont.sa_flags = 0;
    sigemptyset(&(cont.sa_mask));
    sigaction(SIGCONT, &cont, NULL);
}

void Initialize_child_handler(){
  struct sigaction ign_child, tstp_child, cont_child;

  ign_child.sa_handler = SIG_IGN;
  ign_child.sa_flags = 0;
  sigemptyset(&(ign_child.sa_mask));
  sigaction(SIGINT, &ign_child, NULL);

  tstp_child.sa_handler = SIG_DFL;
  tstp_child.sa_flags = 0;
  sigemptyset(&(tstp_child.sa_mask));
  sigaction(SIGTSTP, &tstp_child, NULL);

  cont_child.sa_handler = SIG_DFL;
  cont_child.sa_flags = 0;
  sigemptyset(&(cont_child.sa_mask));
  sigaction(SIGCONT, &cont_child, NULL);

}

void WriteLog(FILE *regists_file, double time, int pid, char info[]){
    fprintf(regists_file,"%4.2f ms ;  %d\t ; %s\t ; \n", time, getpid(), info);
}

int main(int argc, char *argv[], char **envp)  
{
    clock_t start, end;
    struct tms t;
    long ticks;
  
    //starts counting time
    start = times(&t);
    ticks = sysconf(_SC_CLK_TCK);

    //Initialize_main_handler();

    InitializeArguments(argc, argv, &args);
	if(!args.option_R)
	{
		ChangePermissions(&args, args.path_name);
  	}
	ProcessRecursive(argc, argv, envp, &args);

    //file with regists
    FILE* regists_file = GetRegistsFile();

    ticks = ticks;
    end = times(&t);

    //puts informations on the regists file
    //fprintf(regists_file, "%4.2f ms ;  %d\t\n", (double)(end - start)*1000/ticks, getpid());
    WriteLog(regists_file,(double)(end - start)*1000/ticks, getpid(),"TESTE");

    fclose(regists_file);

    unsetenv("parentpid");

	printf("fim processo \n");
	return 0;
}