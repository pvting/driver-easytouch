#ifndef _DATA_H_
#define _DATA_H_

#define RECEIVE_BUF  1024
unsigned char FrameReceive[RECEIVE_BUF];					//接受传输的数据
#define READ           0
#define WRITE          1

/*******************************************VMC事件*********************************************/
#define EVENT_SIGN_IN 			0xA1						//签到事件
#define EVENT_GET_POLL	 		0xA2						//POLL轮询事件
#define	EVENT_OUTGOODS_RESULT	0xA3						//出货报告事件
#define EVENT_CHANNEL_MESSAGE	0xA4						//料道运行事件（表示饮料柜11）
#define EVENT_CHANNEL_MESSAGE2	0xC4						//料道运行事件2（表示食品柜09）
#define EVENT_MASHION_MESSAGE	0xA5						//机器运行事件
#define EVENT_SYSTEM_SETTING	    0xA6						//系统配置事件（表示饮料柜11）
#define EVENT_SYSTEM_SETTING2	0xC6						//系统配置事件2（表示饮料柜09）
#define EVENT_GOODS_MESSAGE		0xA7						//料道商品信息事件（表示饮料柜11）
#define EVENT_GOODS_MESSAGE2    	0xC7						//料道商品信息事件2（表示食品柜09）
#define EVENT_GOODS_PRICE 		0xA8						//料道价格信息事件（表示饮料柜11）
#define EVENT_GOODS_PRICE2 		0xC8						//料道价格信息事件2（表示食品柜09）


#define EVENT_DOOR_STATE_OPEN 	0xB1 						//开门事件
#define EVENT_DOOR_STATE_CLOSE  0xB2 						//关门事件
//#define EVENT_SET_ROAD_SUCCESS    0XB3; 						//设置价格成功
//#define EVENT_SET_ROAD_FAILED     0XB4; 						//设置价格失败
#define EVENT_CASH_CHANGED 		0xB5                        //收到现金变化事件
#define EVENT_CHANNEL_CHANGED   0xB6						    //收到选取料道事件
#define EVENT_SET_FAILD     	    0xB7						//设定失败事件
#define EVENT_SET_SUCCESS     	0xB8						//设定成功事件

#define EVENT_SERIALPORT_CONNECT 	0xC1				    //串口连接状态事件
#define EVENT_SERIALPORT_DISCONNECT 	0xC2				//串口连接状态事件
/*******************************************底层驱动定义**************************************/
#define TAGGED_WORD_ONE		0xEF						//传输的标志字
#define TAGGED_WORD_TWO		0xEE
#define TAGGED_WORD_THREE		0xFE
#define	ACK 					0x00						//ACK
#define NAK  					0x15						//NCK

#define Unknow_mes            0x71                         //未知指令
#define POLL					0x76						//POLL
#define SIGN_IN				0x78						//签到
//#define TAKE_GOOD				0x74						//提货码
#define	CHANNEL_RUN			0x79						//料道运行信息
#define SYSTEM_CONFIG			0x7A						//系统配置信息
#define PRODUCT_SALES			0x7B						//商品销售信息
#define SHIPMENT				0x7C						//出货
#define MACHINE_RUN			0x7D						//机器运行信息
#define CHANNEL_GOOD			0x7E						//料道商品信息
#define CHANNEL_PRICE			0x7F						//料道价格信息
#define CHANNEL_SALE			0x75						//传递料道销售统计
/*********************************************************************************************/

#define SYS_TIME_SET			0x02						//设置系统时间
#define ASK_SHIPMENT			0x03						//出货请求
#define SET_CHANNEL_PRICE		0x04						//设置货道信息
#define CHANNEL_GOOD_ADD		0x05						//货道加货
#define SET_SYS_SETTING		0x06						//设置系统状态信息
#define MACHINE_ONOFF			0x07						//运营的暂停与恢复
#define SALE_GOOD_SUMMARY		0x08						//销售数据传输
/*****************************************底层驱动定义****************************************/

struct 				
{
	unsigned char Master_version[2];  			//主控板版本号
	unsigned char Machine_num[4];				//售货机编号
	unsigned char Time[7];						//时间YYYYMMDDHHMMSS
	unsigned char Machine_class;				//机器类型，01饮料机，11食品机，20饮料1拖N
	unsigned char Drink_num;					//饮料柜数量一般为1
	unsigned char Food_num;						//食品柜数量0-2
	unsigned char Noodle_num;					//泡面柜数量0-5
	unsigned char Drink_channel_num;			//饮料柜货道数设置
	unsigned char Food_channel_num;				//食品柜货道数设置，最大48
	unsigned char Noodle_channel_num;			//泡面柜货道数设置，最大40
}Sign_in_instruct;								


/*******************************************POLL信息*****************************************/
struct 				
{
	unsigned char Total_money[6];  		//接受的金额
//	unsigned char Paper_money[4];		
//	unsigned char Coins_money[4];	
	unsigned char Get_container_num[5];	//选择货道的按钮
//	unsigned char Get_aisle_num;		
}Poll_instruct;
	

/*******************************************出货报告事件***************************************/
struct 				
{
	unsigned char  Container_num;  		//货柜号
	unsigned char  Channel_num;			//货道编号
	unsigned char  Product_num;			//商品序号
	unsigned char  Product_price[3];	//售卖金额
	unsigned char  Serial_number;		//序列号
	unsigned char  Payment_way;			//支付方式
	unsigned char  Product_out_station; //出货情况
	unsigned char  Outfailed_reason;	//出货失败类型
	unsigned char  Card_num[9];			//卡号
	unsigned char  Serial_number2[3];	//流水号
	unsigned char  Product_sale_num[3];	//商品销售总数量
	unsigned char  Product_sale_money[4];	//商品销售总金额
}Shipment_instruct;


/*******************************************料道运行信息****************************************/
struct 				
{
	unsigned char  Container_num;  		//货柜号
	unsigned char  Channel_fault[6];	//料道是否故障
	unsigned char  Channel_sale[48];	//货道销售情况
}Channle_run_instruct[2];


/*******************************************机器运行信息***********************************************/
struct 				
{
	unsigned char   Machine_run;		//机器运行状态
	unsigned char   VMC_station;		//售货机状态
	unsigned char	Note_machine_state;	//纸币器状态
	unsigned char   Coin_machine_state;	//硬币器状态
	unsigned char	Coin_num[4];		//检测硬币器数量
	unsigned char	Note_num[2];		//统计纸币数量
}Mashion_run_instruct;

/******************************************系统配置信息************************************************/
struct 				
{
	unsigned char  Container_num;		//货柜号
	unsigned char  Sale_set;			//售卖配置
	unsigned char  Led_state;			//照明状态
	unsigned char  Led_time[4];			//日光灯时间
	unsigned char  Left_control_state;	//左室状态
	unsigned char  Right_control_state;	//右室状态
	unsigned char  Save_time[4];		//节能时间
	unsigned char  Coldtem_set;			//制冷温度
	unsigned char  Coldtem_real;		//实际温度
	unsigned char  Hottem_set;			//加热温度
	unsigned char  Hottem_real;			//实际温度
}Sys_set_instruct[2];


/*******************************************料道商品信息***********************************************/
struct 				
{
	unsigned char  Container_num;		//货柜号
	unsigned char  Channel_poduct[48];	//货道商品信息
}Channel_product_instruct[2];

/*******************************************料道价格信息***********************************************/
struct 				
{
	unsigned char  Container_num;		//货柜号
	unsigned char  Channel_price[96];	//货道商品信息
}Channel_price_instruct[2];

/**************************************************************************************/

unsigned char Systime_set_instruct[8];		 //设置系统时间
unsigned char Shipment_request_instruct[21]; //扣款出货请求
unsigned char Set_channelprice_instruct[8];	 //设置料道信息
unsigned char Sys_channeladd_instruct[6];	 //设置料道加货信息
unsigned char Set_system_instruct[16];		 //设置系统状态信息
unsigned char Machine_onoff_instruct[3];	 //设置运营的暂停与恢复

#define SERIAL_FAIL -1						//串口错误
#define PTHREAD_FAIL -2						//线程错误
#define TEST_FAIL -3						//测试错误

#define ON_KEY   0x00						//开启售卖
#define OFF_KEY	 0x01						//暂停售卖

#define Cash_pay 0x01						//现金出货
#define Ali_pay  0x41						//支付宝购物
#define Weixin_pay	0x42					//微信支付
#define Other_pay	0x49					//兑换码出货

unsigned int nLen;							//字符串长度  
unsigned char nFcc;							//校验码
unsigned char CMD;
unsigned int nFrameSendLen;					//
unsigned int nDataSendLen;
unsigned int DataReceiveLen;
unsigned int nStandby;
unsigned int nType;
unsigned char szFCC;
static unsigned char LenValue[2]={0};
static unsigned char FccValue[2]={0};
static unsigned char Serial_numberadd[3]={0};
static unsigned char DataReceive[RECEIVE_BUF] = {0};   //value of DATA				
static unsigned char DataSend[RECEIVE_BUF] = {0};	   
static unsigned char FrameSend[RECEIVE_BUF] ={0};
static unsigned char timebuf[64]={0};           //储存当地时间
static int nDebug = 1;

void setEventID(int nEventID);
void setPollFlag(int setValue);
void setVenderID(unsigned char * szVenderID);

static int id_num = 0;
static int event_id[400] = {0};
static int nVenderTag =0;
static int PollFlag = 0;
static int nEventID = 0;
static int nReadStatus = 1;
static int nWriteStatus = 1;


#endif

