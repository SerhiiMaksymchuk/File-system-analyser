#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>

void *fileext(void *);

static double txt = 0, bin = 0, lib = 0;
static int txtcount = 0, bincount = 0, libcount = 0;

typedef struct {
	int thread_id;
	char mnt_dir[PATH_MAX];
	int is_main;
	int txt_n;
	int bin_n;
	int lib_n;
} parameters_t;

#define MAX_THREAD_NUMBER 20

int main(void)
{
	struct mntent *ent;
	struct statvfs buff;
	FILE *aFile;
	pthread_t threads[MAX_THREAD_NUMBER] = {};
	parameters_t thread_params[MAX_THREAD_NUMBER] = {};
	aFile = setmntent("/etc/mtab", "r");
	int i = 0, ret = 0;

	if (aFile == NULL)
	{
		perror("setmntent");
		ret = -1;
		
		goto Exit;
	}
	
	printf("Filesystem\t\t Size \t\t Used \t\t Avail \t\tMounted on\n");
       
	while (NULL != (ent = getmntent(aFile)))
	{

		if (statvfs(ent->mnt_dir, &buff)!=0)
		{
			perror("statvfs() error");
			ret = -1;
	
			goto Exit;
		}

		printf("%s \t\t", ent->mnt_fsname);
		printf("%ld \t\t", (buff.f_blocks * buff.f_frsize) / 1024);
		printf("%ld \t\t",  ((buff.f_blocks * buff.f_frsize) / 1024) - ((buff.f_bavail * buff.f_frsize) / 1024));
		printf("%ld \t\t", (buff.f_bavail * buff.f_frsize) / 1024);
		printf("%s \n", ent->mnt_dir);

		if (i >= MAX_THREAD_NUMBER)
		{
		         perror("fail to create thread\n");
			 parameters_t p_main;
			 p_main.thread_id = -1;
			 strcpy(p_main.mnt_dir, ent->mnt_dir);
			 p_main.is_main = 1;
			 fileext(&p_main);
		}
		else
		{
			thread_params[i].thread_id = i;
			strcpy(thread_params[i].mnt_dir, ent->mnt_dir);
			thread_params[i].is_main = 1;
			int pthc_ret = pthread_create(&threads[i], NULL, fileext, &thread_params[i]) ;
			if (pthc_ret)
			{
				perror("fail to create thread\n");
				ret = -1;
	
				goto Exit;
			}
		}
		i++;
	}

	// wait until all threads will be finished
	for (int k = 0; k <= i; k++)
		pthread_join(threads[k], NULL);

	txt = (txtcount / (double)(txtcount + bincount + libcount)) * 100;
	bin = (bincount / (double)(txtcount + bincount + libcount)) * 100;
	lib = (libcount / (double)(txtcount + bincount + libcount)) * 100;

	printf("*text documents %f%%\n*binary files %f%%\n*libraries(so,a) %f%%\n",txt ,bin, lib);	
	endmntent(aFile);

Exit:

	return ret;

}

void *fileext(void *param)
{
	parameters_t *ctx = param;	
	DIR *p;
	struct dirent *pp;
	int exit_in_the_end = 0;

	if (ctx->is_main)
	{
		exit_in_the_end = 1;
		ctx->is_main = 0;
	}

	p = opendir (ctx->mnt_dir);
	if (p != NULL)
	{
		while ((pp = readdir (p))!=NULL)
		{
			int length = strlen(pp->d_name);
			if (pp->d_type == DT_DIR && (strcmp(pp->d_name, ".") && 
				strcmp(pp->d_name, "..")))
			{
				char temp[PATH_MAX];

				strcpy(temp, ctx->mnt_dir);
				sprintf(ctx->mnt_dir, "%s%s%s", ctx->mnt_dir, "/", pp->d_name);

				fileext(ctx);
				strcpy(ctx->mnt_dir, temp);
			}
			
			if (strncmp(pp->d_name + length - 4, ".txt", 4) == 0)
				++ctx->txt_n;
			if (strncmp(pp->d_name + length - 4, ".bin", 4) == 0)
				++ctx->bin_n;
			if (strncmp(pp->d_name + length - 3, ".so", 3) == 0)
				++ctx->lib_n;
			if (strncmp(pp->d_name + length - 2, ".a", 2) == 0)
			        ++ctx->lib_n;
		}

	closedir (p);
	}

	if (exit_in_the_end)
	{
		libcount += ctx->lib_n;
		bincount += ctx->bin_n;
		txtcount += ctx->txt_n;

		pthread_exit(NULL);
	}
}
