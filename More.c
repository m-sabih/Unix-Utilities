#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>  
#include <errno.h>   
#include <sys/wait.h> 

#define linelen 512
#define pagelen 20

int file(FILE *,int,char *);
int get_input(int,int);
int countlines(FILE *);
void set_input_mode (void);
void reset_input_mode (void);

int main(int argc, char *argv[]){
//	int icount=0;
	//icount++;
	//printf("%d\n",icount );
	if(argc==1){
		FILE *fp=stdin;
		int count=0;
		count=countlines(fp);
		printf("%d\n", count);
		fseek(fp, 0, SEEK_SET);
		file(fp,count,".");
	}
	else{
		set_input_mode ();
		int i=0;
		int file_count;
		FILE *fp;
		while(++i < argc){
			fp=fopen(argv[i],"r");
			file_count=countlines(fp);
			fclose(fp);
			//printf("%s%d\n","Total line: ",file_count);
			fp=fopen(argv[i],"r");
			if(fp==NULL){
				perror("can't open file");
				exit(0);
			}
			file(fp,file_count,argv[i]);
			fclose(fp);
		}	
		//	return EXIT_SUCCESS;
	}

}
	

	struct termios saved_attributes;
	void reset_input_mode (void)
	{
  		tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
	}

	void set_input_mode (void)
	{
	  struct termios tattr;
	  char *name;
	  /* Make sure stdin is a terminal. */
	  if (!isatty (STDIN_FILENO))
	    {
	      fprintf (stderr, "Not a terminal.\n");
	      exit (EXIT_FAILURE);
	    }
	  /* Save the terminal attributes so we can restore them later. */
	  tcgetattr (STDIN_FILENO, &saved_attributes);
	  atexit (reset_input_mode);

	  /* Set the funny terminal modes. */
	  tcgetattr (STDIN_FILENO, &tattr);
	  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	  tattr.c_cc[VMIN] = 1;
	  tattr.c_cc[VTIME] = 0;
	  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
	}


	int countlines(FILE* fp){
  		//FILE *fp1 = fopen(fp,"r");
  		int ch=0;
  		int lines=0;
  		if (fp == NULL){
  			return 0;
  		}
  		while(!feof(fp))
		{
		  ch = fgetc(fp);
		  if(ch == '\n')
		  {
		    lines++;
		  }
		}
  		//fclose(fp1);
  		return lines-1;
	}


	int file(FILE *fp,int count,char *file_name){
		set_input_mode ();
		int line_count = 0;
		int t_lcount=0;
		int inp;
		char arr[linelen];
		while (fgets(arr,linelen,fp)){
			FILE* fp_tty=fopen("/dev/tty","r");
			if(fp==NULL){
				perror("can't open file");
				exit(0);
			}
			fputs(arr,stdout);
			line_count++;
			t_lcount++;
			if(line_count == pagelen){
				inp=get_input(count,t_lcount);
				if(inp == 0 ){
					printf("\033[1A \033[2K \033[1G");
					return EXIT_SUCCESS;
					break;
				}
				else if(inp == 1){
					printf("\033[1A \033[2K \033[1G");
					line_count=line_count-1;	//one line
				}
				else if(inp == 2){
					printf("\033[1A \033[2K \033[1G");
					line_count=0; //space
				}
				else if(inp == 4){
					printf("\033[1A \033[2K \033[1G");                              
//					    char buf[20];                                                          					                                        
//					    snprintf(buf, sizeof(buf), "vim %s",file_name);                                   
//					    system(buf); 
					pid_t  f;
					int status;
					f=fork(); 
					if(f==-1){
						printf("error\n");
					}
					if(f==0){
						execlp("vim","vim",file_name, NULL);
						exit(0);
					}
					wait(&status);
				
			}
				//else if(inp==4){
				//	char* s=fgets();
				//}
				//else if(inp == 5){
				//	printf("\033[1A \033[2K \033[1G");
//}
//		return EXIT_SUCCESS;
//					break;
//				}
			}
		}
		//printf("%d\n",line_count );

		return EXIT_SUCCESS;
	}
		int search_string(FILE *fp,char *find)
		{
			int count=0;
			char arr[500];
				while(!feof(fp))
				{
					fgets(arr,500,fp);
					if(strstr(arr,find))
						break;

					count++;

				}
		return count;		
		}
	int get_input(int count,int lcount){
		float per;
		per = (float)lcount / (float)count * 100.0;
		printf("\033[7m --more--(%.2f%%) \033[m",per);
		//printf("\033[1B");
		char u_input; //=getc(stream);
		printf("\n ");
		read (STDIN_FILENO, &u_input, 1);
		if(u_input=='q')
			return 0;
		if(u_input=='\n')
			return 1;
		if(u_input==' ')
			return 2;
		if(u_input=='/')
			return 3;
		if(u_input=='v' || 'V')
			return 4;
		else 
			return 5;
	}