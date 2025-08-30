#include "git.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态
DHT11_Data_TypeDef DHT11_Data;

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{
	Read_DHT11(&DHT11_Data); // 获取温湿度数据
	Device_Data->temperatuer = DHT11_Data.temp_int + DHT11_Data.temp_deci * 0.01;
	Device_Data->humiditr = DHT11_Data.humi_int + DHT11_Data.humi_deci * 0.01;
	Device_Data->mq2 = mq2_value(); 	//mq2
	Device_Data->mq135 = mq135_value(); //空气质量
	
	return MY_SUCCESSFUL;
}
// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

	// 读
	R_Test();
	// 状态重置
	device_state->check_device = 0;

	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
	char str[50];
	if(LEVEL1){
		sprintf(str, "T:%.1f  H:%.1f  ", Data_init.temperatuer,Data_init.humiditr);
		OLED_ShowCH(4, 0, (unsigned char *)str);
		sprintf(str, "烟雾浓度 : %d  ", Data_init.mq2);
		OLED_ShowCH(0, 2, (unsigned char *)str);
		sprintf(str, "有害气体 : %d  ", Data_init.mq135);
		OLED_ShowCH(0, 4, (unsigned char *)str);
	}

	if(LEVEL1){
		sprintf(str, "是否有明火 : 否 ");
	}else{
		sprintf(str, "是否有明火 : 是 ");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// 更新设备状态
mySta Update_device_massage()
{
	// 满足条件开启 蜂鸣器报警（火灾，烟雾，有害，温度）
	if(LEVEL1 == 0 || Data_init.mq2 > threshold_value_init.mq2_value 
		|| Data_init.mq135 > threshold_value_init.mq135_value
		|| Data_init.temperatuer > (float)threshold_value_init.temp_value)
	{
		Beepout =~ Beepout;
	}else{
		Beepout = 0;
	}
	// 满足条件开启风扇
	if(LEVEL1 == 0 || device_state_init.fan_state || Data_init.mq2 > threshold_value_init.mq2_value 
		|| Data_init.mq135 > threshold_value_init.mq135_value
		|| Data_init.temperatuer > (float)threshold_value_init.temp_value)
	{
		relay2out = 1;
	}else{
		relay2out = 0;
	}
	// 满足条件开启水泵
	if(LEVEL1 == 0){
		relay3out = 1;
	}else{
		relay3out = 0;
	}
	// 满足调节关闭阀门（火灾，烟雾，有害）
	if(LEVEL1 == 0 || Data_init.mq2 > threshold_value_init.mq2_value
		|| Data_init.mq135 > threshold_value_init.mq135_value)
	{
		relay1out = 0;
	}
	// 检测到人体
	if(LEVEL2 == 1){
		device_state_init.poeple_state = 10;
	}

	return MY_SUCCESSFUL;
}

// 定时器
void Automation_Close(void)
{
	// 检测到人体（10倒计时）
	if(device_state_init.poeple_state>0){
		device_state_init.poeple_state --;
	}
	
	//反馈给app/平台
	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			SendMqtt(1); // 发送数据到APP
			break;
		case 2:
			SendData(); // 发送数据到云平台
			break;
		}
		Data_init.App = 0;
	}

}
// 检测按键是否按下
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// 与上一次的键值比较 如果不相等，表明有键值的变化，开始计时
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // 时间记录器
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // 短按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
		  // 按键修改风扇状态 
			if(device_state_init.fan_state){
				device_state_init.fan_state = 0;
			}else{
				device_state_init.fan_state = 1;
			}
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
			// 按键修改燃气阀门状态 
			if(relay1in){
				relay1out = 0;
			}else{
				relay1out = 1;
			}
			break;
		case KEY3_PRES:
			printf("Key3_Short\n");

			break;

		default:
			break;
		}
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // 长按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");


			break;
		case KEY2_PRES:
			printf("Key2_Long\n");

			break;
		case KEY3_PRES:
			printf("Key3_Long\n");

			break;
		default:
			break;
		}
		num_on = 0;
	}
}
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	//cJSON *cjson_data = NULL; // 数据
	const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // 消息包

		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_test, "tmep_v")->valueint;
		threshold_value_init.mq2_value = cJSON_GetObjectItem(cjson_test, "mq2_v")->valueint;
		threshold_value_init.mq135_value = cJSON_GetObjectItem(cjson_test, "mq135_v")->valueint;
		W_Test();
		Data_init.App = 1;
		break;
	case 0x02: // 消息包
		device_state_init.fan_state = cJSON_GetObjectItem(cjson_test, "fan")->valueint;
		Data_init.App = 1;
		break;
	case 0x03: // 消息包
		relay1out = cJSON_GetObjectItem(cjson_test, "relay")->valueint;
		Data_init.App = 1;
		break;

	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
