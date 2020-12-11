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

#include "mg_baolan_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "drv_lcd.h"
#include "stdio.h"

#include "vfs.h"


uint16_t BACK_COLOR=0xffff;    //����ɫ
uint16_t POINT_COLOR=0x0000;   //����ɫ


const unsigned char hanzi_mei[]={/*"��",0*/
0x08,0x20,0x06,0x60,0x02,0x88,0x7F,0x0FC,0x01,0x00,0x3F,0xF8,0x01,0x00,0xFF,0xFE,
0x01,0x00,0x01,0x08,0x7F,0xFC,0x01,0x00,0x02,0x80,0x04,0x60,0x18,0x1E,0x0E0,0x04,
};
const unsigned char hanzi_ge[]={/*��",0*/
0x11,0x00,0x11,0x00,0x11,0xF8,0x12,0x08,0xFF,0x10,0x14,0xA0,0x38,0x40,0x34,0xA0,
0x51,0x10,0x52,0x0E,0x97,0xFC,0x1A,0x08,0x12,0x08,0x12,0x08,0x13,0xF8,0x12,0x08
};
const unsigned char hanzi_zhi[]={/*��",0*/
0x20,0x00,0x22,0x04,0x3F,0x7E,0x48,
0x44,0x08,0x44,0xFF,0xC4,0x10,0x44,
0x14,0x7C,0x22,0x44,0x42,0x10,0x9F,
0xF8,0x10,0x10,0x1F,0xF0,0x10,0x10,
0x1F,0xF0,0x10,0x10
};

const unsigned char hanzi_neng[]={/*��",0*/
0x10,0x80,0x10,0x88,0x24,0x98,0x44,
0xA0,0xFE,0xC0,0x02,0x84,0x7C,0x84,
0x44,0x7C,0x7C,0x00,0x44,0x88,0x44,
0x98,0x7C,0xA0,0x44,0xC0,0x44,0x82,
0x54,0x82,0x48,0x7E
};

static void LCD_Address_set(uint8_t xsta,uint8_t ysta,uint8_t xend,uint8_t yend)
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
//����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_Address_set(x,y,x,y);//���ù��λ�� 
	LCD_WR16(POINT_COLOR);
}

void LCD_DrawPoint_color(uint16_t x,uint16_t y,uint16_t da)
{
	LCD_Address_set(x,y,x,y);//���ù��λ�� 
	LCD_WR16(da);
}

//����
//x0,y0:�������
//x1,y1:�յ�����  
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

//������
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

void LCD_setPos( int sx, int ex, int sy, int ey ) {
  // X��
  // ע������ġ�sx+2��������֮ǰ��˵��ƫ������Ľ������
  display_cmd( 0x2a);
  display_data( 0x00 );
  display_data( sx);
  display_data( 0x00);
  display_data( ex);
  // Y��
  display_cmd( 0x2b);
  display_data( 0x00);
  display_data( sy);
  display_data( 0x00);
  display_data( ey);
  // ����Ҫ������д������
  display_cmd( 0x2c);
}

void LCD_write2Byte( unsigned int twoByte ) {
   // ����оƬ�ֲ��֪�ȴ����8λ�������8λ���ܹ�16λ
  display_data( twoByte>>8 );
  display_data( twoByte );
}

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPoint(x0+a,y0+b);
	}
}
/*-------------------------------------------------------
 ��������: ��ȡ������GBK_Chinese�е���ʼλ��
 ��������: �ַ�ָ��c
 �� �� ֵ: uint������
 ����ʾ��: POS = Get_GBKChineseCode(str)
--------------------------------------------------------*/
#ifdef LARGE_CHN_LIB   //���ʹ�ô��ֿ������˶�
uint32_t Get_GBKChineseCode(const unsigned char *c)
{ 
    unsigned char High8bit,Low8bit;
    unsigned int pos;
    High8bit = *c;     /* ȡ��8λ���� */
    Low8bit  = *(c+1);  /* ȡ��8λ���� */
    
    pos = ((High8bit - 0xB0)*94 + Low8bit - 0xA1)*2*16;  //���������	
    return pos;  
}
#endif
/*-------------------------------------------------------
 ��������: ��ȡ������GBK_Small�е���ʼλ��
 ��������: �ַ�ָ��c
 �� �� ֵ: uint������
 ����ʾ��: POS = Get_GBKSmallCode(str)
--------------------------------------------------------*/ 
#ifdef SMALL_CHN_LIB  //���ʹ��С�ֿ������˶�
uint32 Get_GBKSmallCode(const uint8_t chn[2])
{
	unsigned int i;
	for(i=0; i<WORD_NUMBER; i++)	//WORD_NUMBERΪС�ֿ������,������GBK_Small.h��
	{
		if((GBKSmall[i].InnerCode[0]==chn[0]) && (GBKSmall[i].InnerCode[1]==chn[1]))
			return i;  //���ظ��ֵ�λ��
	}
	return 0;
}
#endif
/*--------------------------------------------------
 ��    �ܣ���ָ��������ʾһ��8x16�����ASCII�ַ�
 ��ڲ�����(x,y)����
           charColor  �ַ�����ɫ
           bColor    �ַ�������ɫ
 ˵    ������Ҫ����ascii_font.hͷ�ļ�����ʹ��
 ����ʾ����LCD_Write_Char(10,10,'a',Green,Black)
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
	    /* ASCII_8x16�ֿⶨ����ascii_font.h�� */
	    temp_char = ASCII_8x16[ ((ch-0x20)*16)+i ];
	    for (j=0;j<8;j++)
	    {  
		   if ( (temp_char >> 7-j) & 0x01 == 0x01)
		   {
		     LCD_DrawPoint_color(x+j,y+i,Color); // �ַ���ɫ
		   }
	       else
	       {
	         LCD_DrawPoint_color(x+j,y+i,bColor); // ������ɫ
	       }
    	}
  	}
}  	

void LCD_Write_Char_ROTATE(uint16_t x,uint16_t y,uint8_t ch,uint16_t Color,uint16_t bColor)
{
   	uint16_t i=0,j=0;
  	uint8_t temp_char = 0;
    uint8_t *p;
    unsigned char ucBufferResult[32]; //������ʱ���ת�����������ģ����

    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;

    p =  &ASCII_8x16[ ((ch-0x20)*16) ];

  	for (i=0; i<16; i++)
  	{
	    /* ASCII_8x16�ֿⶨ����ascii_font.h�� */
	    temp_char = *(p +i);
	    for (j=0;j<8;j++)
	    {  
		   if ( (temp_char >> 7-j) & 0x01 == 0x01)
		   {
		     LCD_DrawPoint_color(x+i,y+j,Color); // �ַ���ɫ
		   }
	       else
	       {
	         LCD_DrawPoint_color(x+i,y+j,bColor); // ������ɫ
	       }
    	}
  	}
}  	 
/*------------------------------------------------------
 ����  ����ָ�����괦��ʾ16*16��С��ָ����ɫ�ַ���
 ����  : 	- x: ��ʾλ�ú�������	 
         	- y: ��ʾλ���������� 
 			- str: ��ʾ���ַ���
 			- Color: �ַ���ɫ   
 			- bColor: ������ɫ		
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
 ��������ʾ���������ַ�
 ����: (x,y)������ʾ����    str: �����ַ�����ַ
        Color: �ַ���ɫ   	bkColor: ������ɫ     
 ������LCD_Char_CH(200,100,"��",0,0);
 ע�⣺����������1�ĺ����ַ�������ʾ����ض�
       ֻ��ʾ��ǰ��һ������
---------------------------------------------*/    
void LCD_Write_Char_CH(uint16_t x,uint16_t y,const uint8_t *str,uint16_t Color,uint16_t bColor)
{
    uint8_t i,j;
    uint8_t *buffer;
    uint16_t tmp_char = 0;

    buffer = str;

	for (i=0;i<16;i++)	 //����������ӡ����
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

	for (i=0;i<16;i++)	 //����������ӡ����
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

#define EXAMPLE_FILE_NAME "/meig.img"
#define SMALL_IMAGE "/100.bmp"
#define IMAGE_557 "/557.bmp"

/*---------------------------------------------
 ��������ʾ�ļ���ͼƬ
 ����: (x,y)������ʾ����    filestr: ͼƬ�ļ�·��
        
 ������LCD_Char_CH(200,100,"��",0,0);
 ע�⣺����������1�ĺ����ַ�������ʾ����ض�
       ֻ��ʾ��ǰ��һ������
---------------------------------------------*/  
void LCD_show_fileimg(uint16_t x,uint16_t y,char *filestr)
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
        vfs_lseek(fd,0x12,SEEK_SET);
        vfs_read(fd,&width,4);
        vfs_read(fd,&height,4);	
        vfs_lseek(fd,10,SEEK_SET);
        vfs_read(fd,&stsize,4);
        vfs_lseek(fd,0x1c,SEEK_SET);
        vfs_read(fd,&depth,2);
        vfs_lseek(fd, 0, SEEK_SET);
        int len = vfs_read(fd,gImage_chgn,sizeof(gImage_chgn));
        if(len <0 ){
        }
        MG_osiTracePrintf(LOG_TAG, "read %d",len);
        vfs_close(fd);
    }

//    display_lcd(gImage_chgn + 0X42,sizeof(gImage_chgn) - 0x42);
    
    buffer = gImage_chgn + stsize;
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
        {
            tmp = buffer + i*width * 2 + 2 * j;
            x_temp = x + j;
            y_temp = y + i;
            LCD_Address_set(x_temp,y_temp,x_temp,y_temp);//���ù��λ�� 
            display_data(tmp[1]);
            display_data(tmp[0]);
        }
    }
}

static void prvLcdThread(void *param)
{
    uint8_t str[] ={0x30,0x42,0x43,0x44,0x45,0x46,0};
    
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
        LCD_write2Byte( 0x0000 );
      }
    }
	MG_osiThreadSleep(200);
    display_bl(1);

    LCD_setPos( 0, 128, 0, 160 );
    for ( int y = 0; y < 160; y++ ) {
      for ( int x = 0; x < 128; x++ ) {
        LCD_write2Byte( 0x0000 );
      }
    }
	MG_osiThreadSleep(500);
    LCD_setPos( 0, 128, 0, 160 );
    for ( int y = 0; y < 160; y++ ) {
      for ( int x = 0; x < 128; x++ ) {
        LCD_write2Byte( 0x001F );
      }
    }
	MG_osiThreadSleep(500);
    LCD_setPos( 0, 128, 0, 160 );
    for ( int y = 0; y < 160; y++ ) {
      for ( int x = 0; x < 128; x++ ) {
        LCD_write2Byte( 0xF800 );
      }
    }
	MG_osiThreadSleep(500);
    LCD_Draw_Circle(64, 80, 40);
    LCD_Draw_Circle(64, 80, 60);
    LCD_Draw_Circle(64, 80, 20);
    MG_osiThreadSleep(500);

    LCD_show_fileimg(0,0,EXAMPLE_FILE_NAME);
    LCD_show_fileimg(20,20,SMALL_IMAGE);
    LCD_show_fileimg(20,20,IMAGE_557);

    //display_lcd(gImage_chgn,sizeof(gImage_chgn));
	MG_osiThreadSleep(1000);
    LCD_Write_Char_CH(0,0,hanzi_mei,0xffff,0x0000);
    LCD_Write_String(16,0,str,0xffff,0x0000);


    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("meg-lcd", prvLcdThread, NULL, OSI_PRIORITY_LOW, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
