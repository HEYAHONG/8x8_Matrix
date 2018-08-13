//点阵显示 亲 - 桃心（形状） 
#include <stc\stc15.h>
#include <intrins.h>

//点阵模块接口定义
#define LEDARRAY_LAT  P33  //储存寄存器时钟
#define LEDARRAY_CLK  P35 //移位寄存器时钟输入端 
#define LEDARRAY_DI   P34 //串行数据输入端

//二维数组 内含字摸代码，取摸方式和软件都在本目录内
unsigned char  code Display_word[2][8] = {	 //测试用数组
0x00,0x48,0x2A,0x8E,0xFB,0x0E,0x2A,0x48,//亲
0x0C,0x1E,0x3E,0x7C,0x7C,0x3E,0x1E,0x0C //桃心（形状） 
};

unsigned char code  Default_Word[2][8] = {	   
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//全灭
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF //全亮
} ;
unsigned char VBuff[8]; //LED显示缓冲区

//发送一个直接数据给点阵模块
void Send_Byte( unsigned char dat)
{
	unsigned char i;//循环次数变量
	LEDARRAY_CLK = 0;//拉低移位时钟
	_nop_();	
	LEDARRAY_LAT = 0;//拉低储存时钟
	_nop_();

	for( i = 0 ; i < 8 ; i++ ) //循环8次写入一个字节数据
	{
		if( dat&0x01 )
		{
			LEDARRAY_DI = 1;	
		}
		else
		{
			LEDARRAY_DI = 0;
		}
		
		LEDARRAY_CLK = 1;				//上升沿发送数据
						_nop_();
		LEDARRAY_CLK = 0;
						_nop_();
				
		dat >>= 1;			
	}		
}

void CopyCodeToBuff(unsigned char code  *src) //从程序区复制指定数据到显示缓冲区
{
unsigned char k=0;
for(k = 0 ; k < 8 ; k++)		
{
VBuff[k]=*src++;
}
}

void CopyToBuff(unsigned char code  *src) //从数据区区复制指定数据到显示缓冲区
{
unsigned char k=0;
for(k = 0 ; k < 8 ; k++)		
{
VBuff[k]=*src++;
}
}

/*以下是从stc15示例代码复制并加以修改的部分，需用到STC15的一些特性，可能不能在某些兼容51的单片机上使用*/
#define FOSC 11059200L	 //系统时钟11.0592M

//#define T1MS (65536-FOSC/1000)      //1T模式
#define T50MS (65536-FOSC/12/20) //12T模式
unsigned  long SysClk=0;  //系统时钟记录变量
bit TestFlag=1; //定义一系列标志，TestFlag为是否显示内部测试图像（心形和亲字）


void tm0_isr() interrupt 1 	//定时器0中断处理函数
{
    SysClk++;
    if(TestFlag){ //显示测试点阵图像
		if(SysClk%40==0) 
		{
		CopyCodeToBuff(Display_word[0]);
		}
		if(SysClk%40==20)
		 {
		CopyCodeToBuff(Display_word[1]);
		}
		}
	if(SysClk== 4000000000)	 SysClk=0; //系统时钟上界
}
void init_tm0()	//定时器0初始化
{
  // AUXR |= 0x80;                   //定时器0为1T模式
   AUXR &= 0x7f;                   //定时器0为12T模式

    TMOD = 0x00;                    //设置定时器为模式0(16位自动重装载)
    TL0 = T50MS;                     //初始化计时值
    TH0 = T50MS >> 8;
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}

sbit INT0 = 0xB2; //如果出现编译错误，可注释掉此行，此行主要补充stc15里缺失的定义。
 void exint0() interrupt 0       //INT0中断入口
{
    if(TestFlag=~TestFlag) CopyCodeToBuff(Default_Word[1]);		//测试外部中断，点亮屏幕
	else                   CopyCodeToBuff(Default_Word[0]);	    //测试外部中断，熄灭屏幕
}

void init_exint0()
{
	INT0 = 1;
    IT0 = 1;                    //设置INT0的中断类型 (1:仅下降沿 0:上升沿和下降沿)
    EX0 = 1;                    //使能INT0中断
    EA = 1;
}
/*stc15修改部分结束*/

void main()
{

	unsigned char k, temp;
	init_tm0(); //初始化定时器0
	init_exint0(); //初始化外部中断0
	while(1)   //将缓冲区的内容放到LED点阵上。
	{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//一个字母8BYTE
			{
				Send_Byte(temp);			//列选择
				Send_Byte(VBuff[k]);			//行数据
				LEDARRAY_LAT = 1;					//锁存数据
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		
			
	}
}
