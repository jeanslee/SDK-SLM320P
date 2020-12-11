/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')



#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "drv_lcd.h"

#include "vfs.h"
#include "cfw.h"
#include "drv_adc.h"
#include "oem.h"
#include "oem_lcd.h"
osiThread_t * meig_baolan_lcd_Handle=NULL;

uint16_t BACK_COLOR=0xffff;    //背景色
uint16_t POINT_COLOR=0x0000;   //画笔色


const unsigned char hanzi_mei[]={/*"美",0*/
0x08,0x20,0x06,0x60,0x02,0x88,0x7F,0x0FC,0x01,0x00,0x3F,0xF8,0x01,0x00,0xFF,0xFE,
0x01,0x00,0x01,0x08,0x7F,0xFC,0x01,0x00,0x02,0x80,0x04,0x60,0x18,0x1E,0x0E0,0x04,
};
const unsigned char hanzi_ge[]={/*格",0*/
0x11,0x00,0x11,0x00,0x11,0xF8,0x12,0x08,0xFF,0x10,0x14,0xA0,0x38,0x40,0x34,0xA0,
0x51,0x10,0x52,0x0E,0x97,0xFC,0x1A,0x08,0x12,0x08,0x12,0x08,0x13,0xF8,0x12,0x08
};
const unsigned char hanzi_zhi[]={/*智",0*/
0x20,0x00,0x22,0x04,0x3F,0x7E,0x48,
0x44,0x08,0x44,0xFF,0xC4,0x10,0x44,
0x14,0x7C,0x22,0x44,0x42,0x10,0x9F,
0xF8,0x10,0x10,0x1F,0xF0,0x10,0x10,
0x1F,0xF0,0x10,0x10
};

const unsigned char hanzi_neng[]={/*能",0*/
0x10,0x80,0x10,0x88,0x24,0x98,0x44,
0xA0,0xFE,0xC0,0x02,0x84,0x7C,0x84,
0x44,0x7C,0x7C,0x00,0x44,0x88,0x44,
0x98,0x7C,0xA0,0x44,0xC0,0x44,0x82,
0x54,0x82,0x48,0x7E
};

uint8_t service_str[] ={0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x20,0x6f,0x6b,0};
uint8_t no_service_str[] ={0x6e,0x6f,0x20,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0};
uint8_t have_data_str[] ={0x67,0x6f,0x74,0x20,0x64,0x61,0x74,0x61,0};
uint8_t no_data_str[] ={0x6e,0x6f,0x20,0x64,0x61,0x74,0x61,0};

//osiTimer_t * g_csq_timer =NULL;//delete by zx 20200916
extern int g_net_total_time_ms;

static void Address_set(uint8_t xsta,uint8_t ysta,uint8_t xend,uint8_t yend)
{

    uint16_t left = xsta;
    uint16_t right = xend;
    uint16_t top = ysta;
    uint16_t bot = yend;

    display_cmd(0x2a);                 // set hori start , end (left, right)
    display_data((left >> 8) & 0xff);  // left high 8 b
    display_data(left & 0xff);         // left low 8 b
    display_data((right >> 8) & 0xff); // right high 8 b
    display_data(right & 0xff);        // right low 8 b

    display_cmd(0x2b);               // set vert start , end (top, bot)
    display_data((top >> 8) & 0xff); // top high 8 b
    display_data(top & 0xff);        // top low 8 b
    display_data((bot >> 8) & 0xff); // bot high 8 b
    display_data(bot & 0xff);        // bot low 8 b

    display_cmd(0x2c); // recover memory write mode

}\
static void LCD_WR16(uint16_t da)
{
	char tmp[2], *buf;
	buf=(char* )&da;
	tmp[1]=*buf;
	tmp[0]=*(buf+1);
	display_data(tmp[1]);
	display_data(tmp[0]);
}
//画点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR16(POINT_COLOR);
}

void LCD_DrawPoint_color(uint16_t x,uint16_t y,uint16_t da)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR16(da);
}

//画线
//x0,y0:起点坐标
//x1,y1:终点坐标  
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	int dx= abs(x1-x0), sx=(x0<x1) ? 1 : -1;
	int dy=-abs(y1-y0), sy=(y0<y1) ? 1 : -1;
	int err=dx+dy, e2;
	
	for(;;)
	{
		LCD_DrawPoint(x0,y0);
		e2=2*err;
		if(e2>=dy)
		{
			if(x0==x1) break;
			err+=dy; x0+=sx;
		}
		if(e2<=dx)
		{
			if(y0==y1) break;
			err+=dx; y0+=sy;
		}
	}
}

//画矩形
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

void setPos( int sx, int ex, int sy, int ey ) {
  // X轴
  // 注意这里的“sx+2”，就是之前所说的偏移问题的解决方法
  display_cmd( 0x2a);
  display_data( 0x00 );
  display_data( sx);
  display_data( 0x00);
  display_data( ex);
  // Y轴
  display_cmd( 0x2b);
  display_data( 0x00);
  display_data( sy);
  display_data( 0x00);
  display_data( ey);
  // 很重要！最后的写入命令
  display_cmd( 0x2c);
}

void write2Byte( unsigned int twoByte ) {
   // 根据芯片手册可知先传输高8位，后传输低8位，总共16位
  display_data( twoByte>>8 );
  display_data( twoByte );
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);
		LCD_DrawPoint(x0+b,y0-a);
		LCD_DrawPoint(x0-a,y0+b);
		LCD_DrawPoint(x0-b,y0-a);
		LCD_DrawPoint(x0-a,y0-b);
		LCD_DrawPoint(x0+b,y0+a);
		LCD_DrawPoint(x0+a,y0-b);
		LCD_DrawPoint(x0+a,y0+b);
		LCD_DrawPoint(x0-b,y0+a);
		a++;
		//使用Bresenham算法画圆
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPoint(x0+a,y0+b);
	}
}
//设置地址
void lcd_address(uint8_t page,uint8_t column)
{
	column=column-0x01;
	display_cmd(0x2b+page-1);   						//设置页地址，8行为一页，全屏共64行，共计8页
	display_cmd(0x2a+(column>>4&0x0f));		//设置列地址高四位
	display_cmd(column & 0x0f);						//设置列地址低四位
}
/*-------------------------------------------------------
 函数功能: 获取汉字在GBK_Chinese中的起始位置
 函数参数: 字符指针c
 返 回 值: uint型数据
 调用示例: POS = Get_GBKChineseCode(str)
--------------------------------------------------------*/
#ifdef LARGE_CHN_LIB   //如果使用大字库则编译此段
uint32_t Get_GBKChineseCode(const unsigned char *c)
{ 
    unsigned char High8bit,Low8bit;
    unsigned int pos;
    High8bit = *c;     /* 取高8位数据 */
    Low8bit  = *(c+1);  /* 取低8位数据 */
    
    pos = ((High8bit - 0xB0)*94 + Low8bit - 0xA1)*2*16;  //计算出内码	
    return pos;  
}
#endif
/*-------------------------------------------------------
 函数功能: 获取汉字在GBK_Small中的起始位置
 函数参数: 字符指针c
 返 回 值: uint型数据
 调用示例: POS = Get_GBKSmallCode(str)
--------------------------------------------------------*/ 
#ifdef SMALL_CHN_LIB  //如果使用小字库则编译此段
uint32 Get_GBKSmallCode(const uint8_t chn[2])
{
	unsigned int i;
	for(i=0; i<WORD_NUMBER; i++)	//WORD_NUMBER为小字库的数量,定义在GBK_Small.h中
	{
		if((GBKSmall[i].InnerCode[0]==chn[0]) && (GBKSmall[i].InnerCode[1]==chn[1]))
			return i;  //返回该字的位置
	}
	return 0;
}
#endif
/*--------------------------------------------------
 功    能：在指定座标显示一个8x16点阵的ASCII字符
 入口参数：(x,y)坐标
           charColor  字符的颜色
           bColor    字符背景颜色
 说    明：需要包含ascii_font.h头文件才能使用
 调用示例：LCD_Write_Char(10,10,'a',Green,Black)
---------------------------------------------------*/
#define MAX_CHAR_POSX 128
#define MAX_CHAR_POSY 160

#if 1//def ASCII_FONT 
void LCD_Write_Char(uint16_t x,uint16_t y,uint8_t ch,uint16_t Color,uint16_t bColor)
{
   	uint16_t i=0,j=0;
  	uint8_t temp_char = 0;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;
	
  	for (i=0; i<16; i++)
  	{
	    /* ASCII_8x16字库定义在ascii_font.h中 */
	    temp_char = ASCII_8x16[ ((ch-0x20)*16)+i ];
	    for (j=0;j<8;j++)
	    {
		   if ( (temp_char >> 7-j) & 0x01 == 0x01)
		   {
		     LCD_DrawPoint_color(x+j,y+i,Color); // 字符颜色
		   }
	       else
	       {
	         LCD_DrawPoint_color(x+j,y+i,bColor); // 背景颜色
	       }
    	}
  	}
}  	

void LCD_Write_Char_ROTATE(uint16_t x,uint16_t y,uint8_t ch,uint16_t Color,uint16_t bColor)
{
   	uint16_t i=0,j=0;
  	uint8_t temp_char = 0;
    uint8_t *p;
    unsigned char ucBufferResult[32];

    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;

    p =  &ASCII_8x16[ ((ch-0x20)*16) ];

  	for (i=0; i<16; i++)
  	{
	    /* ASCII_8x16字库定义在ascii_font.h中 */
	    temp_char = *(p +i);
	    for (j=0;j<8;j++)
	    {  
		   if ( (temp_char >> 7-j) & 0x01 == 0x01)
		   {
		     LCD_DrawPoint_color(x+i,y+j,Color); // 字符颜色
		   }
	       else
	       {
	         LCD_DrawPoint_color(x+i,y+j,bColor); // 背景颜色
	       }
    	}
  	}
}  	 
/*------------------------------------------------------
 描述  ：在指定坐标处显示16*16大小的指定颜色英文字符串
 输入  : 	- x: 显示位置横向坐标	 
         	- y: 显示位置纵向坐标 
 			- str: 显示的英文字符串
 			- Color: 字符颜色   
 			- bColor: 背景颜色		
------------------------------------------------------*/
void LCD_Write_String(uint16_t x,uint16_t y,const uint8_t *p,uint16_t Color,uint16_t bColor)
{
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX)
		{
			x = 0;
			y += 12;
		}
        if(y>MAX_CHAR_POSY)
		{
			y=x=0;
		}
        LCD_Write_Char(x,y,*p,Color,bColor);
        x += 8;
        p++;
    }  
}
#endif

/*---------------------------------------------
 描述：显示单个汉字字符
 输入: (x,y)汉字显示坐标    str: 中文字符串首址
        Color: 字符颜色   	bkColor: 背景颜色     
 举例：LCD_Char_CH(200,100,"好",0,0);
 注意：如果输入大于1的汉字字符串，显示将会截断
       只显示最前面一个汉字
---------------------------------------------*/    
void LCD_Write_Char_CH(uint16_t x,uint16_t y,const uint8_t *str,uint16_t Color,uint16_t bColor)
{
    uint8_t i,j;
    uint8_t *buffer;
    uint16_t tmp_char = 0;

    buffer = str;

	for (i=0;i<16;i++)	 //把字体逐点打印出来
    {
        tmp_char = buffer[i*2];
        tmp_char = (tmp_char<<8);
        tmp_char |= buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_DrawPoint_color(x+j,y+i,Color);
            }
            else
            {
                LCD_DrawPoint_color(x+j,y+i,bColor);
            }
        }
    }
}

void LCD_Write_Char_CH_rotate(uint16_t x,uint16_t y,const uint8_t *str,uint16_t Color,uint16_t bColor)
{
    uint8_t i,j;
    uint8_t *buffer;
    uint16_t tmp_char = 0;

    buffer = str;

	for (i=0;i<16;i++)	 //把字体逐点打印出来
    {
        tmp_char = buffer[i*2];
        tmp_char = (tmp_char<<8);
        tmp_char |= buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_DrawPoint_color(x+i,y+j,Color);
            }
            else
            {
                LCD_DrawPoint_color(x+i,y+j,bColor);
            }
        }
    }
}

//显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
void display_graphic_16x16(uint8_t page,uint8_t column,uint8_t *dp)
{
    uint8_t i,j;
    for(j=0;j<2;j++)
    {
        lcd_address(page+j,column);
        for (i=0;i<16;i++)
        {
        display_data(*dp);//写数据到LCD,每写完一个8位的数据后列地址自动加1
        dp++;
        }
    }
}
#define EXAMPLE_FILE_NAME "/meig.img"
#define SMALL_IMAGE "/button.bmp"
void lcd_show_fileimg(uint16_t x,uint16_t y,char *filestr)
{
    int fd;
    uint32_t i,j;
    unsigned char *buffer;
    uint16_t x_temp = x, y_temp = y, da;
    unsigned char *tmp;
	short depth;
    int width,height;
    int stsize;

    fd = vfs_open(filestr,O_RDONLY);
    if(fd>0){
        vfs_lseek(fd,0x12,0);
        vfs_read(fd,&width,4);
        vfs_read(fd,&height,4);	
        vfs_lseek(fd,10,0);
        vfs_read(fd,&stsize,4);
        vfs_lseek(fd,0x1c,0);
        vfs_read(fd,&depth,2);
        vfs_lseek(fd, 0, 0);
        int len = vfs_read(fd,gImage_chgn,sizeof(gImage_chgn));
        if(len <0 ){
        }
        MG_osiTracePrintf(LOG_TAG, "read %d",len);
        vfs_close(fd);
    }

//    display_lcd(gImage_chgn + 0X42,sizeof(gImage_chgn) - 0x42);
    
    buffer = gImage_chgn + stsize;
	for (i= height-1;i>0;i--)
	{
		for (j=0;j<width;j++)
		{
			tmp = buffer + i*width * 2 + 2 * j;
			x_temp = x + j;
			y_temp = y + (height-i);
			Address_set(x_temp,y_temp,x_temp,y_temp);//设置光标位置 
			display_data(tmp[1]);
			display_data(tmp[0]);
		}
	}
}

void lcd_clear_region(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh, uint16_t bColor)
{
	for ( int y1 = y; y1 < (heigh+y); y1++ )
	{
	  for ( int x1 = x; x1 < (width+x); x1++ )
	  {
		  LCD_DrawPoint_color(x1,y1,bColor);
	  }
	}
}

void show_service_state(bool service)
{
	lcd_clear_region(0,40,128,16, 0xffff);

	if(service)
	{
	    LCD_Write_String(10,40,"nw connect",0x0000,0xffff);	
	}else
	{
	    LCD_Write_String(10,40,"nw disconnect",0x0000,0xffff);
	}
}

void show_tcpdata_state(bool service)
{
	lcd_clear_region(0,60,128,16, 0xffff);

	if(service)
	{
	    LCD_Write_String(10,60,"receive data",0x0000,0xffff);	
	}else
	{
	    LCD_Write_String(10,60,"no data",0x0000,0xffff);	
	}
}
void show_csq_value(void)
{
    char string[3]={0};
	char buffer[20]={0};
	uint8_t rat = 0, biterr = 0, siglevel=10, status=0;
	CFW_NW_STATUS_INFO pStatus;

	MG_osiTracePrintf(LOG_TAG, "show_csq_value");
	
	MG_CFW_GetGprsAttState(&status,0);
	MG_CFW_GprsGetstatus(&pStatus,0);

	if(MG_NwGetSignalQuality(&rat, &siglevel, &biterr))
	{
		itoa(siglevel,string,10);
	}

	snprintf(buffer,20,"%s:%s:%d:%d:%d","CSQ",string,rat,status,pStatus.nStatus);
	lcd_clear_region(0,80,128,16, 0xffff); 
	LCD_Write_String(10,80,buffer,0x0000,0xffff);

	uint8_t nbcl =0;
	MG_SysBattGetRemainCap(&nbcl);
	MG_osiTracePrintf(LOG_TAG, "nbcl =%d", nbcl);

	uint32_t batVol = 0;
	batVol = MG_drvChargerGetVbatVol();
	MG_osiTracePrintf(LOG_TAG, "batVol =%d", batVol);
	
	uint32_t chgsenVol = 0;
	chgsenVol = MG_drvAdcGetChannelVolt(ADC_CHANNEL_2,ADC_SCALE_2V444);
	MG_osiTracePrintf(LOG_TAG, "chgsenVol =%d", chgsenVol);


	memset(buffer,0,20);
	snprintf(buffer,20,"%s:%d:%d:%d","B",nbcl,batVol,chgsenVol);
	lcd_clear_region(0,100,128,16, 0xffff);
	LCD_Write_String(10,100,buffer,0x0000,0xffff);

	memset(buffer,0,20);
	snprintf(buffer,20,"%s:%d","time",g_net_total_time_ms);
	lcd_clear_region(0,120,128,16, 0xffff);
	LCD_Write_String(10,120,buffer,0x0000,0xffff);	
}

/*static void csq_cb(void *ctx)
{
	OSI_LOGI(0, "show_csq_value");
	show_csq_value();
}

#define EVENT_FOR_SHOW_CSQ       2
void lcd_csq_timer_callback(void *param)
{
	//get  signal level
	
	//MG_osiTracePrintf(LOG_TAG, "lcd_csq_timer_callback");
	//osiEvent_t event;
	//event.id = EVENT_FOR_SHOW_CSQ;
	//MG_osiEventSend((osiThread_t *)meig_baolan_lcd_Handle, &event);
	//osiEventTrySend((osiThread_t *)meig_baolan_lcd_Handle, &event,0);
	osiThreadCallback(meig_baolan_lcd_Handle,csq_cb,NULL);
	//show_csq_value();
	//MG_osiTimerStart(g_csq_timer, 2000);
}

void lcd_start_timer_show_csq(void)
{
	MG_osiTracePrintf(LOG_TAG, "lcd_start_timer_show_csq");
#if 1
	//g_csq_timer= MG_osiTimerCreate(MG_osiThreadCurrent(), lcd_csq_timer_callback, NULL);	//create timer
	g_csq_timer= MG_osiTimerCreate(NULL, lcd_csq_timer_callback, NULL);	//create timer
	if(g_csq_timer == NULL)
	{
		MG_osiTracePrintf(LOG_TAG, "create timer failed");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "create timer success");
		
		if(MG_osiTimerSetPeriod(g_csq_timer, 2000,true ))		//set periodic=true
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer failed");
		}
		MG_osiTimerStartLast(g_csq_timer, 2000);
	}
#endif
}*/

void lcd_switch_backlight(void)
{
	static bool g_bl_status = true;
    if(g_bl_status)
	{
		display_bl(0);
		g_bl_status=false;
    }else
	{
		display_bl(1);
		g_bl_status=true;
    }
}

#if 0
void readChar16(unsigned char mat[16][2],unsigned char chinease[3])
{
    int i,j;
	int fd= -1;

    fd = vfs_open("/HZK16",O_RDONLY);
    i=chinease[0]-0xa0;j=chinease[1]-0xa0;
    vfs_lseek(fd,(94*(i-1)+(j-1))*32L,0); 
	vfs_read(fd,mat,32);
    vfs_close(fd);
}
#if 1
//读英文字模函数
void readEnglish(unsigned char mat[16],unsigned char chinease[3])
{
	int fd= -1;
	 fd = vfs_open("/ASC16",O_RDONLY);
	 
	 MG_osiTracePrintf(LOG_TAG, "gaoyunlai readEnglish=%d", chinease[0]);
	 MG_osiTracePrintf(LOG_TAG, "gaoyunlai readEnglish=%d", chinease[0]*16+1);
	 
	 vfs_lseek(fd,chinease[0]*16+1,0);
	 vfs_read(fd,mat,16);
	 vfs_close(fd);
}

//字符串转字模数组
void FontStringToArray(unsigned char string[],unsigned char *array,int a2)
{
    int i=0,j=0,k=0;
    unsigned char mat2[16][2]={0},mat[16][1]={0};
    while(i<a2)
    {
       if(string[i]>0xa0&&string[i+1]>0xa0)//分支语句,判断当前指向的是中文还是英文内码
           {//如为中文读中文字模
            readChar16(mat2,&string[i]);
            for(j=0;j<16;j++){
              for(k=0;k<2;k++){
                 if(k+i<a2)  *(array+j*a2+k+i)=*(*(mat2+j)+k);
               }
             }
             i+=2;
            }
        else
           {//如为英文则读英文字模
            readEnglish(mat,&string[i]);
            for(j=0;j<16;j++){
                 *(array+j*a2+i)=*(*(mat+j)+0);
             }
             i++;
           }
    }
}
void printText(unsigned char* string,int x,int y)
{
    int i,j,k;
    unsigned char mat[16][2]={0},mat1[16]={0},t,m,n;
	if(string[0]>0xa0&&string[1]>0xa0)
	{
	    readChar16(mat,string);
	    for(j=0;j<16;j++) 
		{
	        for(i=0;i<2;i++) {
	            t=mat[j][i];
	            m=0x80;
	            for(k=0;k<8;k++) {
	                n=t&m;
	                n>>=(7-k);
	                if(n==1) LCD_DrawPoint_color(x+(i*8+k),y+j,0X0000);
	                m>>=1;
	             }
	         }
	     }
	}
	else
	{
		readEnglish(mat1,string);
		MG_osiTracePrintf(LOG_TAG, "gaoyunlai");
		
		for (j = 0; j < 16; j++)		  /* 8x16的点阵，一共有16行*/
		{
			t=mat[j];
			m=0x80;	
			MG_osiTracePrintf(LOG_TAG, "gaoyunlai mat[%d]=%d",j,t);
			for (k = 0; k < 8; k++) 		/*横向一个字节8位，依次判断每位是否为0*/
			{
				if(t & m)
					LCD_DrawPoint_color(x+k,y+j,0X0000);
				m >>= 1;
			}
		}
	}
}
#endif

#else

#endif


void OEM_LcdThread(void *param)
	{
		display_init();
	
		display_bl(0);
		display_rst(1);
		MG_osiThreadSleep(100);
		display_rst(0);
		MG_osiThreadSleep(100);
		display_rst(1); 
		MG_osiThreadSleep(100);
		display_cmd(0x11); //Sleep out
		MG_osiThreadSleep(100);
		//------------------------------------ST7735S Frame Rate-----------------------------------------//
		display_cmd(0xB1);
		display_data(0x05);
		display_data(0x3C);
		display_data(0x3C);
		display_cmd(0xB2);
		display_data(0x05);
		display_data(0x3C);
		display_data(0x3C);
		display_cmd(0xB3);
		display_data(0x05);
		display_data(0x3C);
		display_data(0x3C);
		display_data(0x05);
		display_data(0x3C);
		display_data(0x3C);
		//------------------------------------End ST7735S Frame Rate-----------------------------------------//
		display_cmd(0xB4); //Dot inversion
		display_data(0x03);
		display_cmd(0xC0);
		display_data(0x28);
		display_data(0x08);
		display_data(0x04);
		display_cmd(0xC1);
		display_data(0XC0);
		display_cmd(0xC2);
		display_data(0x0D);
		display_data(0x00);
		display_cmd(0xC3);
		display_data(0x8D);
		display_data(0x2A);
		display_cmd(0xC4);
		display_data(0x8D);
		display_data(0xEE);
		//---------------------------------End ST7735S Power Sequence-------------------------------------//
		display_cmd(0xC5); //VCOM
		display_data(0x1A);
		display_cmd(0x36); //MX, MY, RGB mode
		display_data(0xC0);
		//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
		display_cmd(0xE0);
		display_data(0x04);
		display_data(0x22);
		display_data(0x07);
		display_data(0x0A);
		display_data(0x2E);
		display_data(0x30);
		display_data(0x25);
		display_data(0x2A);
		display_data(0x28);
		display_data(0x26);
		display_data(0x2E);
		display_data(0x3A);
		display_data(0x00);
		display_data(0x01);
		display_data(0x03);
		display_data(0x13);
		display_cmd(0xE1);
		display_data(0x04);
		display_data(0x16);
		display_data(0x06);
		display_data(0x0D);
		display_data(0x2D);
		display_data(0x26);
		display_data(0x23);
		display_data(0x27);
		display_data(0x27);
		display_data(0x25);
		display_data(0x2D);
		display_data(0x3B);
		display_data(0x00);
		display_data(0x01);
		display_data(0x04);
		display_data(0x13);
		//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
		display_cmd(0x3A); //65k mode
		display_data(0x05);
		display_cmd(0x29); //Display on
		MG_osiThreadSleep(100);
		display_cmd(0x2c);
	
		for ( int y = 0; y < 160; y++ ) {
		  for ( int x = 0; x < 128; x++ ) {
			write2Byte( 0x0000 );
		  }
		}
		MG_osiThreadSleep(200);
		display_bl(1);
	
		setPos( 0, 128, 0, 160 );
		for ( int y = 0; y < 160; y++ ) {
		  for ( int x = 0; x < 128; x++ ) {
			write2Byte( 0x0000 );
		  }
		}
		MG_osiThreadSleep(500);
		setPos( 0, 128, 0, 160 );
		for ( int y = 0; y < 160; y++ ) {
		  for ( int x = 0; x < 128; x++ ) {
			write2Byte( 0x001F );
		  }
		}
		MG_osiThreadSleep(500);
		setPos( 0, 128, 0, 160 );
		for ( int y = 0; y < 160; y++ ) {
		  for ( int x = 0; x < 128; x++ ) {
			write2Byte( 0xF800 );
		  }
		}
		MG_osiThreadSleep(500);
		LCD_Draw_Circle(64, 80, 40);
		LCD_Draw_Circle(64, 80, 60);
		LCD_Draw_Circle(64, 80, 20);
		MG_osiThreadSleep(500);

	    //lcd_show_fileimg(0,0,128,160,EXAMPLE_FILE_NAME);

		lcd_clear_region(0,0,128,160,0xffff);
	    
	    //lcd_show_fileimg(20,20,SMALL_IMAGE);

	    //display_lcd(gImage_chgn,sizeof(gImage_chgn));
		//MG_osiThreadSleep(1000);
	    //LCD_Write_Char_CH(0,0,hanzi_mei,0xffff,0x0000);
	    //LCD_Write_String(16,0,str,0xffff,0x0000);
		//lcd_start_timer_show_csq();//zx modify 20200916
		//printText("我",20,135);
		//MG_osiThreadSleep(500);		
		//printText("A",20,140);
		for (;;)
		{
#if 1
			osiEvent_t event;
			event.id = OSI_EVENT_ID_NONE;
			osiEventTryWait(MG_osiThreadCurrent(), &event,300*1000);
			OSI_LOGI(0, "receive event %d", event.id);
			/*if (event.id == EVENT_FOR_SHOW_CSQ)//delete by zx 20200916
			{
				OSI_LOGI(0, "show_csq_value");
				//show_csq_value();
				
			}
			else*/
			if(event.id == OSI_EVENT_ID_QUIT)
			{
				break;
			}
#else
			MG_osiTracePrintf(LOG_TAG, "BaolanLcdThread");
			MG_osiThreadSleep(5000);
			MG_osiTracePrintf(LOG_TAG, "BaolanLcdThread end");			
#endif
		}
		
		//MG_osiTimerStop(g_csq_timer);//delete by zx 20200916
		//MG_osiTimerDelete(g_csq_timer);//delete by zx 20200916
		MG_osiThreadExit();
	}


