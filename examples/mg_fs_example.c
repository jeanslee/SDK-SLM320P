#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'F', 'S')

#include "osi_api.h"
#include "osi_log.h"
#include "mg_fs_api.h"
#include "vfs.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


static void prvThreadEntry(void *param)
{
    build_envi();
    readdir("/test", true);
    read_and_write();
    // test_fstat_and_statvfs();
    remove_dir("/test");
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg fs enter");
    MG_osiThreadCreate("mgfs", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg fs exit");
}


//读取一个目录下的文件
//第二参数决定要不要进行目录切换操作
void readdir(const char *path, bool dir_change)
{
    //切换目录
    if (dir_change)
    {
        char curdir[64] = {0};
        if (NULL == MG_FS_GetCurDir(curdir, 64))
        {
            MG_osiTracePrintf(LOG_TAG, "get path fail");
            return;
        }
        else
        {
            MG_osiTracePrintf(LOG_TAG, "current dir path : %s", curdir);
        }

        if (-1 == MG_FS_ChangeDir(path))
        {
            MG_osiTracePrintf(LOG_TAG, "change dir fail");
            return;
        }
        else
        {
            curdir[0] = '\0';
            if (NULL == MG_FS_RealPath(".", curdir))
            {
                MG_osiTracePrintf(LOG_TAG, "get realpath fail");
                return;
            }
            else
            {
                MG_osiTracePrintf(LOG_TAG, "realpath of current dir: %s", curdir);
            }
        }
    }

    //打开目录
    DIR *dir = MG_FS_OpenDir(path);
    if (dir == NULL)
    {
        MG_osiTracePrintf(LOG_TAG, "OpenDir error");
        return;
    }
    else 
    {
        MG_osiTracePrintf(LOG_TAG, "OpenDir success"); 
    }

    //读目录
    struct dirent *temp = NULL;
    while (NULL != (temp = MG_FS_ReadDir(dir)))
    {
        MG_osiTracePrintf(LOG_TAG, "member of the dir : %s", temp->d_name);
    }

    //关闭目录
    int fd = MG_FS_CloseDir(dir);
    if (fd==-1)
    {
        MG_osiTracePrintf(LOG_TAG, "CloseDir error");
        return;
    }
    else 
    {
        MG_osiTracePrintf(LOG_TAG, "CloseDir success");
    }
}

//打开/创建文件并返回描述符
int open_file(const char *path)
{
    int fd = MG_FS_Creat(path, 777);
    if (fd == -1)
    {
        MG_osiTracePrintf(LOG_TAG, "CreatFile/OpenFile error");
        return -1;
    }
    else 
    {
        MG_osiTracePrintf(LOG_TAG, "CreatFile/OpenFile success");
        return fd;
    }
}

//关闭文件
void close_file(int fd)
{
    fd = MG_FS_Close(fd);
    if (fd == -1)MG_osiTracePrintf(LOG_TAG, "CloseFile error");
    else MG_osiTracePrintf(LOG_TAG, "CloseFile success");
}

//创建文件夹
void make_dir(const char *path)
{
    int fd = MG_FS_MakeDir(path, 777);
    if (fd == -1)MG_osiTracePrintf(LOG_TAG, "MakeDir error : dir already exist or incorrect path");
    else MG_osiTracePrintf(LOG_TAG, "MakeDir success");
}

//打开这个目录，遍历一遍，若目录非空则将其置空，然后移到根目录下，移除
void remove_dir(const char *path)
{
    DIR *dir = MG_FS_OpenDir(path);
    struct dirent * temp1 = (struct dirent *)malloc(sizeof(struct dirent));
    struct dirent * temp2 = (struct dirent *)malloc(sizeof(struct dirent));
    long offset = 0;
    int fd = 0;
    bool dir_empty = false;
    if (0 == MG_FS_ReadDir_r(dir, temp1, &temp2))
    {
        if (temp2 == NULL)
        {
            MG_osiTracePrintf(LOG_TAG, "dir is empty");
            dir_empty = true;
        }
    }
    else
    {
        MG_osiTracePrintf(LOG_TAG, "check dir fail");
    }
    
    if (!dir_empty)
    {
        char temp_path[64] = {0};
        offset = MG_FS_TellDir(dir);
        if (offset == -1)
        {
            MG_osiTracePrintf(LOG_TAG, "get dir offset fail");
        }
        else
        {
            MG_osiTracePrintf(LOG_TAG, "offset : %lu", offset);
        }
        
        MG_FS_SeekDir(dir, 0);
        while (0 == MG_FS_ReadDir_r(dir, temp1, &temp2))
        {
            if (temp2 == NULL)break;            
            else
            {
                sprintf(temp_path, "%s/%s", path, temp2->d_name);
                fd = MG_FS_Unlink(temp_path);
                if (fd == 0)
                {
                    MG_osiTracePrintf(LOG_TAG, "%s unlink success", temp2->d_name);
                }
                else
                {
                    MG_osiTracePrintf(LOG_TAG, "%s unlink fail", temp2->d_name);
                }
            }   
        }
    }

    MG_FS_CloseDir(dir);
    MG_FS_Rename(path, "/aa");

    readdir("/", false);
    if (MG_FS_RemoveDir("/aa") == -1)
    {
        MG_osiTracePrintf(LOG_TAG, "remove dir fail");
    }
    else
    {
        MG_osiTracePrintf(LOG_TAG, "remove dir success");
    }
    readdir("/", false);

    free(temp1);
    free(temp2);
}

//创建测试文件
void build_envi()
{
    char file_name[48] = {0};
    int fd = 0;
    make_dir("/test");
    for (int i = 0; i < 6; ++i)
    {
        sprintf(file_name, "/test/%s%d.%s", "test_file", i, "txt");
        fd = open_file(file_name);
        close_file(fd);
    }
}

//文件读写
void read_and_write()
{
    char buf[128] = {0};

    //通过文件路径读写
    int fd = MG_FS_Write("/test/test_file0.txt", "12345678910", sizeof("12345678910"));
    MG_osiTracePrintf(LOG_TAG, "write size = %d", fd);
    fd = MG_FS_Read("/test/test_file0.txt", buf, sizeof(buf));
    MG_osiTracePrintf(LOG_TAG, "read size = %d, buf = %s", fd, buf);

    //通过文件描述符读写
    bzero(buf, sizeof(buf));
    fd = MG_FS_Open("/test/test_file0.txt", O_WRONLY, 777);

    //从第二位开始读
    MG_FS_Seek(fd, 2, SEEK_SET);
    int size = vfs_read(fd, buf, sizeof(buf));
    MG_osiTracePrintf(LOG_TAG, "fd read size = %d, buf = %s", size, buf);

    //从第三位开始写
    MG_FS_Seek(fd, 3, SEEK_SET);
    size = vfs_write(fd, "aa", sizeof("aa")-1);
    MG_osiTracePrintf(LOG_TAG, "fd write size = %d", size);

    //从头开始读取
    bzero(buf, sizeof(buf));
    MG_FS_Seek(fd, 0, SEEK_SET);
    size = vfs_read(fd, buf, sizeof(buf));
    MG_osiTracePrintf(LOG_TAG, "fd read size = %d, buf = %s", size, buf);

    //截取长度后再次读取
    MG_FS_Ftruncate(fd, 4);
    MG_FS_Seek(fd, 0, SEEK_SET);
    bzero(buf, sizeof(buf));
    size = vfs_read(fd, buf, sizeof(buf));
    MG_osiTracePrintf(LOG_TAG, "fd read size = %d, buf = %s", size, buf);

    close_file(fd);
}

void test_fstat_and_statvfs(){
    MG_osiThreadSleep(3000);
    struct stat st = {0,};

    //int result = MG_FS_Stat("/test/test_file0.txt", &st)
    // if (result == -1){
    //     MG_osiTracePrintf(LOG_TAG, "get file stat error");
    //     return;
    // }
    // else{
    //     MG_osiTracePrintf(LOG_TAG, "protect mode : %d", st.st_mode);
    //     MG_osiTracePrintf(LOG_TAG, "file size(byte) : %lu", st.st_size);
    // }

    int fd = open_file("/test/test_file0.txt");

    int result = MG_FS_Fstat(fd, &st);
    if (result == -1){
        MG_osiTracePrintf(LOG_TAG, "get file stat fail");
        fd = MG_FS_Close(fd);
        if (fd == -1)MG_osiTracePrintf(LOG_TAG, "CloseFile error");
        else MG_osiTracePrintf(LOG_TAG, "CloseFile success");
        return;
    }
    else{
        MG_osiTracePrintf(LOG_TAG, "protect mode : %d", st.st_mode);
        MG_osiTracePrintf(LOG_TAG, "file size(byte) : %lu", st.st_size);
    }
    
    close_file(fd);

    struct statvfs stvfs = {0,};
    result = MG_FS_Statvfs("/test/test_file0.txt", &stvfs);
    if (result != -1){
        MG_osiTracePrintf(LOG_TAG, "file system id : %lu", stvfs.f_fsid);
        MG_osiTracePrintf(LOG_TAG, "file system block size : %lu", stvfs.f_bsize);
    }
    else{
        return;
    }
    
    fd = open_file("/test/test_file0.txt");
    
    result = MG_FS_Fstatvfs(fd, &stvfs);
    if (result != -1){
        MG_osiTracePrintf(LOG_TAG, "file system id : %lu", stvfs.f_fsid);
        MG_osiTracePrintf(LOG_TAG, "file system block size : %lu", stvfs.f_bsize);
    }
    else{
        return;
    }
    
    close_file(fd);
}






