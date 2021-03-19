#include "./xmod.h"


int diff_ms(struct timeval t2, struct timeval t1  )
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

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
			fprintf(stderr, "invalid character");
			break;
		case 3:
			fprintf(stderr, "unable to set masks");
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

void WriteLog(FILE *regists_file, double time, int pid, char event[], char info[])
{
    fprintf(regists_file,"%4.2f ms ;  %d\t ; %s\t ; %s \n", time, getpid(), event, info);
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
    // Check permissions 
    if ((file_mode & S_IRUSR)) 
      perm += 256;
    if ((file_mode & S_IWUSR))
      perm += 128;
    if ((file_mode & S_IXUSR))
      perm += 64;
    if (file_mode & S_IRGRP) 
      perm += 32;
    if (file_mode & S_IWGRP)
      perm += 16;
    if (file_mode & S_IXGRP)
      perm += 8;
    if (file_mode & S_IROTH) 
      perm += 4;
    if (file_mode & S_IWOTH) 
      perm += 2;
    if (file_mode & S_IXOTH) 
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
      default:  
	  	PrintError(3);
        break;     
    }
  }
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

  	switch(operator)
	{
    	case '+':
        	new_perm = form_perm | new_mask;
      		break; 
    	case '-':
        	new_perm = form_perm & ~new_mask;
      		break;
    	case '=':
          if(new_mode[0]=='a')
        	  new_perm = new_mask;
          else {
            if(new_mode[0]=='u'){
              new_perm = form_perm & 63;
              new_perm = new_perm | new_mask;
            }
            if(new_mode[0]=='g'){
              new_perm = form_perm & 455;
              
              new_perm = new_perm | new_mask;
             
            }
            if(new_mode[0]=='x'){
              new_perm = form_perm & 504;
              new_perm = new_perm| new_mask;
            }
          }  
      		break;
    	default:
			PrintError(7);
        	return -1;
  	}
  return new_perm;
}

void OctalToMode(int octal, char *mode)
{
  char snum[4]; 
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

void ChangePermissions(const struct Arguments *args, char *path)
{
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
        OctalToMode(new_perm, mode);
        fprintf(stdout, "mode of '%s' retained as 0%o (%s)\n", path, new_perm, mode);
        nftot++;
    }
    else if (actual_perm != new_perm && (args->option_c || args->option_v)) 
	{
		OctalToMode(actual_perm, mode);
        fprintf(stdout, "mode of '%s' changed from 0%o (%s) ", path, actual_perm, mode);
        OctalToMode(new_perm, mode);
        fprintf(stdout, "to 0%o (%s)\n", new_perm, mode);
        nftot++;
        nfmod++;
    }
	return;

}

static void ExitHandler(int signo)
{
  raise(SIGKILL);
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

			argv[argc-1] = path_;
			
      gettimeofday(&curr_time, NULL);
			pid_t child_pid = fork();
			int status;
			if (child_pid == 0)
			{
				if(execve("./xmod.o", argv, envp) == -1)
					perror("execve");
				return;
			}
			else
			{
        WriteLog(GetRegistsFile(),diff_ms(start,curr_time),getpid(),"PROC_CREATE", argv[0]);
				while(wait(&status)>0) ; //makes the parent waits for his child to finish
			}
		}
		free(path_);
	}

}

static void SignalFunc(int signo)
{
  char *buffer = NULL;
  size_t n;
  
    gettimeofday(curr_time,start);
    WriteLog(GetRegistsFile(),diff_ms(curr_time,start),getpid(), "SIGNAL_RECV", strsignal(SIGINT));
      if(getpid() == getpgid(getpid()))
    {
      fprintf(stdout,"%d ; %s ; %d ; %d\n",getpid(), args.path_name, nftot, nfmod);
        while(1)
      {
              sleep(5);
              printf("Exit or continue program? (E/C)");
              getline(&buffer,&n,stdin);
              if(strncasecmp(buffer, "E", 1) == 0)
          {
                gettimeofday(&curr_time,NULL);
                WriteLog(GetRegistsFile(),diff_ms(curr_time,start), getpid(), "SIGNAL_SENT",strsignal(SIGUSR1));
                raise(SIGUSR1);
                break;
              }
        else if(strncasecmp(buffer, "C", 1)==0)
        {
                gettimeofday(&curr_time,NULL);
                WriteLog(GetRegistsFile(),diff_ms(curr_time,start),getpid(), "SIGNAL_SENT", strsignal(SIGCONT));
                raise(SIGCONT);
                break;
              }
          }
      }
    else
    {
        fprintf(stdout,"\n%d ; %s ; %d ; %d\n",getpid(),args.path_name, nftot, nfmod);
        gettimeofday(&curr_time, NULL);
        WriteLog(GetRegistsFile(), diff_ms(curr_time, start), getpid(), "SIGNAL_SENT", strsignal(SIGSTOP));
        raise(SIGSTOP);
      }
}
  
int main(int argc, char *argv[], char *envp[])  
{
    //start counting time
    gettimeofday(&start, NULL);
    struct sigaction sig;

    sig.sa_handler = SignalFunc;
    sig.sa_flags = 0;
    sigemptyset(&(sig.sa_mask));
    sigaddset(&(sig.sa_mask), SIGINT);
    sigaddset(&(sig.sa_mask), SIGUSR1);
    sigaddset(&(sig.sa_mask), SIGTSTP);
    sigaddset(&(sig.sa_mask), SIGCONT);

    //check if CTRL + C was pressed
    sigaction(SIGINT,&sig, NULL);

    signal(SIGSTOP,SIG_DFL);
    signal(SIGUSR1,ExitHandler);
    signal(SIGCONT, SIG_DFL);

    InitializeArguments(argc, argv, &args);

	if(!args.option_R)
	{
		ChangePermissions(&args, args.path_name);
  	}
	else 
	{
		ProcessRecursive(argc, argv, envp, &args);
	}

    //file with regists
    FILE* regists_file = GetRegistsFile();

    gettimeofday(&end,NULL);
    WriteLog(GetRegistsFile(),diff_ms(end,start),getpid(),"PROC_EXIT", "0");

    fclose(regists_file);

	return 0;
}