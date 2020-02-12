#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int errno;
void do_find(char *,char *,char );
void show_stat_info(char *);

int main(int argc,char* argv[]){
	if (argc == 1){
		do_find(".","NULL",' ');
	}
  else if(argc==2){
    do_find(argv[1],"NULL",' ');
  }  
	else if(argc==4){
       if(strcmp(argv[2],"-name") == 0)
            do_find(argv[1],argv[3],'n'); 
        else if(strcmp(argv[2],"-type") == 0) 
            do_find(argv[1],argv[3],'t');          
        else
          printf("wrong option \n");
  }
  else{
    do_find(argv[1],"NULL",' ');
    printf(" 'Wrong arguments' \n");
  }
	return 0;
}

void do_find(char *dir,char *fname,char op){
	struct dirent* entry;
	DIR *dp=opendir(dir);
	if(dp==NULL){
		printf("cannot open directory: %s\n",dir);
		return;
	}
  if(op=='t'){
    printf("%s\n",dir);
  }
		errno=0;
    chdir(dir);
		while((entry =readdir(dp))!=NULL){
			if(entry==NULL && errno!=0){
				printf("ERROR \n");
				exit(errno);
			}
      if(op=='n'){
			   if (strcmp(entry->d_name,fname)==0){
          printf("/");
          printf("%s",dir);
          printf("/");
          printf("%s\n",fname );
          return;
        }
      }
      else if(op=='t'){
        struct stat info;
        char *name=entry->d_name;
        int rv = lstat(name, &info);
        if (rv == -1){
          perror("stat failed " );
          exit(1);
        }
        if (strcmp("p",fname)==0){
          if ((info.st_mode &  0170000) == 0010000) {
            printf("%s",dir);
            printf("/");
            printf("%s\n",entry->d_name);
         }
       }
        if (strcmp("c",fname)==0){ 
          if ((info.st_mode &  0170000) == 0020000) {
            printf("%s",dir);
            printf("/");
            printf("%s\n",entry->d_name);
         }
       }
        if (strcmp("d",fname)==0){ 
          if ((info.st_mode &  0170000) == 0040000) {
            printf("%s",dir);
            printf("/");
            printf("%s\n",entry->d_name);
         }
       }
          if (strcmp("b",fname)==0){ 
            if ((info.st_mode &  0170000) == 0060000){
              printf("%s",dir);
              printf("/");
              printf("%s\n",entry->d_name);
         }
       }
          if (strcmp("-",fname)==0){ 
            if ((info.st_mode &  0170000) == 0100000) {
              printf("%s",dir);
              printf("/");
              printf("%s\n",entry->d_name);
           }
       }
         if (strcmp("|",fname)==0){ 
           if ((info.st_mode &  0170000) == 0120000) {
              printf("%s",dir);
              printf("/");
              printf("%s\n",entry->d_name);
          }
       }
          if (strcmp("s",fname)==0){ 
            if ((info.st_mode &  0170000) == 0140000) {
              printf("%s",dir);
              printf("/");
              printf("%s\n",entry->d_name);
         }
        }
       }
      else{
        printf("%s",dir );
        printf("/");
         printf("%s\n", entry->d_name);
      }
    }
		closedir(dp);
  }