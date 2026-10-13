#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
int main()
{
	char com[100];
	int pid;
	while(1)
	{   
		printf("my shell>");
		scanf(" %[^\n]",com);
		printf("commond is:%s\n",com);
		char *p=com;
		int c=0;
		char *args[20];
		args[c]=p;
		c++;
		while(*p!='\0')
		{
			if(*p==' ')
			{
				*p='\0';
				p++; 
				args[c]=p;
				c++;
			}

			p++;
		}

		args[c]=NULL;

		if(strcmp(args[0],"cd")==0) // for cd
		{
			if(args[1]!=NULL)
			{
				if( chdir(args[1])==-1)
					perror("chdir");
				else
					printf("child success\n");
			}
			continue;         
		}
		if(strcmp(args[0],"exit")==0) // for exit
		{
			printf("exit \n");
			break;
		}

		if(c>2)// for > ,>>
			if((strcmp(args[1],">")==0)|(strcmp(args[1],">>")==0))
			{
				int fd,flag;
				if(strcmp(args[1],">>")==0)
					fd=open(args[2],O_WRONLY|O_CREAT|O_APPEND,0664);
				else
					fd=open(args[2],O_WRONLY|O_CREAT|O_TRUNC,0664);
				int p[2];
				if(pipe(p)==-1)
				{
					perror("pipe");
					return 0;
				} 
				if(fork()==0)
				{
					close(1);
					close(p[0]);
					//close(0);
					dup(p[1]);
					char *argc[2]={args[0],NULL};
					execvp(argc[0],argc);
				}
				else
				{ 

					close(p[1]);
					dup(p[0]);
					close(1);
					close(0);
					char s[100];
					int n;
					while( (n=read(p[0],s,sizeof(s)))>0)
						write(fd,s,n);

					wait(0);
					// wait(0);
					close(fd);
					break;  

				}
			}
		if (c > 2)//<
		{
			if (strcmp(args[1], "<") == 0)
			{
				int fd = open(args[2], O_RDONLY);

				if (fd == -1)
				{
					perror("open");
					continue;
				}

				if (fork() == 0)
				{
					dup2(fd, 0);
					close(fd);

					args[1] = NULL;
					execvp(args[0], args);

					perror("execvp");
					return 1;
				}
				else
				{
					close(fd);
					wait(0);
				}

				continue;
			}
		}
		/*
		   if(c>2)
		   {
		   int f=0,k=0,i=0,j=0;
		   while(args[k]!=0)
		   {
		   if(strcmp(args[k],"|")==0)
		   {
		   f=1;
		   break;
		   }
		   k++;
		   }
		   k=0;
		   if(f==1)
		   {
		   char *args1[20],*args2[20];
		   while(strcmp(args[k],"|")!=0)
		   {
		   args1[i]=args[k];
		   k++;i++;
		   }
		   args1[i]=NULL;
		   k++;

		   while(args[k])
		   {
		   args2[j]=args[k];
		   k++;j++;
		   }
		   args2[j]=NULL;

		 *for (i = 0; args1[i] != NULL; i++)
		 printf("args1[%d] = [%s]\n", i, args1[i]);

		 for (j = 0; args2[j] != NULL; j++)
		 printf("args2[%d] = [%s]\n", j, args2[j]);


		 int p[2];
		 pipe(p);
		 if(fork()==0)
		 {
		//ps command
		close(p[0]);
		dup2(p[1],1);
		close(p[1]);
		execvp(args1[0],args1);
		perror("execvp");
		_exit(1);

		}
		else
		{
		if(fork()==0)
		{
		//greap command
		close(p[1]);
		dup2(p[0],0);
		close(p[0]);
		execvp(args2[0],args2);
		perror("execvp");
		_exit(1);
		}
		else
		{
		close(p[0]);
		close(p[1]);
		wait(0);
		wait(0);
		continue;
	}
	}
	}

	}*/




	if (c > 2)
	{
		int f = 0, k = 0, i = 0, j = 0;

		while (args[k] != NULL)
		{
			if (strcmp(args[k], "|") == 0)
			{
				f = 1;
				break;
			}
			k++;
		}

		if (f == 1)
		{
			char *args1[20], *args2[20];

			k = 0;

			while (strcmp(args[k], "|") != 0)
			{
				args1[i++] = args[k++];
			}
			args1[i] = NULL;
			k++;

			while (args[k] != NULL)
			{
				args2[j++] = args[k++];
			}
			args2[j] = NULL;

			/* Check output redirection in second command */
			int outfd = -1;
			int r = 0;

			for (j = 0; args2[j] != NULL; j++)
			{
				if (strcmp(args2[j], ">") == 0 ||
						strcmp(args2[j], ">>") == 0)
				{
					if (args2[j + 1] == NULL)
					{
						printf("Missing output filename\n");
						r = 1;
						break;
					}

					if (strcmp(args2[j], ">") == 0)
					{
						outfd = open(args2[j + 1],
								O_WRONLY | O_CREAT | O_TRUNC,
								0664);
					}
					else
					{
						outfd = open(args2[j + 1],
								O_WRONLY | O_CREAT | O_APPEND,
								0664);
					}

					if (outfd == -1)
					{
						perror("open");
						r = 1;
						break;
					}

					args2[j] = NULL;
					break;
				}
			}

			if (r)
			{
				if (outfd != -1)
					close(outfd);
				continue;
			}

			int p[2];

			if (pipe(p) == -1)
			{
				perror("pipe");
				if (outfd != -1)
					close(outfd);
				continue;
			}

			pid = fork();

			if (pid == 0)
			{
				close(p[0]);
				dup2(p[1], 1);
				close(p[1]);

				if (outfd != -1)
					close(outfd);

				execvp(args1[0], args1);
				perror("execvp");
				_exit(1);
			}

			if (pid < 0)
			{
				perror("fork");
				close(p[0]);
				close(p[1]);
				if (outfd != -1)
					close(outfd);
				continue;
			}

			int pid2 = fork();

			if (pid2 == 0)
			{
				close(p[1]);
				dup2(p[0], 0);
				close(p[0]);

				if (outfd != -1)
				{
					dup2(outfd, 1);
					close(outfd);
				}

				execvp(args2[0], args2);
				perror("execvp");
				_exit(1);
			}

			close(p[0]);
			close(p[1]);

			if (outfd != -1)
				close(outfd);

			if (pid2 < 0)
				perror("fork");

			waitpid(pid, NULL, 0);

			if (pid2 > 0)
				waitpid(pid2, NULL, 0);

			continue;
		}
	}

	if((pid=fork())==0)//for normal command
	{
		args[c]=NULL;
		execvp(args[0],args);
		perror("execvp");
	}
	else
	{
		waitpid(pid,0,0);
	}


	}
	return 0;
}
