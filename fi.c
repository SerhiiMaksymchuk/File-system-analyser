#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <sys/statvfs.h>
#include <dirent.h>

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
	
	printf("Filesystem\t\t Size \t\t Used \t\t Avail \t\tMounted on\n");
	
       
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
		printf("%s \n",ent->mnt_dir);
	}
	
	endmntent(aFile);
}
