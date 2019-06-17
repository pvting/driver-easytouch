#ifndef  _RESOLVE_H_
#define  _RESOLVE_H_

void* pthread_resolveData(void *arg);

void* threadDaemon(void *arg);

extern int startProtocol();

extern void setEvent(int Id);

extern int getEvent();

extern int setDebugFlag(int nflag);

extern void PrintDebug(unsigned char *data, int len,int type);

extern void Printmessage(unsigned char *data, int len);

extern void frameSendDataGenerate(unsigned char cmd,unsigned char ANSWER);

extern int setSystemtime(unsigned char* szVMCtime);

extern void ReturnSystimesetting();

extern int setProductsale(unsigned char cContainnum,unsigned char cChannelnum,unsigned char sale_way);

extern void ReturnProductsale();

extern int setChannelprice(unsigned char cContainnum,unsigned char cChannelnum,int cChannelprice);

extern void ReturnChannelprice();

extern int setChanneladd(unsigned char cContainnum,unsigned char cChannelnum,int cAddnum);

extern void ReturnChanneladd();

extern int setSystemstate(unsigned char* szSysconfig);

extern void ReturnSystemstate();

extern int setMachinerun(unsigned char on_off);

extern void ReturnMachinerun();

extern void clearData();

extern int getSigninMessage(unsigned char *gR);

extern int getPollMessage(unsigned char *gR);

extern int getVenderOutReport(unsigned char *gR);

extern int getChannelRunMessage(unsigned char *gR,unsigned char cContainnum);

extern int getMachineRunMessage(unsigned char *gR);

extern int getSystemSetMessage(unsigned char *gR,unsigned char cContainnum);

extern int getChannelGoodsMessage(unsigned char *gR,unsigned char cContainnum);

extern int getChannelPriceMessage(unsigned char *gR,unsigned char cContainnum);

extern void setPollFlag(int setValue);

extern int  setReturnCoin();

extern void returnCoin();

#endif

