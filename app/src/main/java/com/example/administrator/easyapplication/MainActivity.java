package com.example.administrator.easyapplication;

import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;

import java.io.DataOutputStream;
import java.io.File;
import java.util.ArrayList;

import static android.R.attr.type;

public class MainActivity extends AppCompatActivity {
    int column;
    char contain_num;
    char cSetChannelAddress;
    char cPayType;
    char payMent;
    int setPrise;
    int nSetNumber;
    int nSetPrice;
    int nRefundPrice;
    char nSetMachine;                  //机器柜号
    byte[] nSetTime;
    EditText et;
    EditText et1;
    EditText et2;
    TextView tv;

    static {
        System.loadLibrary("easy_touch");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        et = (EditText) findViewById(R.id.editText1);
        et.setOnFocusChangeListener(new View.OnFocusChangeListener() {

            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    et.setText("");
                } else {
                    et.setText("请输入货道编号：");
                }
            }
        });

        et1 = (EditText) findViewById(R.id.editText2);
        et1.setOnFocusChangeListener(new View.OnFocusChangeListener() {

            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    et.setText("");
                } else {
                    et.setText("请输入货道价格：");
                }
            }
        });

        et2 = (EditText) findViewById(R.id.editText3);
        et2.setOnFocusChangeListener(new View.OnFocusChangeListener() {

            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    et.setText("");
                } else {
                    et.setText("请输入设置时间：");
                }
            }
        });

        tv = (TextView) findViewById(R.id.textview);
        tv.setText("易触售货机");
        tv.append("\n");

        Thread thread_protocol = new Thread(new Runnable() {
            @Override
            public void run() {
                Log.e("tag:", "tag:");
                startProtocol();
            }
        });

        thread_protocol.start();

        final Handler mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 0xA1:
                        tv.append("签到事件:" + "机器码:"+bytes2HexString(getSigninMessage()) );
                        tv.append("\n");
                        break;
                    case 0xA2:
                        tv.append("POLL轮询事件:" + bytes2HexString(getPollMessage()));
                        Log.e("tagg:",bytes2HexString(getPollMessage()));
                        tv.append("\n");
                        break;
                    case 0xA3:
                        tv.append("出货报告事件:" + bytes2HexString(getVenderOutReport()));
                        Log.e("tagg:",bytes2HexString(getVenderOutReport()));
                        tv.append("\n");
                        break;
                    case 0xA4:
                        nSetMachine = 11;
                        tv.append("料道运行事件（表示饮料柜11）:" + bytes2HexString(getChannelRunMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelRunMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xA5:
                        tv.append("机器运行事件"+ bytes2HexString(getMachineRunMessage()));
                        Log.e("tagg:",bytes2HexString(getMachineRunMessage()));
                        tv.append("\n");
                        break;
                    case 0xA6:
                        nSetMachine = 11;
                        tv.append("系统配置事件（表示饮料柜11）:" + bytes2HexString(getSystemSetMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getSystemSetMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xA7:
                        nSetMachine = 11;
                        tv.append("料道商品信息事件（表示饮料柜11）:" + bytes2HexString(getChannelGoodsMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelGoodsMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xA8:
                        nSetMachine = 11;
                        tv.append("料道价格信息事件（表示饮料柜11）:" + bytes2HexString(getChannelPriceMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelPriceMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xC4:
                        nSetMachine = 9;
                        tv.append("料道运行事件2（表示食品柜09）:" + bytes2HexString(getChannelRunMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelRunMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xC6:
                        nSetMachine = 9;
                        tv.append("系统配置事件2（表示饮料柜09）:" + bytes2HexString(getSystemSetMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getSystemSetMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xC7:
                        nSetMachine = 9;
                        tv.append("料道商品信息事件2（表示食品柜09）:" + bytes2HexString(getChannelGoodsMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelGoodsMessage(nSetMachine)));
                        tv.append("\n");
                        break;
                    case 0xC8:
                        nSetMachine = 9;
                        tv.append("料道价格信息事件2（表示食品柜09）:" + bytes2HexString(getChannelPriceMessage(nSetMachine)));
                        Log.e("tagg:",bytes2HexString(getChannelPriceMessage(nSetMachine)));
                        tv.append("\n");
                        break;

                    case 0xB1:
                        tv.append("开门事件:" + bytes2HexString(getMachineRunMessage()));
                        Log.e("tagg:",bytes2HexString(getMachineRunMessage()));
                        tv.append("\n");
                        break;
                    case 0xB2:
                        tv.append("关门事件:" + bytes2HexString(getMachineRunMessage()));
                        Log.e("tagg:",bytes2HexString(getMachineRunMessage()));
                        tv.append("\n");
                        break;
                    case 0xB5:
                        tv.append("收到现金变化事件:" + bytes2HexString(getPollMessage()));
                        Log.e("tagg:",bytes2HexString(getPollMessage()));
                        tv.append("\n");
                        break;
                    case 0xB6:
                        tv.append("收到选取料道事件:" + bytes2HexString(getPollMessage()));
                        Log.e("tagg:",bytes2HexString(getPollMessage()));
                        tv.append("\n");
                        break;
                    case 0xB7:
                        tv.append("设定失败事件:");
                        tv.append("\n");
                        break;
                    case 0xC1:
                        tv.append("串口连接状态事件:" );
                        tv.append("\n");
                        break;
                    default :
                        break;
                }
            }
        };


        Thread thread_event = new Thread(new Runnable() {

            @Override
            public void run() {
                while (true) {
                    if (!(true)) break;
                    int i;
                    i = getEvent();
                    Log.d("LOG_TEST",""+i);
                    mHandler.sendEmptyMessage(i);
                }
            }
        });
        thread_event.start();
    }
    public void tuibi(View view){
        setReturnCoin();
    }

    public static String bytes2HexString(byte[] b) {        //byte转16进制字符串函数
        String ret = "";
        for (int i = 0; i < b.length; i++) {
            String hex = Integer.toHexString(b[i] & 0xFF);
            if (hex.length() == 1) {
                hex = '0' + hex;
            }
            ret += hex.toUpperCase();
        }
        return ret;
    }


    public void VenderoutActionClicked(View view) {
        contain_num = 11;
        cSetChannelAddress = 0x01;
        cPayType = 0x01;
        setProductsale(contain_num,cSetChannelAddress,cPayType);
        Log.e("tag:","出货");
    }

    public void SetPriceButtonClicked(View view) {
        contain_num = 11;
        cSetChannelAddress = 0x01;
        setPrise = 500;
        setChannelprice(contain_num,cSetChannelAddress,setPrise);
        Log.e("tag:","设置货道价格");
    }

    public void GetSigninClicked(View view) {
        getSigninMessage();
        tv.append("获取签到机器信息:" + bytes2HexString(getSigninMessage()));
        Log.e("tag:","获取机器签到信息");
    }

    public void GetproductsaleClicked(View view) {
        getVenderOutReport();
        tv.append("获取出货报告信息:" + bytes2HexString(getVenderOutReport()));
        Log.e("tag:","获取机器出货报告信息");
    }

    public void SetSystemTimeClicked(View view) {
        nSetTime = new byte[]{0x20, 0x17, 0x05, 0x31, 0x17, 0x44, 0x30};
        setSystemtime(nSetTime);
        Log.e("tag:","设置机器时间");
    }

    public void GetMachinerunClicked(View view) {
        getMachineRunMessage();
        tv.append("请求货道运行信息:" + bytes2HexString(getMachineRunMessage()));
        Log.e("tag:","获取货道运行信息");
    }

    public void GetChannelrunClicked(View view) {
        contain_num = 11;
        getChannelRunMessage(contain_num);
        tv.append("请求货道运行信息:" + bytes2HexString(getChannelRunMessage(contain_num)));
        Log.e("tag:","获取货道运行信息");
    }

    public void GetChannelErrorClicked(View view) {
        contain_num = 11;
        getChannelGoodsMessage(contain_num);
        tv.append("请求料道商品信息:" + bytes2HexString(getChannelGoodsMessage(contain_num)));
        Log.e("tag:","请求料道商品信息");
    }

    public void GetChannelPriceClicked(View view) {
        contain_num = 11;
        getChannelPriceMessage(contain_num);
        tv.append("请求料道价格信息:" + bytes2HexString(getChannelPriceMessage(contain_num)));
        Log.e("tag:","请求料道价格信息");
    }

    public void GetMachineStatusClicked(View view) {
        contain_num = 11;
        getSystemSetMessage(contain_num);
        tv.append("请求系统设置信息:" + bytes2HexString(getSystemSetMessage(contain_num)));
        Log.e("tag:","请求系统设置信息");
    }

    public void GetMachinePollClicked(View view) {
        getPollMessage();
        tv.append("请求POLL信息:" + bytes2HexString(getPollMessage()));
        Log.e("tag:","请求POLL信息");
    }

    public native int getEvent();

    public native int startProtocol();

    public native int setSystemtime(byte[] szVMCtime);

    public native int setProductsale(char cContainnum,char cChannelnum,char sale_way);

    public native int setChannelprice(char cContainnum,char cChannelnum,int cChannelprice);

    public native int setSystemstate(byte[] szVMCstate);

    public native int setMachinerun(char on_off );

    public native byte[] getSigninMessage();

    public native byte[] getPollMessage();

    public native byte[] getVenderOutReport();

    public native byte[] getChannelRunMessage(char cContainnum);

    public native byte[] getMachineRunMessage();

    public native byte[] getSystemSetMessage(char cContainnum);

    public native byte[] getChannelGoodsMessage(char cContainnum);

    public native byte[] getChannelPriceMessage(char cContainnum);

    public native int setReturnCoin();

}
