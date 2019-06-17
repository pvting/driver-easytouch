#ifndef  _SERIALPORT_H_
#define  _SERIALPORT_H_

extern int serialPortInitialize();
extern void readData(unsigned char* buf,unsigned int bufSize);
extern void writeData(unsigned char const* buf, unsigned int bufSize);
extern void getCommStatus();
extern int getCommStatusRpt(unsigned char *gComm);
extern int  SendComStatus(int n);
extern void* threadDaemon(void *arg);

#endif

