#include "ty_flash_adpt.h"
#include "ty_os_adpt.h"

#define USER_FLASH_BASE_BAKCUP_OFFSET 0x1000

#define  TEST_FILE  "hello_test.bin"
#define  BAK_TEST_FILE  "bakhello_test.bin"

bool ty_flash_check_factory()
{
  //  return opencpu_file_is_exist(TEST_FILE);
  int fd = MG_FS_Creat(EXAMPLE_FILE_NAME,664);
}

void ty_flash_read(void *data, int size)
{
    opencpu_file_read(TEST_FILE, data, size);
}


int ty_flash_write(void *data, int size)
{

    return opencpu_file_write(TEST_FILE, data, size);
    
}


//user备份分区写入数据
int ty_flash_write_backup(void *data, int size)
{
    return opencpu_file_write(BAK_TEST_FILE, data, size);
}

//user备份分区读取数据
void ty_flash_read_backup(void *data, int size)
{
    opencpu_file_read(BAK_TEST_FILE, data, size);
}



