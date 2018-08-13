//������ʾ �� - ���ģ���״�� 
#include <stc\stc15.h>
#include <intrins.h>

//����ģ��ӿڶ���
#define LEDARRAY_LAT  P33  //����Ĵ���ʱ��
#define LEDARRAY_CLK  P35 //��λ�Ĵ���ʱ������� 
#define LEDARRAY_DI   P34 //�������������

//��ά���� �ں��������룬ȡ����ʽ��������ڱ�Ŀ¼��
unsigned char  code Display_word[2][8] = {	 //����������
0x00,0x48,0x2A,0x8E,0xFB,0x0E,0x2A,0x48,//��
0x0C,0x1E,0x3E,0x7C,0x7C,0x3E,0x1E,0x0C //���ģ���״�� 
};

unsigned char code  Default_Word[2][8] = {	   
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//ȫ��
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF //ȫ��
} ;
unsigned char VBuff[8]; //LED��ʾ������

//����һ��ֱ�����ݸ�����ģ��
void Send_Byte( unsigned char dat)
{
	unsigned char i;//ѭ����������
	LEDARRAY_CLK = 0;//������λʱ��
	_nop_();	
	LEDARRAY_LAT = 0;//���ʹ���ʱ��
	_nop_();

	for( i = 0 ; i < 8 ; i++ ) //ѭ��8��д��һ���ֽ�����
	{
		if( dat&0x01 )
		{
			LEDARRAY_DI = 1;	
		}
		else
		{
			LEDARRAY_DI = 0;
		}
		
		LEDARRAY_CLK = 1;				//�����ط�������
						_nop_();
		LEDARRAY_CLK = 0;
						_nop_();
				
		dat >>= 1;			
	}		
}

void CopyCodeToBuff(unsigned char code  *src) //�ӳ���������ָ�����ݵ���ʾ������
{
unsigned char k=0;
for(k = 0 ; k < 8 ; k++)		
{
VBuff[k]=*src++;
}
}

void CopyToBuff(unsigned char code  *src) //��������������ָ�����ݵ���ʾ������
{
unsigned char k=0;
for(k = 0 ; k < 8 ; k++)		
{
VBuff[k]=*src++;
}
}

/*�����Ǵ�stc15ʾ�����븴�Ʋ������޸ĵĲ��֣����õ�STC15��һЩ���ԣ����ܲ�����ĳЩ����51�ĵ�Ƭ����ʹ��*/
#define FOSC 11059200L	 //ϵͳʱ��11.0592M

//#define T1MS (65536-FOSC/1000)      //1Tģʽ
#define T50MS (65536-FOSC/12/20) //12Tģʽ
unsigned  long SysClk=0;  //ϵͳʱ�Ӽ�¼����
bit TestFlag=1; //����һϵ�б�־��TestFlagΪ�Ƿ���ʾ�ڲ�����ͼ�����κ����֣�


void tm0_isr() interrupt 1 	//��ʱ��0�жϴ�����
{
    SysClk++;
    if(TestFlag){ //��ʾ���Ե���ͼ��
		if(SysClk%40==0) 
		{
		CopyCodeToBuff(Display_word[0]);
		}
		if(SysClk%40==20)
		 {
		CopyCodeToBuff(Display_word[1]);
		}
		}
	if(SysClk== 4000000000)	 SysClk=0; //ϵͳʱ���Ͻ�
}
void init_tm0()	//��ʱ��0��ʼ��
{
  // AUXR |= 0x80;                   //��ʱ��0Ϊ1Tģʽ
   AUXR &= 0x7f;                   //��ʱ��0Ϊ12Tģʽ

    TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL0 = T50MS;                     //��ʼ����ʱֵ
    TH0 = T50MS >> 8;
    TR0 = 1;                        //��ʱ��0��ʼ��ʱ
    ET0 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}

sbit INT0 = 0xB2; //������ֱ�����󣬿�ע�͵����У�������Ҫ����stc15��ȱʧ�Ķ��塣
 void exint0() interrupt 0       //INT0�ж����
{
    if(TestFlag=~TestFlag) CopyCodeToBuff(Default_Word[1]);		//�����ⲿ�жϣ�������Ļ
	else                   CopyCodeToBuff(Default_Word[0]);	    //�����ⲿ�жϣ�Ϩ����Ļ
}

void init_exint0()
{
	INT0 = 1;
    IT0 = 1;                    //����INT0���ж����� (1:���½��� 0:�����غ��½���)
    EX0 = 1;                    //ʹ��INT0�ж�
    EA = 1;
}
/*stc15�޸Ĳ��ֽ���*/

void main()
{

	unsigned char k, temp;
	init_tm0(); //��ʼ����ʱ��0
	init_exint0(); //��ʼ���ⲿ�ж�0
	while(1)   //�������������ݷŵ�LED�����ϡ�
	{
			temp = 0x7f;
			for(k = 0 ; k < 8 ; k++)		//һ����ĸ8BYTE
			{
				Send_Byte(temp);			//��ѡ��
				Send_Byte(VBuff[k]);			//������
				LEDARRAY_LAT = 1;					//��������
				_nop_();
			
				LEDARRAY_LAT = 0;
				_nop_();
				temp = (temp>>1)|0x80;
			}
		
			
	}
}
