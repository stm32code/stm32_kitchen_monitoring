package com.example.kitchenmonitoringsystem.utils;

import com.itfitness.mqttlibrary.MQTTHelper;


public class Common {
    //    public static String Port = "6002";
//    public static String Sever = "tcp://183.230.40.39" + ":" + Port;
    public static String Port = "1883";
    public static String Sever = "tcp://iot-06z00axdhgfk24n.mqtt.iothub.aliyuncs.com" + ":" + Port;

    public static String ReceiveTopic = "/broadcast/a1Yxobg0xMK/test2";
    public static String PushTopic = "/broadcast/a1Yxobg0xMK/test1";
    public static String DriveID = "a1Yxobg0xMK.smartapp|securemode=2,signmethod=hmacsha256,timestamp=1713371036734|";
    public static String DriveName = "smartapp&a1Yxobg0xMK";
    public static String DrivePassword = "e59e1830d754a2af4035d1cec6b0a907d53c55174d5913b8f0d56fc615c7275f";
    public static String Drive2ID = "1181073142";
    public static String api_key = "EtpuIH6FIU3XjavzoDRPSdYAkT8=";
    public static boolean DeviceOnline = false;
    public static String LatestOnlineDate = "离线";
    public static MQTTHelper mqttHelper = null;
}
