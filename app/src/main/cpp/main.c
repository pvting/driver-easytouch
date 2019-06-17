#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "serialPort.h"
#include "pthread_resolveData.h"
#include "data.h"
//#include "Test_method.h"



#include <android/log.h>
#define   LOG_TAG    "LOG_TEST"
#define   LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

const unsigned char portName[]="/dev/ttyUSB0";
unsigned int baudRate =19200;

pthread_t id_thread,
          id_resolve_data,
		  id_test;

//void ReleaseResource(int signo)
//{
//    pthread_cancel (id_thread);
//    pthread_cancel (id_resolve_data);
//	pthread_cancel (id_test);
//    exit(0);
//}

//The enterance of function
int startProtocol()						//startProtocol
{

    int nRet;
	//Shipment_request_instruct[11] = 100;
    nRet = serialPortInitialize();

    if(0 != nRet)																	//
        return SERIAL_FAIL;

     nRet = pthread_create(&id_thread,0,threadDaemon,NULL);		
	printf("*********pthread_getsful!************%d\n",nRet);		 //
    LOGI("*********pthread_getsful!************%d\n", nRet);
    if(0 == nRet)
    {
        LOGI("*********pthread_getEvent create successful!*************%d\n", nRet);
        printf("*********pthread_getEvent create successful!*************\n");
    }


    nRet = pthread_create(&id_resolve_data,0,pthread_resolveData,NULL);			//����ͨѶ���̣߳������Ľ�������ʹ��
    if(0 == nRet)
        LOGI("*********pthread_resolveData create successful!*************\n");
  //  else
    //    return PTHREAD_FAIL;

	#if 0
	nRet =  pthread_create(&id_test,0,pthread_getevent,NULL);
	if(0 == nRet)
		printf("*********TEST create successful!**********\n");
	else
		return TEST_FAIL;
	#endif

	
  //  pthread_join (id_thread, NULL);
	//printf ("g1\n");
    //pthread_join (id_resolve_data, NULL);
    //printf ("g2\n");
	//pthread_join (id_test, NULL);
    //printf ("g3\n");
	
    /*Ctrl + C create SIGINT signal function*/ 
	//signal (SIGINT, ReleaseResource);
    return 0;
}


#if 0
int main()
{
	startProtocol();
	while(1);
}
#endif
