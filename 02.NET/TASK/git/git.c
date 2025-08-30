#include "git.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬
DHT11_Data_TypeDef DHT11_Data;

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	Read_DHT11(&DHT11_Data); // ��ȡ��ʪ������
	Device_Data->temperatuer = DHT11_Data.temp_int + DHT11_Data.temp_deci * 0.01;
	Device_Data->humiditr = DHT11_Data.humi_int + DHT11_Data.humi_deci * 0.01;
	Device_Data->mq2 = mq2_value(); 	//mq2
	Device_Data->mq135 = mq135_value(); //��������
	
	return MY_SUCCESSFUL;
}
// ��ʼ��
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

	// ��
	R_Test();
	// ״̬����
	device_state->check_device = 0;

	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];
	if(LEVEL1){
		sprintf(str, "T:%.1f  H:%.1f  ", Data_init.temperatuer,Data_init.humiditr);
		OLED_ShowCH(4, 0, (unsigned char *)str);
		sprintf(str, "����Ũ�� : %d  ", Data_init.mq2);
		OLED_ShowCH(0, 2, (unsigned char *)str);
		sprintf(str, "�к����� : %d  ", Data_init.mq135);
		OLED_ShowCH(0, 4, (unsigned char *)str);
	}

	if(LEVEL1){
		sprintf(str, "�Ƿ������� : �� ");
	}else{
		sprintf(str, "�Ƿ������� : �� ");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// �����豸״̬
mySta Update_device_massage()
{
	// ������������ ���������������֣������к����¶ȣ�
	if(LEVEL1 == 0 || Data_init.mq2 > threshold_value_init.mq2_value 
		|| Data_init.mq135 > threshold_value_init.mq135_value
		|| Data_init.temperatuer > (float)threshold_value_init.temp_value)
	{
		Beepout =~ Beepout;
	}else{
		Beepout = 0;
	}
	// ����������������
	if(LEVEL1 == 0 || device_state_init.fan_state || Data_init.mq2 > threshold_value_init.mq2_value 
		|| Data_init.mq135 > threshold_value_init.mq135_value
		|| Data_init.temperatuer > (float)threshold_value_init.temp_value)
	{
		relay2out = 1;
	}else{
		relay2out = 0;
	}
	// ������������ˮ��
	if(LEVEL1 == 0){
		relay3out = 1;
	}else{
		relay3out = 0;
	}
	// ������ڹرշ��ţ����֣������к���
	if(LEVEL1 == 0 || Data_init.mq2 > threshold_value_init.mq2_value
		|| Data_init.mq135 > threshold_value_init.mq135_value)
	{
		relay1out = 0;
	}
	// ��⵽����
	if(LEVEL2 == 1){
		device_state_init.poeple_state = 10;
	}

	return MY_SUCCESSFUL;
}

// ��ʱ��
void Automation_Close(void)
{
	// ��⵽���壨10����ʱ��
	if(device_state_init.poeple_state>0){
		device_state_init.poeple_state --;
	}
	
	//������app/ƽ̨
	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			SendMqtt(1); // �������ݵ�APP
			break;
		case 2:
			SendData(); // �������ݵ���ƽ̨
			break;
		}
		Data_init.App = 0;
	}

}
// ��ⰴ���Ƿ���
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// ����һ�εļ�ֵ�Ƚ� �������ȣ������м�ֵ�ı仯����ʼ��ʱ
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // ʱ���¼��
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // �̰�
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
		  // �����޸ķ���״̬ 
			if(device_state_init.fan_state){
				device_state_init.fan_state = 0;
			}else{
				device_state_init.fan_state = 1;
			}
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
			// �����޸�ȼ������״̬ 
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
	else if (key == 0 && num_on >= Key_Scan_Time) // ����
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
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	//cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��

		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_test, "tmep_v")->valueint;
		threshold_value_init.mq2_value = cJSON_GetObjectItem(cjson_test, "mq2_v")->valueint;
		threshold_value_init.mq135_value = cJSON_GetObjectItem(cjson_test, "mq135_v")->valueint;
		W_Test();
		Data_init.App = 1;
		break;
	case 0x02: // ��Ϣ��
		device_state_init.fan_state = cJSON_GetObjectItem(cjson_test, "fan")->valueint;
		Data_init.App = 1;
		break;
	case 0x03: // ��Ϣ��
		relay1out = cJSON_GetObjectItem(cjson_test, "relay")->valueint;
		Data_init.App = 1;
		break;

	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
