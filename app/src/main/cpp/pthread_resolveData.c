#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include "data.h"
#include "serialPort.h"
#include "pthread_resolveData.h"
#include <semaphore.h>
#include <time.h>

#include <android/log.h>
#define   LOG_TAG    "LOG_TEST"
#define   LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


struct  timeval tv;
sem_t event_sem_id;

void setEvent(int Id) {
    nEventID = Id;
    sem_post(&event_sem_id);
    LOGI("\r\nEVENTID is %d\r\n",Id);                     //三个事件来了，id_num = 3;
}

int getEvent() {
    sem_wait(&event_sem_id);  //
    return nEventID;
}


//void setEvent(int Id) {
//    unsigned int i;
//    id_num++;
//    if(id_num<399)
//        event_id[id_num-1] = Id;
//    sem_post(&event_sem_id);
//   // for(i=0;i<1000;i++);
//    LOGI("\r\nEVENTID is %d\r\n",Id);                     //三个事件来了，id_num = 3;
//}
//
//int getEvent(unsigned char *gR) {
//    int i;
//    nEventID = event_id[0];
//    memcpy(gR,event_id,400);
//
//    if(id_num>0)
//        id_num--;               //id_num = 2
//    for(i=0;i<id_num;i--)
//    {
//        event_id[i]=event_id[i+1];
//    }
//    if(id_num >=0)
//        event_id[id_num] = 0x00;
//    sem_wait(&event_sem_id);
//    return nEventID;
//}

int setDebugFlag(int nflag)
{
    if(nflag > 0)
        nDebug = 1;
    else if(nflag == 0)
        nDebug = 0;
    return 0;
}

void getCurrentTime()
{
    gettimeofday(&tv,NULL);
    strftime(timebuf,sizeof(timebuf)-1 ,"%Y-%m-%d %H:%M:%S",localtime(&tv.tv_sec));
}

void PrintDebug(unsigned char *data, int len,int type)
{
    int i = 0;
    char temp[2048], buf[128],disp[10];

    memset(temp, 0, sizeof(temp));
    memset(disp,0,sizeof(disp));
    if(1 == type)
        memcpy(disp,"[write]",sizeof("[write]"));
    else if(0 == type)
        memcpy(disp,"[read]",sizeof("[read]"));

    sprintf(temp, "\r\n%s:%d", disp, len);
    for(i = 0; i < len; i++)
    {
        if(i % 16 == 0)
        {
            strcat(temp, "\r\n");
            if(strlen(temp) >= 1024)
            {
                printf("%s", temp);
                memset(temp, 0, sizeof(temp));
            }
        }
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%02X ", data[i]);
        strcat(temp, buf);
    }
    strcat(temp, "\r\n");
    printf("%s", temp);
}


void Printmessage(unsigned char *data, int len)				//安卓打印信息
{
	int i = 0;
    char temp[2048], buf[128],disp[10];
	memset(temp, 0, sizeof(temp));
	memcpy(temp,data,len);
	for(i=0;i<len;i++)
	{
        LOGI("%02X ",temp[i]);
	}
    LOGI("\n");
}


void frameSendDataGenerate(unsigned char cmd,unsigned char ANSWER)					//ACK或者NCK
{	
	unsigned int i;
	nFcc= 0x00;
	FrameSend[0] = cmd;	
	FrameSend[1] = ANSWER;
    for(i=0;i<2;i++)
        nFcc+=FrameSend[i];
    FrameSend[2] = nFcc;
}

/************************************************设定时间**************************************************/

int setSystemtime(unsigned char* szVMCtime)											//设定时间
{
	memset(Systime_set_instruct,0,sizeof(Systime_set_instruct));
	Systime_set_instruct[0] = SYS_TIME_SET;											//
	memcpy(Systime_set_instruct+1,szVMCtime,7);
	PollFlag = 2;
	return 0;	
}

void ReturnSystimesetting()															//返回设定的时间
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																//POLL
	memcpy(DataSend+1,Systime_set_instruct,sizeof(Systime_set_instruct));
	for(i=0;i<9;i++)
		nFcc+=DataSend[i];
	DataSend[9] = nFcc;
	//Printmessage(DataSend,10);
	writeData(DataSend, 10);														//返回POLL
    PrintDebug(DataSend,10,WRITE);
}

/************************************************设定出货请求**************************************************/
int setProductsale(unsigned char cContainnum,unsigned char cChannelnum,unsigned char sale_way)						//
{
	memset(Shipment_request_instruct,0,21);
	Shipment_request_instruct[0] = ASK_SHIPMENT;										//出货请求交易码

    int money_sale;
	
	if((cContainnum == 0x09)||(cContainnum == 0x0B))									//货柜号
		Shipment_request_instruct[1] = cContainnum;
	else
		return 1;
	
		Shipment_request_instruct[2] = 0x01;											//按照料道请求出货
		
	if(cChannelnum <=48)																//料道号出货
		Shipment_request_instruct[3] = cChannelnum;
	else
		return 1;

	if(cContainnum == 0x09)																//食品柜0x09  值为09
		{
            money_sale = (int)(Channel_price_instruct[0].Channel_price[cChannelnum*2-2])*256+(int)(Channel_price_instruct[0].Channel_price[cChannelnum*2-1]);
            Shipment_request_instruct[4] = (money_sale*10)/65536;
            Shipment_request_instruct[5] = ((money_sale*10)%65536)/256;
            Shipment_request_instruct[6] = (money_sale*10)%256;
            printf("%d %d",Channel_price_instruct[0].Channel_price[0],Channel_price_instruct[0].Channel_price[1]);
            printf("the money is %d\r\n",money_sale);
		}
	else if(cContainnum == 0x0B)															//饮料柜0x0B  值为11
		{
            money_sale = (int)(Channel_price_instruct[1].Channel_price[cChannelnum*2-2])*256+(int)(Channel_price_instruct[1].Channel_price[cChannelnum*2-1]);
			Shipment_request_instruct[4] = (money_sale*10)/65536;
            Shipment_request_instruct[5] = ((money_sale*10)%65536)/256;
            Shipment_request_instruct[6] = (money_sale*10)%256;
            printf("%d %d",Channel_price_instruct[1].Channel_price[0],Channel_price_instruct[1].Channel_price[1]);
            printf("the money is %d\r\n",money_sale);
		}
	else
		return 1;
	
		if((sale_way == Cash_pay)||(sale_way == Ali_pay)||(sale_way == Weixin_pay)||(sale_way == Other_pay))
			Shipment_request_instruct[7] = sale_way;										 //支付方式
		else
			return 1;
		
		if(Serial_numberadd[1] == 255)
		{
			Serial_numberadd[0]++;
		}
		if(Serial_numberadd[2] == 255)
		{
			Serial_numberadd[1]++;
		}
			Serial_numberadd[2]++;
		
		Shipment_request_instruct[8] = Serial_numberadd[0];
		Shipment_request_instruct[9] = Serial_numberadd[1];
		Shipment_request_instruct[10] = Serial_numberadd[2];
		
		//11到20 10位卡号为0x00，不计
	PollFlag = 3;
	return 0;	
}

void ReturnProductsale()																	//真正交互值
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																		//
	memcpy(DataSend+1,Shipment_request_instruct,sizeof(Shipment_request_instruct));
	for(i=0;i<22;i++)
		nFcc+=DataSend[i];
	DataSend[22] = nFcc;
	Printmessage(DataSend,23);
	writeData(DataSend, 23);
    PrintDebug(DataSend,23,WRITE);
}

/************************************************设置料道信息，就是价格**************************************************/
int setChannelprice(unsigned char cContainnum,unsigned char cChannelnum,int cChannelprice)		//
{
	memset(Set_channelprice_instruct,0,sizeof(Set_channelprice_instruct));
	Set_channelprice_instruct[0] = SET_CHANNEL_PRICE;
	
	if((cContainnum == 0x09)||(cContainnum == 0x0B))									//货柜号
		Set_channelprice_instruct[1] = cContainnum;
	else
		return 1;

	if(cChannelnum <= 48)
		Set_channelprice_instruct[2] = cChannelnum;
	else
		return 1;
	
	if(cContainnum == 0x09)																//食品柜商品序号设置默认1-48，饮料柜商品序号设置默认49-96
		Set_channelprice_instruct[3] = cChannelnum;
	else if(cContainnum == 0x0B)
		Set_channelprice_instruct[3] = (unsigned char)(cChannelnum+48);
	
	if(cChannelprice <= 65535)
	{
		Set_channelprice_instruct[4] = (unsigned char)((cChannelprice/10)/256);
		Set_channelprice_instruct[5] = (unsigned char)((cChannelprice/10)%256);
	}
    Set_channelprice_instruct[6] = 0;
    Set_channelprice_instruct[7] = 0;
    printf("price = %d\r\n",cChannelprice);
    printf("5 = %d\r\n",Set_channelprice_instruct[4]);
    printf("6 = %d\r\n",Set_channelprice_instruct[5]);
	PollFlag = 4;
	return 0;
}

void ReturnChannelprice()																	//更改料道价格
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																		//POLL
	memcpy(DataSend+1,Set_channelprice_instruct,sizeof(Set_channelprice_instruct));
	for(i=0;i<9;i++)
		nFcc+=DataSend[i];
	DataSend[9] = nFcc;
	writeData(DataSend, 10);
    PrintDebug(DataSend,10,WRITE);
}

/************************************************设置料道加货信息**************************************************/
int setChanneladd(unsigned char cContainnum,unsigned char cChannelnum,int cAddnum)			//
{
	memset(Sys_channeladd_instruct,0,sizeof(Sys_channeladd_instruct));
	Sys_channeladd_instruct[0] = CHANNEL_GOOD_ADD;								//交易码
	
	if((cContainnum == 0x09)||(cContainnum == 0x0B))									//货柜号
		Set_channelprice_instruct[1] = cContainnum;
	else
		return 1;
	
	Set_channelprice_instruct[2] = cChannelnum;
	
	if(cAddnum<256)
		Set_channelprice_instruct[2] = cAddnum;
	else	
		return 1;
	
	PollFlag = 5;
	return 0;	
}

void ReturnChanneladd()															//货道添加商品
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																		//
	memcpy(DataSend+1,Set_channelprice_instruct,sizeof(Set_channelprice_instruct));
	for(i=0;i<7;i++)
		nFcc+=DataSend[i];
	DataSend[7] = nFcc;
	writeData(DataSend, 8);
    PrintDebug(DataSend,8,WRITE);
}

/*******************************************设置系统状态信息**************************************/
int setSystemstate(unsigned char* szSysconfig)											//
{
	memset(Set_system_instruct,0,sizeof(Set_system_instruct));
	memcpy(Set_system_instruct,szSysconfig,16);
	PollFlag = 6;
	return 0;
}

void ReturnSystemstate()															//
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																		//
	memcpy(DataSend+1,Set_system_instruct,sizeof(Set_system_instruct));
	for(i=0;i<17;i++)
		nFcc+=DataSend[i];
	DataSend[17] = nFcc;
	writeData(DataSend, 18);
    PrintDebug(DataSend,18,WRITE);
}

/*******************************************暂停与恢复售卖**************************************/
int setMachinerun(unsigned char on_off)										//
{
	memset(Machine_onoff_instruct,0,sizeof(Machine_onoff_instruct));
	//memcpy(Machine_onoff_instruct,szSysconfig,16);
	Machine_onoff_instruct[0] = MACHINE_ONOFF;
	if((on_off == ON_KEY)||(on_off == OFF_KEY))
		Machine_onoff_instruct[1] = on_off;
	PollFlag = 7;
	return 0;
}

void ReturnMachinerun()															//
{
	unsigned int i;
	nFcc= 0x00;
	DataSend[0] = POLL;																		//
	memcpy(DataSend+1,Machine_onoff_instruct,sizeof(Machine_onoff_instruct));
	
	for(i=0;i<4;i++)
		nFcc+=DataSend[i];
	DataSend[4] = nFcc;
	writeData(DataSend, 5);
    PrintDebug(DataSend,5,WRITE);
}

/********************************************料道解除锁定**************************************/


/********************************************获取机器号****************************************/


/**********************************************以上为设定参数部分******************************************************/

void clearData()
{	
    memset(FrameReceive,0, sizeof(FrameReceive));
	memset(FrameSend,0, sizeof(FrameSend));
	memset(DataReceive,0, sizeof(DataReceive));
	memset(DataSend,0, sizeof(DataSend));
	nDataSendLen = 0;
	PollFlag = 0;
}

int getSigninMessage(unsigned char *gR)												//签到信息
{
	unsigned char getVCMinformation[20];
	int i;
	getVCMinformation[0] = Sign_in_instruct.Master_version[0];
	getVCMinformation[1] = Sign_in_instruct.Master_version[1];
	getVCMinformation[2] = Sign_in_instruct.Machine_num[0];
	getVCMinformation[3] = Sign_in_instruct.Machine_num[1];
	getVCMinformation[4] = Sign_in_instruct.Machine_num[2];
	getVCMinformation[5] = Sign_in_instruct.Machine_num[3];
	
	for(i=0;i<7;i++)
		getVCMinformation[6+i] = Sign_in_instruct.Time[i];
	
	getVCMinformation[13] = Sign_in_instruct.Machine_class;
	getVCMinformation[14] = Sign_in_instruct.Drink_num;
	getVCMinformation[15] = Sign_in_instruct.Food_num;
	getVCMinformation[16] = Sign_in_instruct.Noodle_num;
	getVCMinformation[17] = Sign_in_instruct.Drink_channel_num;
	getVCMinformation[18] = Sign_in_instruct.Food_channel_num;
	getVCMinformation[19] = Sign_in_instruct.Noodle_channel_num;
	
	memcpy(gR,getVCMinformation,20);
	return 20;	
}

int getPollMessage(unsigned char *gR)												//POLL信息
{
	unsigned char getVCMinformation[11];
	unsigned int i;
	for(i=0;i<6;i++)
		getVCMinformation[i] = Poll_instruct.Total_money[i];
	
	for(i=0;i<5;i++)
		getVCMinformation[6+i] = Poll_instruct.Get_container_num[i];
	
	memcpy(gR,getVCMinformation,7);
	return 11;
}

int getVenderOutReport(unsigned char *gR)											//出货报告
{
	unsigned int i;
	unsigned char getVCMinformation[29];
	
	getVCMinformation[0] = Shipment_instruct.Container_num;
	getVCMinformation[1] = Shipment_instruct.Channel_num;
	getVCMinformation[2] = Shipment_instruct.Product_num;
	getVCMinformation[3] = Shipment_instruct.Product_price[0];
	getVCMinformation[4] = Shipment_instruct.Product_price[1];
	getVCMinformation[5] = Shipment_instruct.Product_price[2];
	getVCMinformation[6] = Shipment_instruct.Serial_number;
	getVCMinformation[7] = Shipment_instruct.Payment_way;
	getVCMinformation[8] = Shipment_instruct.Product_out_station;
	getVCMinformation[9] = Shipment_instruct.Outfailed_reason;
	for(i=0;i<9;i++)
		getVCMinformation[10+i] = Shipment_instruct.Card_num[i];
	getVCMinformation[19] = Shipment_instruct.Serial_number2[0];
	getVCMinformation[20] = Shipment_instruct.Serial_number2[1];
	getVCMinformation[21] = Shipment_instruct.Serial_number2[2];
	getVCMinformation[22] = Shipment_instruct.Product_sale_num[0];
	getVCMinformation[23] = Shipment_instruct.Product_sale_num[1];
	getVCMinformation[24] = Shipment_instruct.Product_sale_num[2];
	getVCMinformation[25] = Shipment_instruct.Product_sale_money[0];
	getVCMinformation[26] = Shipment_instruct.Product_sale_money[1];
	getVCMinformation[27] = Shipment_instruct.Product_sale_money[2];
	getVCMinformation[28] = Shipment_instruct.Product_sale_money[3];
	
	memcpy(gR,getVCMinformation,29);
	return 29;
}


int getChannelRunMessage(unsigned char *gR,unsigned char cContainnum)									//料道运行信息
{
	unsigned char getVCMinformation[55];
	int num_channel;			
	int i;	//我们设定09对应的是Channle_run_instruct[0]
	
	if(cContainnum == 0x09)
		num_channel = 0;
	else if( cContainnum == 0x0B)
		num_channel = 1;
	else
		return 0;
	
	getVCMinformation[0] = Channle_run_instruct[num_channel].Container_num;
	
	for(i=0;i<6;i++)
		getVCMinformation[1+i] = Channle_run_instruct[num_channel].Channel_fault[i];
	
	for(i=0;i<48;i++)
		getVCMinformation[7+i] = Channle_run_instruct[num_channel].Channel_sale[i];
	
	memcpy(gR,getVCMinformation,48);
	return 48;
}


int getMachineRunMessage(unsigned char *gR)																//机器运行信息
{
	unsigned char getVCMinformation[10];
	
	getVCMinformation[0] = Mashion_run_instruct.Machine_run;
	getVCMinformation[1] = Mashion_run_instruct.VMC_station;
	getVCMinformation[2] = Mashion_run_instruct.Note_machine_state;
	getVCMinformation[3] = Mashion_run_instruct.Coin_machine_state;
	getVCMinformation[4] = Mashion_run_instruct.Coin_num[0];
	getVCMinformation[5] = Mashion_run_instruct.Coin_num[1];
	getVCMinformation[6] = Mashion_run_instruct.Coin_num[2];
	getVCMinformation[7] = Mashion_run_instruct.Coin_num[3];
	getVCMinformation[8] = Mashion_run_instruct.Note_num[0];	
	getVCMinformation[9] = Mashion_run_instruct.Note_num[1];
	
	memcpy(gR,getVCMinformation,10);
	return 10;
}

int getSystemSetMessage(unsigned char *gR,unsigned char cContainnum)																//系统配置信息
{
	unsigned char getVCMinformation[17];
	int num_channel;																					//我们设定09对应的是Sys_set_instruct[0]
	
	if(cContainnum == 0x09)
		num_channel = 0;
	else if( cContainnum == 0x0B)
		num_channel = 1;
	else
		return 0;
	
	getVCMinformation[0] = Sys_set_instruct[num_channel].Container_num;
	getVCMinformation[1] = Sys_set_instruct[num_channel].Sale_set;
	getVCMinformation[2] = Sys_set_instruct[num_channel].Led_state;
	getVCMinformation[3] = Sys_set_instruct[num_channel].Led_time[0];
	getVCMinformation[4] = Sys_set_instruct[num_channel].Led_time[1];
	getVCMinformation[5] = Sys_set_instruct[num_channel].Led_time[2];
	getVCMinformation[6] = Sys_set_instruct[num_channel].Led_time[3];
	
	getVCMinformation[7] = Sys_set_instruct[num_channel].Left_control_state;
	getVCMinformation[8] = Sys_set_instruct[num_channel].Right_control_state;
	getVCMinformation[9] = Sys_set_instruct[num_channel].Save_time[0];
	getVCMinformation[10] = Sys_set_instruct[num_channel].Save_time[1];
	getVCMinformation[11] = Sys_set_instruct[num_channel].Save_time[2];
	getVCMinformation[12] = Sys_set_instruct[num_channel].Save_time[3];
	
	getVCMinformation[13] = Sys_set_instruct[num_channel].Coldtem_set;
	getVCMinformation[14] = Sys_set_instruct[num_channel].Coldtem_real;
	getVCMinformation[15] = Sys_set_instruct[num_channel].Hottem_set;
	getVCMinformation[16] = Sys_set_instruct[num_channel].Hottem_real;
		
	memcpy(gR,getVCMinformation,17);
	return 17;
}

int getChannelGoodsMessage(unsigned char *gR,unsigned char cContainnum)																//料道商品信息
{
	unsigned char getVCMinformation[49];
	int num_channel;																					//我们设定09对应的是Channel_product_instruct[0]
	int i;
	if(cContainnum == 0x09)
		num_channel = 0;
	else if( cContainnum == 0x0B)
		num_channel = 1;
	else
		return 0;

	getVCMinformation[0] = Channel_product_instruct[num_channel].Container_num;
	
	for(i=0;i<48;i++)
		getVCMinformation[1+i] = Channel_product_instruct[num_channel].Channel_poduct[i];
	
	memcpy(gR,getVCMinformation,49);
	return 49;
}


int getChannelPriceMessage(unsigned char *gR,unsigned char cContainnum)																//料道价格信息
{
	unsigned char getVCMinformation[97];
	int num_channel;	
	int i;	//我们设定09对应的是Channel_product_instruct[0]
	
	if(cContainnum == 0x09)
		num_channel = 0;
	else if( cContainnum == 0x0B)
		num_channel = 1;
	else
		return 0;
	
	getVCMinformation[0] = Channel_price_instruct[num_channel].Container_num;
	
	for(i=0;i<96;i++)
		getVCMinformation[1+i] = Channel_price_instruct[num_channel].Channel_price[i];
	
	memcpy(gR,getVCMinformation,97);
	return 97;
}

void setPollFlag(int setValue)
{
	PollFlag = setValue;
	printf("*********************PollFlag = %d\n",PollFlag);
}

int  setReturnCoin()
{
    PollFlag = 8;
    return 0;
}

void returnCoin()
{
    unsigned int i;
    nFcc= 0x00;
    DataSend[0] = POLL;
    DataSend[1] = ASK_SHIPMENT;
    DataSend[2] = 97;
    DataSend[3] = 0x01;
    DataSend[4] = 0x01;
    DataSend[5] = 0x00;
    DataSend[6] = 0x00;
    DataSend[7] = 0x00;
    DataSend[8] = Cash_pay;

    if(Serial_numberadd[1] == 255)
    {
        Serial_numberadd[0]++;
    }
    if(Serial_numberadd[2] == 255)
    {
        Serial_numberadd[1]++;
    }
    Serial_numberadd[2]++;

    DataSend[9] = Serial_numberadd[0];
    DataSend[10] = Serial_numberadd[1];
    DataSend[11] = Serial_numberadd[2];

    for(i=0;i<10;i++)
    {
        DataSend[12+i] = 0x00;
    }
    for(i=0;i<22;i++)
        nFcc+=DataSend[i];
    DataSend[22] = nFcc;
    Printmessage(DataSend,23);
    writeData(DataSend, 23);
    PrintDebug(DataSend,23,WRITE);
}

void* pthread_resolveData(void *arg) {
    FILE * STREAM;
    unsigned int i;
    unsigned int pollnum = 0;
    int num_channel;
    unsigned int j;
    unsigned int aislenum;
    unsigned char set_event_flag = 0;                //传输的数据，变化才发事件信息。//我们设定09对应的是Channle_run_instruct[0]
    unsigned char set_poll_flag = 1;                //传输的数据，变化才发事件信息。//我们设定09对应的是Channle_run_instruct[0]
    unsigned char set_ack_flag = 0;                  //设定的反馈标志。

    char filename[60] = "/sdcard/VMC/log";
    int f_original_len = strlen(filename);
    getCurrentTime();
    memcpy(filename+f_original_len,timebuf,19);
    memcpy(filename+f_original_len+19,".txt",4);
    STREAM = freopen(filename,"w",stdout);
    setbuf(stdout, NULL);
    if(STREAM == NULL)
    {
        LOGI("log create error\n");
    }else
    {
            LOGI("log create OK\n");
    }

    while(1){
        nFcc = 0x00;
        readData(FrameReceive,3);                                                    // read data from serial

        if ((FrameReceive[1] == TAGGED_WORD_ONE) && (FrameReceive[2] == TAGGED_WORD_TWO)) {
            CMD = FrameReceive[0];
            switch (CMD) {
                case Unknow_mes:                                                   //测试出来的，未知指令
                    readData(FrameReceive + 3, 40);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 43);  //get data in receive

                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,43,READ);

                    for (j = 0;j<42;j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[42]) {
                        frameSendDataGenerate(Unknow_mes, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********Unknow_mes successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(Unknow_mes, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********Unknow_mes error!**********\r\n");
                    }
                    clearData();
                    break;

                case SIGN_IN:                                                                        //签到事件
                    readData(FrameReceive + 3,22);                                                    // read data from serial
                    memcpy(DataReceive, FrameReceive,25);                                            //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,25,READ);
                    Sign_in_instruct.Master_version[0] = DataReceive[4];
                    Sign_in_instruct.Master_version[1] = DataReceive[5];
                    Sign_in_instruct.Machine_num[0] = DataReceive[6];
                    Sign_in_instruct.Machine_num[1] = DataReceive[7];
                    Sign_in_instruct.Machine_num[2] = DataReceive[8];
                    Sign_in_instruct.Machine_num[3] = DataReceive[9];

                    for (i = 0; i < 7; i++)
                        Sign_in_instruct.Time[i] = DataReceive[10 + i];

                    Sign_in_instruct.Machine_class = DataReceive[17];
                    Sign_in_instruct.Drink_num = DataReceive[18];
                    Sign_in_instruct.Food_num = DataReceive[19];
                    Sign_in_instruct.Noodle_num = DataReceive[20];
                    Sign_in_instruct.Drink_channel_num = DataReceive[21];
                    Sign_in_instruct.Food_channel_num = DataReceive[22];
                    Sign_in_instruct.Noodle_channel_num = DataReceive[23];

                    for (j = 0; j < 24; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[24]) {
                        frameSendDataGenerate(SIGN_IN, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        LOGI("*********SIGN_IN successful!**********\n");
                        printf("ACK *********SIGN_IN successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(SIGN_IN, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********SIGN_IN error!**********\r\n");
                        LOGI("*********SIGN_IN error!**********\n");
                    }
                    setEvent(EVENT_SIGN_IN);                                                                                //发送签到事件
                    clearData();
                    break;

                case CHANNEL_RUN:                                                                                            //料道运行信息2s一次
                    readData(FrameReceive + 3, 57);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 60);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,60,READ);

                    if (DataReceive[4] == 0x09)
                        num_channel = 0;
                    else if (DataReceive[4] == 0x0B)
                        num_channel = 1;
                    else
                        frameSendDataGenerate(CHANNEL_RUN, NAK);

                    if(Channle_run_instruct[num_channel].Container_num != DataReceive[4])
                    {
                        Channle_run_instruct[num_channel].Container_num = DataReceive[4];
                        set_event_flag = 1;
                    }

                    for (i = 0; i < 6; i++)
                    {
                        if(Channle_run_instruct[num_channel].Channel_fault[i] != DataReceive[5 + i])
                        {
                            Channle_run_instruct[num_channel].Channel_fault[i] = DataReceive[5 + i];
                            set_event_flag = 1;
                        }
                    }

                    for (i = 0; i < 48; i++)
                    {
                        if(Channle_run_instruct[num_channel].Channel_sale[i] != DataReceive[11 + i])
                        {
                            Channle_run_instruct[num_channel].Channel_sale[i] = DataReceive[11 + i];
                            set_event_flag = 1;
                        }
                    }

                    for (j = 0; j < 59; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[59]) {
                        frameSendDataGenerate(CHANNEL_RUN, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********CHANNEL_RUN successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(CHANNEL_RUN, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********CHANNEL_RUN error!**********\r\n");
                    }
                    if ((DataReceive[4] == 0x09)&&(set_event_flag ==1))
                    {
                        setEvent(EVENT_CHANNEL_MESSAGE2);
                        set_event_flag = 0;
                    }
                    else if ((DataReceive[4] == 0x0B)&&(set_event_flag == 1))
                    {
                        setEvent(EVENT_CHANNEL_MESSAGE);
                        set_event_flag = 0;
                    }
                    clearData();
                    break;

                case SYSTEM_CONFIG:                                                                                            //系统配置信息2s一次
                    readData(FrameReceive + 3, 19);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 22);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,22,READ);

                    if (DataReceive[4] == 0x09)
                        num_channel = 0;
                    else if (DataReceive[4] == 0x0B)
                        num_channel = 1;
                    else
                        frameSendDataGenerate(SYSTEM_CONFIG, NAK);

                    if(Sys_set_instruct[num_channel].Container_num != DataReceive[4]) {
                        Sys_set_instruct[num_channel].Container_num = DataReceive[4];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Sale_set != DataReceive[5]){
                        Sys_set_instruct[num_channel].Sale_set = DataReceive[5];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_state != DataReceive[6]){
                        Sys_set_instruct[num_channel].Led_state = DataReceive[6];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_time[0] != DataReceive[7]){
                        Sys_set_instruct[num_channel].Led_time[0] = DataReceive[7];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_time[0] != DataReceive[7]){
                        Sys_set_instruct[num_channel].Led_time[0] = DataReceive[7];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_time[1] != DataReceive[8]){
                        Sys_set_instruct[num_channel].Led_time[1] = DataReceive[8];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_time[2] != DataReceive[9]){
                        Sys_set_instruct[num_channel].Led_time[2] = DataReceive[9];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Led_time[3] != DataReceive[10]){
                        Sys_set_instruct[num_channel].Led_time[3] = DataReceive[10];
                        set_event_flag = 1;
                    }
                    if( Sys_set_instruct[num_channel].Left_control_state != DataReceive[11]){
                        Sys_set_instruct[num_channel].Left_control_state = DataReceive[11];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Right_control_state != DataReceive[12]){
                        Sys_set_instruct[num_channel].Right_control_state = DataReceive[12];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Save_time[0] != DataReceive[13]){
                        Sys_set_instruct[num_channel].Save_time[0] = DataReceive[13];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Save_time[1] != DataReceive[14]){
                        Sys_set_instruct[num_channel].Save_time[1] = DataReceive[14];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Save_time[2] != DataReceive[15]){
                        Sys_set_instruct[num_channel].Save_time[2] = DataReceive[15];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Save_time[3] != DataReceive[16]){
                        Sys_set_instruct[num_channel].Save_time[3] = DataReceive[16];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Coldtem_set != DataReceive[17]){
                        Sys_set_instruct[num_channel].Coldtem_set = DataReceive[17];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Coldtem_real != DataReceive[18]){
                        Sys_set_instruct[num_channel].Coldtem_real = DataReceive[18];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Hottem_set != DataReceive[19]){
                        Sys_set_instruct[num_channel].Hottem_set = DataReceive[19];
                        set_event_flag = 1;
                    }
                    if(Sys_set_instruct[num_channel].Hottem_real != DataReceive[20]){
                        Sys_set_instruct[num_channel].Hottem_real = DataReceive[20];
                        set_event_flag = 1;
                    }

                    for (j = 0; j < 21; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[21]) {
                        frameSendDataGenerate(SYSTEM_CONFIG, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********SYSTEM_CONFIG successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(SYSTEM_CONFIG, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********SYSTEM_CONFIG error!**********\r\n");
                    }
                    if ((set_event_flag == 1)&&(DataReceive[4] == 0x09))
                    {
                        setEvent(EVENT_SYSTEM_SETTING2);
                        set_event_flag = 0;
                    }
                    else if ((set_event_flag == 1)&&(DataReceive[4] == 0x0B))
                    {
                        setEvent(EVENT_SYSTEM_SETTING);
                        set_event_flag = 0;
                    }
                    clearData();
                    break;

                case PRODUCT_SALES:                                                            //商品销售信息，暂时不需要详细数量，只是进行回复
                    readData(FrameReceive + 3, 78);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 81);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,81,READ);

                    for (j = 0; j < 80; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[80]) {
                        frameSendDataGenerate(PRODUCT_SALES, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********PRODUCT_SALES successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(PRODUCT_SALES, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********PRODUCT_SALES error!**********\r\n");
                    }
                    clearData();
                    break;

                case SHIPMENT:                                                            //商品出货信息
                    readData(FrameReceive + 3, 31);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 34);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,34,READ);

                    Shipment_instruct.Container_num = DataReceive[4];
                    Shipment_instruct.Channel_num = DataReceive[5];
                    Shipment_instruct.Product_num = DataReceive[6];
                    Shipment_instruct.Product_price[0] = DataReceive[7];
                    Shipment_instruct.Product_price[1] = DataReceive[8];
                    Shipment_instruct.Product_price[2] = DataReceive[9];
                    Shipment_instruct.Serial_number = DataReceive[10];
                    Shipment_instruct.Payment_way = DataReceive[11];
                    Shipment_instruct.Product_out_station = DataReceive[12];
                    Shipment_instruct.Outfailed_reason = DataReceive[13];
                    for (i = 0; i < 9; i++)
                        Shipment_instruct.Card_num[i] = DataReceive[14 + i];
                    Shipment_instruct.Serial_number2[0] = DataReceive[23];
                    Shipment_instruct.Serial_number2[1] = DataReceive[24];
                    Shipment_instruct.Serial_number2[2] = DataReceive[25];
                    Shipment_instruct.Product_sale_num[0] = DataReceive[26];
                    Shipment_instruct.Product_sale_num[1] = DataReceive[27];
                    Shipment_instruct.Product_sale_num[2] = DataReceive[28];
                    Shipment_instruct.Product_sale_money[0] = DataReceive[29];
                    Shipment_instruct.Product_sale_money[1] = DataReceive[30];
                    Shipment_instruct.Product_sale_money[2] = DataReceive[31];
                    Shipment_instruct.Product_sale_money[3] = DataReceive[32];

                    for (j = 0; j < 33; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[33]) {
                        frameSendDataGenerate(SHIPMENT, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********SHIPMENT successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(SHIPMENT, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********SHIPMENT error!**********\r\n");
                    }
                    setEvent(EVENT_OUTGOODS_RESULT);                                                //发送出货报告
                    clearData();
                    break;

                case MACHINE_RUN:                                                                    //机器运行信息，变化才发送时间
                    readData(FrameReceive + 3, 12);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 15);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,15,READ);

                    if (Mashion_run_instruct.Machine_run != DataReceive[4]) {
                        if ((Mashion_run_instruct.Machine_run & 0x02) && !(DataReceive[4] & 0x02))
                         setEvent(EVENT_DOOR_STATE_CLOSE);                                        //原来门是开的，现在门关了，发送关门事件
                        if (!(Mashion_run_instruct.Machine_run & 0x02) && (DataReceive[4] & 0x02))
                            setEvent(EVENT_DOOR_STATE_OPEN);                                        //原来门是关的，现在门开了，发送开门事件
                        Mashion_run_instruct.Machine_run = DataReceive[4];
                    }

                    if (Mashion_run_instruct.VMC_station != DataReceive[5]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.VMC_station = DataReceive[5];
                    }
                    if (Mashion_run_instruct.Note_machine_state != DataReceive[6]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Note_machine_state = DataReceive[6];
                    }
                    if (Mashion_run_instruct.Coin_machine_state != DataReceive[7]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Coin_machine_state = DataReceive[7];
                    }
                    if (Mashion_run_instruct.Coin_num[0] != DataReceive[8]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Coin_num[0] = DataReceive[8];
                    }
                    if (Mashion_run_instruct.Coin_num[1] != DataReceive[9]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Coin_num[1] = DataReceive[9];
                    }
                    if (Mashion_run_instruct.Coin_num[2] != DataReceive[10]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Coin_num[2] = DataReceive[10];
                    }
                    if (Mashion_run_instruct.Coin_num[3] != DataReceive[11]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Coin_num[3] = DataReceive[11];
                    }

                    if (Mashion_run_instruct.Note_num[0] != DataReceive[12]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Note_num[0] = DataReceive[12];
                    }
                    if (Mashion_run_instruct.Note_num[1] != DataReceive[13]) {
                        set_event_flag = 1;
                        Mashion_run_instruct.Note_num[1] = DataReceive[13];
                    }

                    for (j = 0; j < 14; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[14]) {
                        frameSendDataGenerate(MACHINE_RUN, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********MACHINE_RUN successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(MACHINE_RUN, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********MACHINE_RUN error!**********\r\n");
                    }
                    if (set_event_flag == 1) {
                        setEvent(EVENT_MASHION_MESSAGE);                                                //发送机器运行信息事件
                        }//clearData();
                    break;

                case CHANNEL_GOOD:                                                                        //料道商品事件
                    readData(FrameReceive + 3, 51);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 54);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,54,READ);

                    if (DataReceive[4] == 0x09)
                        num_channel = 0;
                    else if (DataReceive[4] == 0x0B)
                        num_channel = 1;
                    else
                        frameSendDataGenerate(CHANNEL_GOOD, NAK);

                    Channel_product_instruct[num_channel].Container_num = DataReceive[4];

                    for (i = 0; i < 48; i++)
                        Channel_product_instruct[num_channel].Channel_poduct[i] = DataReceive[5 + i];

                    for (j = 0; j < 53; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[53]) {
                        frameSendDataGenerate(CHANNEL_GOOD, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********CHANNEL_GOOD successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(CHANNEL_GOOD, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********CHANNEL_GOOD error!**********\r\n");
                    }
                    if (DataReceive[4] == 0x09)
                        setEvent(EVENT_GOODS_MESSAGE2);
                    else if (DataReceive[4] == 0x0B)
                        setEvent(EVENT_GOODS_MESSAGE);                                                            //料道商品信息事件
                    clearData();
                    break;

                case CHANNEL_PRICE:
                    readData(FrameReceive + 3, 99);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 102);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,102,READ);

                    if (DataReceive[4] == 0x09)
                        num_channel = 0;
                    else if (DataReceive[4] == 0x0B)
                        num_channel = 1;
                    else
                        frameSendDataGenerate(CHANNEL_PRICE, NAK);

                    Channel_price_instruct[num_channel].Container_num = DataReceive[4];

                    for (i = 0; i < 96; i++)
                        Channel_price_instruct[num_channel].Channel_price[i] = DataReceive[5 + i];

                    printf("input money is %d,%d",DataReceive[5],DataReceive[6]);
                    printf("input money is %d,%d",  Channel_price_instruct[num_channel].Channel_price[0],  Channel_price_instruct[num_channel].Channel_price[1]);

                    for (j = 0; j < 101; j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[101]) {
                        frameSendDataGenerate(CHANNEL_PRICE, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********CHANNEL_PRICE successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(CHANNEL_PRICE, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********CHANNEL_PRICE error!**********\r\n");
                    }

                    if (DataReceive[4] == 0x09)
                        setEvent(EVENT_GOODS_PRICE2);
                    else if (DataReceive[4] == 0x0B)
                        setEvent(EVENT_GOODS_PRICE);                                                            //料道商品信息事件
                    clearData();
                    break;

                case CHANNEL_SALE:                                                                                //传递料道销售统计，不记录，只回复
                    readData(FrameReceive + 3, 148);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 151);  //get data in receive

                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,151,READ);

                    for (j = 0;j<150;j++)
                        nFcc += DataReceive[j];

                    if (nFcc == DataReceive[150]) {
                        frameSendDataGenerate(CHANNEL_SALE, ACK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********CHANNEL_SALE successful!**********\r\n");
                    } else {
                        frameSendDataGenerate(CHANNEL_SALE, NAK);
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("NAK *********CHANNEL_SALE error!**********\r\n");
                    }
                    clearData();
                    break;
                    /////////////////////////////////////////////////////////////////////////////////////////////////////
                case POLL:
                    readData(FrameReceive + 3, 13);            // read data from serial
                    memcpy(DataReceive, FrameReceive, 16);  //get data in receive
                    getCurrentTime();
                    printf("\r\n%s",timebuf);
                    PrintDebug(FrameReceive,16,READ);

                    if( (Poll_instruct.Get_container_num[0] != DataReceive[10])||
                        (Poll_instruct.Get_container_num[1] != DataReceive[11])||
                        (Poll_instruct.Get_container_num[2] != DataReceive[12])||
                        (Poll_instruct.Get_container_num[3] != DataReceive[13])||
                        (Poll_instruct.Get_container_num[4] != DataReceive[14]))
                    {
                        Poll_instruct.Get_container_num[0] = DataReceive[10];
                        Poll_instruct.Get_container_num[1] = DataReceive[11];
                        Poll_instruct.Get_container_num[2] = DataReceive[12];
                        Poll_instruct.Get_container_num[3] = DataReceive[13];
                        Poll_instruct.Get_container_num[4] = DataReceive[14];
                        //set_poll_flag = 1;
                        setEvent(EVENT_CHANNEL_CHANGED);
                    }

                    if ((Poll_instruct.Total_money[0] != DataReceive[4]) ||
                        (Poll_instruct.Total_money[1] != DataReceive[5]) ||
                        (Poll_instruct.Total_money[2] != DataReceive[6]) ||
                        (Poll_instruct.Total_money[3] != DataReceive[7]) ||
                        (Poll_instruct.Total_money[4] != DataReceive[8]) ||
                        (Poll_instruct.Total_money[5] != DataReceive[9])) {
                        //set_poll_flag = 1;
                        setEvent(EVENT_CASH_CHANGED);
                        Poll_instruct.Total_money[0] = DataReceive[4];
                        Poll_instruct.Total_money[1] = DataReceive[5];
                        Poll_instruct.Total_money[2] = DataReceive[6];
                        Poll_instruct.Total_money[3] = DataReceive[7];
                        Poll_instruct.Total_money[4] = DataReceive[8];
                        Poll_instruct.Total_money[5] = DataReceive[9];
                    }

                    if (set_poll_flag == 1) {
                        set_poll_flag = 0;
                        setEvent(EVENT_GET_POLL);
                    }

                    if (PollFlag == 2) {
                        //setSystemtime(unsigned char* szVMCtime);
                        set_ack_flag = 1;
                        ReturnSystimesetting();                                                                    //返回设定的时间
                        clearData();
                        break;
                    } else if (PollFlag == 3) {
                        //setProductsale(0,60,4);
                        set_ack_flag = 1;
                        //setProductsale(0x0B, 1, Cash_pay);                                                         //09，0b;1-48;Cash_pay,Ali_pay,Weixin_pay,Other_pay
                        ReturnProductsale();                                                                       //出货
                        clearData();
                        break;
                    } else if (PollFlag == 4) {
                        set_ack_flag = 1;
                        //setChannelprice(0x0B, 1, 500);                                                            //设定价格
                        ReturnChannelprice();                                                                     //更改料道价格
                        clearData();
                        break;
                    } else if (PollFlag == 5) {
                        set_ack_flag = 1;
                        ReturnChanneladd();                                                                       //货道添加商品
                        clearData();
                        break;
                    } else if (PollFlag == 6) {
                        set_ack_flag = 1;
                        ReturnSystemstate();                                                                     //系统配置信息
                        clearData();
                        break;
                    } else if (PollFlag == 7) {
                        set_ack_flag = 1;
                      //  setMachinerun(ON_KEY);                                                                    //OFF_KEY
                        ReturnMachinerun();                                                                        //机器运行还是暂停
                        clearData();
                        break;
                    } else if (PollFlag == 8) {                                                                //设定退币接口
                        set_ack_flag = 1;
                        returnCoin();
                        clearData();
                        break;
                    } else {
                        frameSendDataGenerate(POLL,ACK);                                                          //无命令，就回ACK
                        writeData(FrameSend, 3);
                        PrintDebug(FrameSend,3,WRITE);
                        printf("ACK *********POLL successful!**********\r\n");
                        clearData();
                        break;
                    }
                default:
                    printf("what's the matter?");
                    PrintDebug(FrameReceive,3,READ);
                    break;

            }
        } else if ((set_ack_flag == 1)&&(FrameReceive[1] == ACK) || (FrameReceive[1] == NAK)) {
            set_ack_flag = 0;
            if (FrameReceive[1] == NAK) {//设定参数失败事件
                setEvent(EVENT_SET_FAILD);
                printf("NAK *********SET error!**********\r\n");
                printf("ACK *********SET successful!**********\r\n");
            }
            else if(FrameReceive[1] == ACK){
                setEvent(EVENT_SET_SUCCESS);                                                //设定参数失败事件
                printf("ACK *********SET successful!**********\r\n");
                printf("ACK *********SET successful!**********\r\n");
            }
            clearData();
        } else{
            printf("connect error?");
            PrintDebug(FrameReceive,3,READ);
        }
    }
}





