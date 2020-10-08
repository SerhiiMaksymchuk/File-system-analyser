#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <string.h>


void fileext(char *mnt_dir);

float txt = 0, bin = 0, lib = 0;
static int txtcount = 0, bincount = 0, libcount = 0;

int main(void)
{
	struct mntent *ent;
	struct statvfs buff;
	FILE *aFile;
	aFile = setmntent("/etc/mtab", "r");
	if (aFile == NULL)
	{
		perror("setmntent");
		exit(1);
	}
	
	printf("Filesystem\t Size \t Used \t Avail \tMounted on\n");
	
       
	while (NULL != (ent = getmntent(aFile)))
	{

		if (statvfs(ent->mnt_dir, &buff)!=0)
		{
			perror("statvfs() error");
			exit(1);
		}

		printf("%s \t\t", ent->mnt_fsname);
		printf("%ld \t\t", (buff.f_blocks * buff.f_frsize) / 1024);
		printf("%ld \t\t",  ((buff.f_blocks * buff.f_frsize) / 1024) - ((buff.f_bavail * buff.f_frsize) / 1024));
		printf("%ld \t\t", (buff.f_bavail * buff.f_frsize) / 1024);
		printf("%s \n", ent->mnt_dir);
		fileext(ent->mnt_dir);
		
	
//	printf("*text documents -%d", txtcount);
	}	
	endmntent(aFile);
}
void fileext(char *mnt_dir)
{
	DIR *p;
	struct dirent *pp;
	p = opendir (mnt_dir);
	if (p != NULL)
	{
		while ((pp = readdir (p))!=NULL)
		{
			if (pp->d_type == DT_DIR && (strcmp(pp->d_name, ".") && 
				strcmp(pp->d_name, "..")))
			{
#if 0
				int path_len = strlen(mnt_dir);
				int file_name_len = strlen(pp->d_name);
				/* +1 for null symbol */
				int new_path_len = path_len + file_name_len + 1;
#endif

				int len = snprintf(NULL, 0, "%s%s%s", mnt_dir, "/", pp->d_name);

				char *new_path = (char*)calloc(1, sizeof(char) * (len + 1));

				sprintf(new_path, "%s%s%s", mnt_dir, "/", pp->d_name);

				printf("THE FILE IS A DIRECTORY, path is: %s\n", new_path);
#if 0
				strncpy(new_path, mnt_dir, path_len);
				printf("step 1:, path is: %s\n", new_path);
				strncpy(new_path + path_len, pp->d_name, strlen(pp->d_name));

				printf("THE FILE IS A DIRECTORY, path is: %s\n", new_path);
#endif
				fileext(new_path);

				free(new_path);
			}
			
			int length = strlen(pp->d_name);
			if (strncmp(pp->d_name + length - 4, ".txt", 4) == 0)
				++txtcount;
				printf("%s\n",pp->d_name);
			if (strncmp(pp->d_name + length - 4, ".bin", 4) == 0)
				++bincount;
				printf("%s\n",pp->d_name);
			if (strncmp(pp->d_name + length - 3, ".so", 3) == 0)
				++libcount;
				printf("%s\n",pp->d_name);
			if (strncmp(pp->d_name + length - 2, ".a", 2) == 0)
			        ++libcount;
				printf("%s\n",pp->d_name);
		}
	
	(void) closedir (p);
	
//	txt = (txtcount / (txtcount + bincount + libcount)) * 100;
//	bin = (bincount / (txtcount + bincount + libcount)) * 100;
//	lib = (libcount / (txtcount + bincount + libcount)) * 100;
	
	}

}

