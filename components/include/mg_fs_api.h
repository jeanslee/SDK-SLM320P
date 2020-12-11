#include "vfs.h"
#include <errno.h>

/**
 * @brief：关闭文件
 * @input：fd 文件描述符
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Close(int fd);

/**
 * @brief：关闭目录
 * @input：pdir 一个与目录相关联的DIR指针
 * @return：成功返回0，失败返回-1
*/
int MG_FS_CloseDir(DIR *pdir);

/**
 * @brief：打开一个文件，若不存在则创建
 * @input：path 文件路径
 * @input：mode 文件模式
 * @return：成功返回文件描述符，失败返回-1
*/
int MG_FS_Creat(const char *path, mode_t mode);

/**
 * @brief：通过文件描述符获取文件状态
 * @input：fd 文件描述符
 * @output：st 用于存放文件状态信息的结构体
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Fstat(int fd, struct stat *st);

/**
 * @brief：通过文件描述符截取文件到指定长度
 * @input：fd 文件描述符
 * @input：length 指定长度
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Ftruncate(int fd, long length);

/**
 * @brief：创建一个目录
 * @input：name 目标路径
 * @input：mode 模式
 * @return：成功返回0，失败返回-1
*/
int MG_FS_MakeDir(const char *name, mode_t mode);

/**
 * @brief：打开一个文件，若不存在就创建
 * @input：path 文件路径
 * @input：flags 标志位
 * @return：成功返回描述符，失败返回-1
*/
int MG_FS_Open(const char *path, int flags, ...);

/**
 * @brief：打开目录
 * @input：name 目录路径
 * @return：一个和目录关联的DIR指针，失败返回NULL
*/
DIR *MG_FS_OpenDir(const char *name);

/**
 * @brief：读取文件
 * @input：path 文件路径
 * @output：det 存放读取内容的数组
 * @input：size 读取最大长度
 * @return：实际的读取长度，失败返回-1
*/
ssize_t MG_FS_Read(const char *path, void *dst, size_t size);

/**
 * @brief：写数据到文件
 * @input：path 文件路径
 * @input；data 待写数据
 * @input：size 写入的最大长度
 * @return：实际写入长度，失败返回-1
*/
ssize_t MG_FS_Write(const char *path, const void *data, size_t size);

/**
 * @brief：解除文件的一个连接，当连接数为0时回删除文件
 * @input：path 目标文件路径
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Unlink(const char *path);

/**
 * @brief：通过路径截取文件到指定长度
 * @input：path 文件路径
 * @input：length 指定的长度
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Truncate(const char *path, long length);

/**
 * @brief：通过路径获取文件系统的统计信息
 * @input：path 目标文件系统路径
 * @output：buf 用于存储文件系统的统计信息的statvfs结构
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Statvfs(const char *path, struct statvfs *buf);

/**
 * @brief：通过路径获取文件状态信息
 * @input：path 文件路径
 * @output：st 用于存储文件状态信息的stat结构
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Stat(const char *path, struct stat *st);

/**
 * @brief：指定文件夹遍历的下一个位置
 * @input：pdir 文件夹的DIR指针
 * @loc：目标位置
 * @attention：配合readdir系列函数使用
*/
void MG_FS_SeekDir(DIR *pdir, long loc);

/**
 * @brief：复位文件夹遍历的下一个位置，相当于MG_FS_SeekDir(pdir，0)
 * @input：pdir 文件的DIR指针
*/
void MG_FS_RewindDir(DIR *pdir);

/**
 * @brief：自定义文件读写的偏移位置
 * @input：fd 文件描述符
 * @input：offset 偏移量
 * @input：mode 偏移起点
 * @return：成功返回当前偏移位置，失败返回-1
*/
off_t MG_FS_Seek(int fd, off_t offset, int mode);

/**
 * @brief：重命名文件
 * @input：src 原文件名
 * @input：dst 新文件名
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Rename(const char *src, const char *dst);

/**
 * @brief：遍历文件夹下的内容
 * @input：pdir 目标文件夹的DIR指针
 * @output：entry 输出
 * @output：out_dirent 输出
 * @return：成功返回0，失败返回-1
*/
int MG_FS_ReadDir_r(DIR *pdir, struct dirent *entry, struct dirent **out_dirent);

/**
 * @brief：遍历文件夹下的内容
 * @input：pdir 目标文件夹的DIR指针
 * @return：成功返回结果指针，失败返回NULL
*/
struct dirent *MG_FS_ReadDir(DIR *pdir);

/**
 * @brief：获取当前文件夹遍历的偏移量
 * @input：pdir 文件夹的DIR指针
 * @return：成功返回偏移量，失败返回-1
*/
long MG_FS_TellDir(DIR *pdir);

/**
 * @brief：获取当前所在目录
 * @output：buf 用于存储路径的数组
 * @input：size 数组大小
 * @return：返回数组指针，失败返回NULL
*/
char* MG_FS_GetCurDir(char *buf, size_t size);

/**
 * @brief：移除目录
 * @input：所要移除的目录的路径
 * @return：成功返回0，失败返回-1
*/
int MG_FS_RemoveDir(const char *name);

/**
 * @brief：获取绝对路径
 * @input：path 给定的路径
 * @output：resolved_path 根据给定路径得到的绝对路径
 * @return：成功返回绝对路径，失败返回NULL
*/
char* MG_FS_RealPath(const char *path, char *resolved_path);

/**
 * @brief：切换目录
 * @input：path 目标目录路径
 * @return：成功返回0，失败返回-1
*/
int MG_FS_ChangeDir(const char *path);

/**
 * @brief：通过文件描述符获取文件系统统计信息
 * @input：fd 文件描述符
 * @output：buf 用于存储文件系统统计信息的statvfs结构
 * @return：成功返回0，失败返回-1
*/
int MG_FS_Fstatvfs(int fd, struct statvfs *buf);

/**
 * @brief：用描述符读取文件
 * @input：fd 文件描述符
 * @output：det 存放读取内容的数组
 * @input：size 读取最大长度
 * @return：实际的读取长度，失败返回-1
*/
ssize_t MG_FS_Fread(int fd, void *dst, size_t size);

/**
 * @brief：用描述符写文件
 * @input：fd 文件描述符
 * @output：data 待写数据
 * @input：size 写入最大长度
 * @return：实际的写入长度，失败返回-1
*/
ssize_t MG_FS_Fwrite(int fd, const void *data, size_t size);

// 卸载文件系统
int MG_FS_Unmount(const char *path);

//不支持
int MG_FS_Link(const char *oldpath, const char *newpath);




