#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define com_len 512
#define args 10
#define arg_len 30
#define PROMPT "cs321shell@"

int jobs[100];
int job_index=0;

char* read_cmd(FILE*,char *);
char **tokens(char *);
char *execute(char* arrlist[],int background);
void execute_pipe(char* ,int);
void pipe_cmd(char** cmd1, char** );
void InFile(char* cmdline);
int getCommandFromFile(char* cmdline);
void child_handler(int);
void ttin_handler(int);
int builtin_cd(char* arlist[]);
void builtin_job();
void builtin_help(char *);
void builtin_kill(char * pid);
int a1,a2;

int main(int argc,char *argv[]){
	char **n;
	if(argv[1] == "-f"){
		if((n=tokens(argv[2]))!= NULL)
			execute(n,0);

	}
	else{
   a1=dup(0);
   a2=dup(1);
   printf("Enter any command \n");
   char* cmdline;
   char** res;
   char *prompt=PROMPT;
   char colon=';';
   int check=2,colon_count=0,m=0;
   FILE *file_p = fopen("commands.txt","r+");
   while((cmdline=read_cmd(stdin,prompt))!= NULL){
   	if(cmdline[0]=='\0'){
   		continue;
   	}
   else{
   	//occurences of ;
   	while(cmdline[m] != '\0')
    {
      	if(cmdline[m] == colon){
            colon_count++;
        }
        m++;
    }

	   char **total_commands=(char**)malloc(sizeof(char*)*colon_count+1);
   		for (int t=0;t<colon_count;t++){
      		total_commands[t]=(char*)malloc(sizeof(char)*arg_len);
      		bzero(total_commands[t],arg_len);
   		}
      char* colon_token;
      int commands_count=0;
      colon_token=strtok(cmdline,";");
      while(colon_token!=NULL){
        total_commands[commands_count]=colon_token;
        commands_count++;
        colon_token=strtok(NULL,";");
      }
      for(int com=0;com<commands_count;com++){
	   //printf("count %d\n", f_count);
	   char* ncmdline=(char*) malloc(sizeof(char*)*strlen(total_commands[com]));
	   	   int background=0;
	      char *re;
	     re = strchr(total_commands[com], '|');
	     int indexx=re-total_commands[com];
	     int len=strlen(total_commands[com]);

	     //background
	     if(total_commands[com][len-1]=='&'){
	     	background=1;
	     	for(int p=0;p<len-2;p++){
	     		ncmdline[p]=total_commands[com][p];
	     	}
	     }
	     else{
	     	for(int p=0;p<len;p++){
	     		ncmdline[p]=total_commands[com][p];
	     	}
	     }
	         	   InFile(ncmdline);
	     //pipe
	      if(re != NULL){
	         execute_pipe(ncmdline,indexx);
	         wait(NULL);
	      }
	      else{
	      	char first;
	      	first=ncmdline[0];
	      	printf("first    %c\n", first );
	         if((res=tokens(ncmdline))!= NULL){      
	          	//printf("%s\n",res[0] );
	         	if (strcmp(res[0],"cd") == 0) 
	         		builtin_cd(res);
	         	else if (strcmp(res[0],"exit") == 0) 
	         		exit(0);
	         	else if (strcmp(res[0],"jobs") == 0) 
	         		builtin_job();
	         	else if (strcmp(res[0],"help") == 0)	         		
	         		builtin_help(res[1]);
	         	else if (strcmp(res[0],"kill") == 0)	         	
	         		builtin_kill(res[1]);
	            else if (first == '!')	{
	            	printf("hmmmmmmm\n");         	
	         		getCommandFromFile(ncmdline);	     
	         	}    		
	         	else{
	         	//printf("%s\n",ncmdline );
	            execute(res,background);
	        	}
	            free(res);
	            free(ncmdline);

	         }
      	}
  	  }
  	  //for(int a=0;a<commands_count;a++)
  	    //   free(total_commands[a]);
  	   free(total_commands);
  	}
   }
}
}

void InFile(char* cmdline){
    FILE* fp=fopen("commands.txt","a+");
    char str[1024], last[1024];
    char n[10]="\0";
    fseek(fp,0,SEEK_SET);
    int flag=0;
    while(!feof(fp)){
        flag=1;
        fgets(str,1024,fp);
        strncpy(last,str,1024);
    }
    int no=0;
    if(flag==1){
        int i=0;
        while(last[i]!=' '){
            n[i]=last[i];
            i++;
        }
        no=atoi(n);
    }
    no=no+1;
    fprintf(fp, "%d    ", no);
    fprintf(fp,"%s\n",cmdline );
    fclose(fp);
}

int getCommandFromFile(char* cmdline){
    FILE* fp=fopen("history.txt","a+");
    char type='\0';
    int totalno=0;
    char n[10]="\0";
    if(cmdline[1]!='-'){
        int i=1,k=0;
        while(cmdline[i]!='\0'){
            n[k]=cmdline[i];
            i++;k++;
        }

        type='+';
    }
    else {
        int i=2,k=0;
        while(cmdline[i]!='\0'){
            n[k]=cmdline[i];
            i++;k++;
        } 
        type='-';
        fseek(fp,0,SEEK_SET);
        char last[1024],s[1024],no[10];
        while(!feof(fp)){
            fgets(s,1024,fp);
            strncpy(last,s,1024);
        }
        i=0;
        while(last[i]!=' '){
            no[i]=s[i];
            i++;
        }   
        totalno=atoi(no);
        totalno++;
    }
    int no=atoi(n);
    if(type=='-')
        no=totalno-no;
    char s[1024];
    fseek(fp,0,SEEK_SET);
    while(!feof(fp)){
        fgets(s,1024,fp);
        char n2[10]="\0";
        int i=0;
        while(s[i]!=' '){
            n2[i]=s[i];
            i++;
        }   
        int no2=atoi(n2);
        if(no2==no){
            int k=0;
            while(k<100){
                cmdline[k]='\0';
                k++;
            }
            while(s[i]==' ')
                i++;
            k=0;
            while(s[i]!='\0'){
                cmdline[k]=s[i];
                k++;i++;
            }
            cmdline[--k]='\0';
            fclose(fp);
            return 1;
        }
        //strncpy(last,s,1024);
    }
    fclose(fp);
    return -1;
}


char* read_cmd(FILE* fp,char *prompt){
   char buf[255];
   getcwd(buf, sizeof(buf));
   printf("\033[32m\033[1m%s\033[39m\033[m",prompt);
   printf("\033[34m\033[1m%s\033[39m\033[m:- ",buf);
   int c;
   int pos=0;
   char* cmdline=(char*) malloc(sizeof(char)*com_len);
   signal(SIGINT,SIG_IGN);
   while((c=getc(fp)) != EOF ){
      if(c=='\n')
         break;
      cmdline[pos++]=c;
   }
    if(c == EOF && pos == 0) {
    	printf("\n");
      return NULL;
  }
   cmdline[pos]='\0';
   return cmdline;
}

char **tokens(char *cmdline){
   char* token;
   token=strtok(cmdline," ");
   char **commands=(char**)malloc(sizeof(char*)*args+1);
   for (int i=0;i<args;i++){
      commands[i]=(char*)malloc(sizeof(char)*arg_len);
      bzero(commands[i],arg_len);
   }
   int in,out;
    int j=0;
   while(token!=NULL){
      if(strstr(token,"<")){
      token = strtok (NULL, " ");
      in = open(token, O_RDONLY);
      dup2(in, STDIN_FILENO);
	  close(in);
      token = strtok (NULL, " ");
      continue;
      }
      else if(strstr(token,">")){
      token = strtok (NULL, " ");
      out = open(token, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
      dup2(out, STDOUT_FILENO);
	 close(out);
      token = strtok (NULL, " ");
      continue;
      }
      commands[j]=token;
      j++;
      token=strtok(NULL," ");
      
      }
   commands[j]=NULL;
   return commands;
}

char* execute(char* arrlist[],int background){
   signal(SIGINT,SIG_IGN);
   int status;
   if(background==1)
   {
   	    signal(SIGTTIN,ttin_handler);
		signal(SIGTTOU,ttin_handler);
   }
   int cpid=fork();
   switch(cpid){
      case -1:
         perror("fork failed");
         exit(1);
      case 0:
      if(background==1)
      	 setpgrp();
         signal(SIGINT,SIG_DFL);
         execvp(arrlist[0],arrlist);
         perror("command not found");
         exit(1);
      default:
        if (background == 0){
        	waitpid(cpid, &status, 0);
         	//printf("child exited with status %d \n", status >> 8);
         	dup2(a1,0);
         	dup2(a2,1);
    	}
    	else{
       	printf("Process created with PID: %d\n",cpid);
		signal(SIGCHLD,child_handler);
       	jobs[job_index]=cpid;
       	job_index++;
       	}
       return 0;
   }
}

void execute_pipe(char* cmdline,int index){
   char **arr1=(char**)malloc(sizeof(char*)*args+1);
   for (int i=0;i<args;i++){
      arr1[i]=(char*)malloc(sizeof(char)*arg_len);
      bzero(arr1[i],arg_len);
   }
   char **arr2=(char**)malloc(sizeof(char*)*args+1);
   for (int k=0;k<args;k++){
      arr2[k]=(char*)malloc(sizeof(char)*arg_len);
      bzero(arr2[k],arg_len);
   }
      char* token;
      int len,ind=0,j=0;
      token=strtok(cmdline," ");
      arr1[ind]=token;
      len=strlen(token);
      j=j+len+1;ind++;
      while(token!=NULL){
         if(j==index){
            arr1[ind]=NULL;
            break;}
      token=strtok(NULL," ");
      arr1[ind]=token;
      len=strlen(token);
      j=j+len+1;ind++;}
      ind=0;
      token=strtok(NULL," ");
      while(token!=NULL){
         token=strtok(NULL," ");
            if(token==NULL)
               break;
         arr2[ind]=token;
         len=strlen(token);
         ind++;
      }
      arr2[ind]=NULL;
      int pid=fork();
      if(pid==0)
      pipe_cmd(arr1,arr2);
  }

     
void pipe_cmd(char** cmd1, char** cmd2) {
  int fds[2]; 
  int status;
  pipe(fds);
  pid_t pid;
 pid = fork();
      if (pid == 0)
      { 
      close(fds[1]); 
      dup2(fds[0], STDIN_FILENO); 
      close(fds[0]); 
      execvp(cmd2[0], cmd2);
      }
      else
      {
      close(fds[0]); 
      dup2(fds[1], STDOUT_FILENO); 
      close(fds[1]);
      execvp(cmd1[0], cmd1);
      }
  }

void ttin_handler(int n){
	close(0);
	int fp=open("/dev/tty",O_RDONLY);
	printf("Stopped %d",getpid());
	kill(getpid(),SIGSTOP);
}

void child_handler(int n){
	while(waitpid(-1,NULL,WNOHANG)>0){}
	printf("\n");
}

int builtin_cd(char* arlist[]){
	if (arlist[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	else{ 
		if (chdir(arlist[1]) == -1) {
			printf(" %s: no such directory\n", arlist[1]);
            return -1;
		}
	}
	return 0;
}

void builtin_job(){
	if(job_index>0){
		for(int i=0;i<job_index;i++){
			printf("[%d]    %d\n",i+1, jobs[i]);
		}
	}
}

void builtin_help(char *arg){
	if (strcmp(arg,"cd") == 0) 
		printf("cd command is used to change directories\n");
	else if (strcmp(arg,"jobs") == 0) 
		printf("jobs command is used to tell about the background processes\n");
	else if (strcmp(arg,"exit") == 0) 
		printf("exit is used to exit shell\n");
	else if (strcmp(arg,"kill") == 0) 
		printf("kill command is used to kill a process with a specific id\n");
}

void builtin_kill(char* pid){
	int n=atoi(pid);
	kill(n, SIGKILL);
}

