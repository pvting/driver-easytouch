#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" {
#include "pthread_resolveData.h"
#include "serialPort.h"
}


#define   LOG_TAG    "LOG_TEST"
#define   LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setReturnCoin(JNIEnv *env,
                                                                          jobject instance) {

    jint nRet;
    nRet = setReturnCoin();
    return nRet;

}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getChannelPriceMessage(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jchar cContainnum) {

    jbyte gR[97];
    jint len = getChannelPriceMessage((unsigned char *)gR,(unsigned char)cContainnum);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getChannelGoodsMessage(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jchar cContainnum) {

    jbyte gR[49];
    jint len = getChannelGoodsMessage((unsigned char *)gR,(unsigned char)cContainnum);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getSystemSetMessage(JNIEnv *env,
                                                                                jobject instance,
                                                                                jchar cContainnum) {

    jbyte gR[17];
    jint len = getSystemSetMessage((unsigned char *)gR,(unsigned char)cContainnum);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getMachineRunMessage(JNIEnv *env,
                                                                                 jobject instance) {

    jbyte gR[10];
    jint len = getMachineRunMessage((unsigned char *)gR);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getChannelRunMessage(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jchar cContainnum) {

    jbyte gR[55];
    jint len = getChannelRunMessage((unsigned char *)gR,(unsigned char)cContainnum);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getVenderOutReport(JNIEnv *env,
                                                                               jobject instance) {

    jbyte gR[29];
    jint len = getVenderOutReport((unsigned char *)gR);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getPollMessage(JNIEnv *env,
                                                                           jobject instance) {

    jbyte gR[11];
    jint len = getPollMessage((unsigned char *)gR);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getSigninMessage(JNIEnv *env,
                                                                             jobject instance) {
    jbyte gR[20];
    jint len = getSigninMessage((unsigned char *)gR);
    jbyteArray  ret =(*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret,0,len,gR);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setMachinerun(JNIEnv *env,
                                                                          jobject instance,
                                                                          jchar on_off) {
    jint nRet;
    nRet = setMachinerun((unsigned char) on_off);
    return nRet;
    // TODO

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setSystemstate(JNIEnv *env,
                                                                           jobject instance,
                                                                           jbyteArray szVMCstate_) {
    jint nRet;
    jbyte *szVMCstate = env->GetByteArrayElements(szVMCstate_, NULL);
    nRet = setSystemstate((unsigned char*) szVMCstate_);
    env->ReleaseByteArrayElements(szVMCstate_, szVMCstate, 0);
    return nRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setChannelprice(JNIEnv *env,
                                                                            jobject instance,
                                                                            jchar cContainnum,
                                                                            jchar cChannelnum,
                                                                            jint cChannelprice) {
    jint nRet;
    nRet = setChannelprice((unsigned char) cContainnum,(unsigned char) cChannelnum,(int) cChannelprice);
    return nRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setProductsale(JNIEnv *env,
                                                                           jobject instance,
                                                                           jchar cContainnum,
                                                                           jchar cChannelnum,
                                                                           jchar sale_way) {
    jint nRet;
    nRet = setProductsale((unsigned char) cContainnum,(unsigned char) cChannelnum,(unsigned char) sale_way);
    return nRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_setSystemtime(JNIEnv *env,
                                                                          jobject instance,
                                                                          jbyteArray szVMCtime_) {
    jint nRet;
    jbyte *szVMCtime = env->GetByteArrayElements(szVMCtime_, NULL);
    nRet = setSystemtime((unsigned char*) szVMCtime);
    // TODO
    env->ReleaseByteArrayElements(szVMCtime_, szVMCtime, 0);
    return nRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_startProtocol(JNIEnv *env,
                                                                          jobject instance) {
    jint nRet;
    nRet = startProtocol();
    return  nRet;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_administrator_easyapplication_MainActivity_getEvent(JNIEnv *env,
                                                                     jobject instance) {
    jint nRet;
    LOGI("get Event\n");
    nRet = getEvent();
    return nRet;// TODO
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_administrator_easyapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
