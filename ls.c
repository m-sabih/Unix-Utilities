#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

extern int errno;
void do_ls(char *,char );
void show_stat_info(char *);

int myCompare (const void * a, const void * b ) {
    const char *pa = *(const char**)a;
    const char *pb = *(const char**)b;
    return strcmp(pa,pb);
}

int main(int argc,char* argv[]){
	if (argc == 1){
		do_ls(".",' ');
	}
	else{
      int opt;  
      while((opt = getopt(argc, argv, ":if:la")) != -1)  
    {  
        switch(opt)  
        {  
            case 'l':  
                for(; optind < argc; optind++){      
                 do_ls(argv[optind],'l'); 
               } 
                break;  
            case 'a':  
                for(; optind < argc; optind++){      
                 do_ls(argv[optind],'a'); 
               } 
                break;  
         }  
   }      
    for(; optind < argc; optind++){      
      do_ls(argv[optind],' ');

    } 
    
	}
	return 0;
}

void do_ls(char *dir,char op){
	struct dirent* entry;
   int rv = lstat(dir, &new);
	char *arr[50];
	int i=0;
	DIR *dp=opendir(dir);
	if(dp==NULL){
		printf("cannot open directory: %s\n",dir);
		return;
	}
		errno=0;
		while((entry =readdir(dp))!=NULL){
			if(entry==NULL && errno!=0){
				printf("ERROR \n");
				exit(errno);
			}
			else if(op!='a'){
				if(entry->d_name[0]=='.')
					continue;
			}
			arr[i]=entry->d_name;
			i=i+1;
		}
      if(op=='l'){
      for (int j=0;j<i;j++){
         chdir(dir);
         show_stat_info(arr[j]);
      }
   }
   else {
    qsort(arr, i, sizeof(char *), myCompare);
		 int n=0;
   		for( n = 0 ; n < i; n++ ) {   
      	printf("%s ", arr[n]);
   		}
   		printf("\n");
		closedir(dp);
   }
}

void show_stat_info(char *fname){
   struct stat info;
   struct passwd *psd;
   struct group *grp;
   int rv = lstat(fname, &info);
   if (rv == -1){
      perror("stat failed");
      exit(1);
   }

   char f_type[0];
   if ((info.st_mode &  0170000) == 0010000) {
		f_type[0]='p';
   }
   else if ((info.st_mode &  0170000) == 0020000) {
		f_type[0]='c';
   }

   else if ((info.st_mode &  0170000) == 0040000) {
		f_type[0]='d';
   }
   else if ((info.st_mode &  0170000) == 0060000) {
		f_type[0]='b';
   }
   else if ((info.st_mode &  0170000) == 0100000) {
		f_type[0]='-';
   }
   else if ((info.st_mode &  0170000) == 0120000) {
		f_type[0]='|';
   }
   else if ((info.st_mode &  0170000) == 0140000) {
		f_type[0]='s';
   }
   else{} 



   int mode = info.st_mode; 
   char str[10];
   strcpy(str, "---------");
//owner  permissions
   if((mode & 0000400) == 0000400) str[0] = 'r';
   if((mode & 0000200) == 0000200) str[1] = 'w';
   if((mode & 0000100) == 0000100) str[2] = 'x';
//group permissions
   if((mode & 0000040) == 0000040) str[3] = 'r';
   if((mode & 0000020) == 0000020) str[4] = 'w';
   if((mode & 0000010) == 0000010) str[5] = 'x';
//others  permissions
   if((mode & 0000004) == 0000004) str[6] = 'r';
   if((mode & 0000002) == 0000002) str[7] = 'w';
   if((mode & 0000001) == 0000001) str[8] = 'x';
//special  permissions
   if((mode & 0004000) == 0004000) str[2] = 's';
   if((mode & 0002000) == 0002000) str[5] = 's';
   if((mode & 0001000) == 0001000) str[8] = 't';
   
   //printf("\033[0;31m"); //Set the text to the color red
   //printf("Hello\n"); //Display Hello in red
   //printf("\033[0m");
   printf("%c",f_type[0]);
   printf("%s  ", str);
   printf("%ld  ", info.st_nlink);
   psd=getpwuid(info.st_uid);
   grp=getgrgid(info.st_gid);
   long sec=info.st_mtime;  
   printf("%s  ",psd->pw_name );
   printf("%s  ",psd->pw_name );
   printf("%ld  ", info.st_size);
   printf("%s   ", fname );
	printf("%s",ctime(&sec)); 
//	printf("\033[10C \033[1A \033[50C");
}
