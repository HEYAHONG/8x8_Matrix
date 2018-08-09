//点阵显示 亲 - 桃心（形状） 
#include <stc\stc15.h>
#include <intrins.h>

//点阵模块接口定义
#define LEDARRAY_LAT  P33  //储存寄存器时钟
#define LEDARRAY_CLK  P35 //移位寄存器时钟输入端 
#define LEDARRAY_DI   P34 //串行数据输入端

//二维数组 内含字摸代码，取摸方式和软件都在本目录内
unsigned char  code Display_word[2][8] = {
0x00,0x48,0x2A,0x8E,0xFB,0x0E,0x2A,0x48,//亲
0x0C,0x1E,0x3E,0x7C,0x7C,0x3E,0x1E,0x0C //桃心（形状） 
};

//SPI通信函数 发送一个直接数据给点阵模块
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

void main()
{

	unsigned char k, temp;
	unsigned int i;//i的数值越大显示停留的时间就越久

	while(1)
	{
			//亲
		for(i = 0 ; i < 1000 ; i++)
		{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//一个字母8BYTE
			{
				Send_Byte(temp);			//列选择
				Send_Byte(Display_word[0][k]);			//行数据
				LEDARRAY_LAT = 1;					//锁存数据
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		}
			//桃心（形状） 
		for(i = 0 ; i < 1000 ; i++)
		{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//一个字母4BYTE
			{
				Send_Byte(temp);			//列选择
				Send_Byte(Display_word[1][k]);			//行数据
				LEDARRAY_LAT = 1;					//锁存数据
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		}
		P10=~P10; //LED	
	}
}
