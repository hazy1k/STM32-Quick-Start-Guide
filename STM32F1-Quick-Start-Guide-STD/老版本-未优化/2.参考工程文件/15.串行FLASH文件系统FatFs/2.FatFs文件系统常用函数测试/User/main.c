#include "stm32f10x.h"
#include "usart.h"
#include "ff.h"
#include "string.h"

FATFS fs; // FatFs文件系统对象
FIL fnew; // 新创建的文件对象
FRESULT res_flash; // 文件操作结果
UINT fnum; // 文件成功读写数量
char fpath[100]; // 文件路径
char readbuffer[512]; // 读写缓冲区

// FatFs基础功能测试
static FRESULT miscellaneous(void)
{
  DIR dir; // 目录文件
  FATFS *pfs; // 文件系统对象
  DWORD fre_clust, fre_sect, tot_sect; // 剩余簇数、剩余扇区数、总扇区数

  /* 设备信息获取 */
  printf("第一项测试：设备信息获取\r\n");
  res_flash = f_getfree("1:", &fre_clust, &pfs); // 函数参数：驱动器号、剩余簇数指针、文件系统对象指针
  // 计算得到总扇区个数和空簇大小
  tot_sect = (pfs->n_fatent-2)*pfs->csize;
  fre_sect = fre_clust * pfs->csize;
  printf("设备可用总空间：%10lu KB \n 设备剩余空间：%10lu KB \n", tot_sect*4, fre_sect*4);

  /* 进行文件系统格式化和写入 */
  printf("第二项测试：文件系统格式化和写入\r\n");
  res_flash = f_open(&fnew, "1:FatFS读写测试文件.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
  if(res_flash == FR_OK)
  {
    // 文件定位
    res_flash = f_lseek(&fnew, f_size(&fnew)); // 函数参数：文件对象、文件指针位置
    if(res_flash == FR_OK)
    {
      // 格式化写入
      f_printf(&fnew, "在原来文件新添加一行内容！\r\n");
      f_printf(&fnew, "设备总空间：%10lu KB \n 设备剩余空间：%10lu KB \n", tot_sect*4, fre_sect*4);

      // 文件定位到起始位置
      res_flash = f_lseek(&fnew, 0);
      res_flash = f_read(&fnew, readbuffer, f_size(&fnew), &fnum);
      if(res_flash == FR_OK)
      {
        printf("文件内容：\r\n%s", readbuffer);
      }
    }
    f_close(&fnew); // 关闭文件

    /* 目录创建和重命名功能测试 */
    printf("第三项测试：目录创建和重命名功能测试\r\n");
    res_flash = f_opendir(&dir, "1:TestDir");
    if(res_flash != FR_OK)
    {
      res_flash = f_mkdir("1:TestDir"); // 创建目录
    }
    else
    {
      // 如果目录已经存在，关闭它
      res_flash = f_closedir(&dir);
      f_unlink("1:TestDir/testdir.txt");
    }
    if(res_flash == FR_OK)
    {
      // 重命名并移动文件
      res_flash = f_rename("1:FatFS读写测试文件.txt","1:TestDir/testdir.txt");
    }
  }
  else
  {
    printf("文件打开失败: %d\r\n", res_flash);
    printf("文件系统格式化失败！\r\n");
  }
  return res_flash;
}

FILINFO fno; // 文件信息对象

// 文件获取
static FRESULT file_check(void)
{
  res_flash = f_stat("1:TestDir/testdir.txt", &fno);
  if(res_flash == FR_OK)
  {
    printf("testdir.txt文件信息:\n");
    printf("文件大小: %ld(字节)\n", fno.fsize);
        printf("时间戳信息: %u/%02u/%02u, %02u:%02u\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("文件属性: %c%c%c%c%c\n\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',  // 目录
           (fno.fattrib & AM_RDO) ? 'R' : '-',  // 只读
           (fno.fattrib & AM_HID) ? 'H' : '-',  // 隐藏
           (fno.fattrib & AM_SYS) ? 'S' : '-',  // 系统
           (fno.fattrib & AM_ARC) ? 'A' : '-'); // 存档
  }
  return res_flash;
}

// 递归扫描FatFs内的文件
static FRESULT scan_files (char* path)
{ 
  FRESULT res; 	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;   
	
#if _USE_LFN 
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      res = f_readdir(&dir, &fno); 								
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 	
      if (*fn == '.') continue; 	   
      if (fno.fattrib & AM_DIR)         
			{ 			    
        sprintf(&path[i], "/%s", fn); 		      
        res = scan_files(path);	
        path[i] = 0;              
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);     
      }
    }
  } 
  return res; 
}

int main(void)
{
  USART_Config();
  res_flash = f_mount(&fs, "1:", 1); // 挂载FatFs文件系统
  if(res_flash != FR_OK)
  {
    printf("文件系统挂载失败！\r\n");
    while(1);
  }
  else{
    printf("FatFs文件系统初始化成功！\r\n");
  }
  printf("FatFs文件系统常用函数测试\r\n");
  res_flash = miscellaneous(); // FatFs基础功能测试
  printf("文件扫描测试");
  res_flash = file_check();
  strcpy(fpath, "1:");
  scan_files(fpath);
  f_mount(NULL, "1:", 1); // 卸载FatFs文件系统
  while(1);
}


