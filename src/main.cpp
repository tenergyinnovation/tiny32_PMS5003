/***********************************************************************
 * Project      :     tiny32_PMS5003
 * Description  :     this code is template source code Interface test
 *                    between tiny32 and PM2.5_PMS5003
 * Hardware     :     tiny32_v4
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     21/10/2024
 * Revision     :     1.0
 * Rev1.0       :     Origital
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     uten.boonliam@tenergyinnovation.co.th
 * TEL          :     +66 89-140-7205
 ***********************************************************************/
#include <Arduino.h>
#include <tiny32_v3.h>
#include <PMS.h>
#include <esp_task_wdt.h>
#include <stdio.h>
#include <stdint.h>

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.0";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     tiny32_PMS5003\r\n");
    Serial.printf("* Description  :     this code is template source code Interface test  \r\n");
    Serial.printf("*                    between tiny32 and PM2.5_PMS5003 \r\n");
    Serial.printf("* Hardware     :     tiny32_v4\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     19/06/2023\r\n");
    Serial.printf("* Revision     :     %s\r\n", version);
    Serial.printf("* Rev1.0       :     Origital\r\n");
    Serial.printf("* website      :     http://www.tenergyinnovation.co.th\r\n");
    Serial.printf("* Email        :     uten.boonliam@tenergyinnovation.co.th\r\n");
    Serial.printf("* TEL          :     +66 89-140-7205\r\n");
    Serial.printf("***********************************************************************/\r\n");
}

/**************************************/
/*        define object variable      */
/**************************************/
tiny32_v3 mcu;
HardwareSerial Uart2(2);
PMS pms(Uart2);
PMS::DATA data;

/**************************************/
/*   MultiTasking function define     */
/**************************************/
void PMSensor_Task(void *p); // rask for read sensor

/**************************************/
/*            GPIO define             */
/**************************************/

/**************************************/
/*       Constand define value        */
/**************************************/
#define WDT_TIMEOUT 10

/**************************************/
/*       eeprom address define        */
/**************************************/

/**************************************/
/*        define global variable      */
/**************************************/
float value_1 = 0;             // เก็บค่า PM1.0
float value_2 = 0;             // เก็บค่า PM2.5
float value_3 = 0;             // เก็บค่า PM10.0
volatile bool sensor_flag = 0; // ตัวแปรบอกสถานะเซนเซอร์ fail = 0,  pass = 1

/**************************************/
/*           define function          */
/**************************************/

/***********************************************************************
 * FUNCTION:    PMSensor_Task
 * DESCRIPTION: Multitasking Read PM sensor
 *              โดย task นี้จะวนอ่านเซนเซอร์ซ้ำเยอะมากจนกว่า จนอ่านได้ โดยตัวฟังก์ชั่นไม่สามารถ
 *              คืนค่าครั้งแรกเป็น true ได้*
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void PMSensor_Task(void *p)
{

    while (1)
    {

        if (pms.read(data))
        {
            Serial.print("PM 1.0 (ug/m3): ");
            Serial.println(data.PM_AE_UG_1_0);
            value_1 = (float)data.PM_AE_UG_1_0;

            Serial.print("PM 2.5 (ug/m3): ");
            Serial.println(data.PM_AE_UG_2_5);
            value_2 = (float)data.PM_AE_UG_2_5;

            Serial.print("PM 10.0 (ug/m3): ");
            Serial.println(data.PM_AE_UG_10_0);
            value_3 = (float)data.PM_SP_UG_10_0;
            Serial.println();
            sensor_flag = 1; // sensor active
        }

        vTaskDelay(1);
    }
}
/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup()
{
    Serial.begin(115200);
    Uart2.begin(9600, SERIAL_8N1, TXD3, RXD3);
    header_print();
    Serial.print("Configuring WDT...");
    esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
    esp_task_wdt_add(NULL);
    Serial.println("done");

    Serial.print("Info: Multitasking initial ...");
    xTaskCreate(&PMSensor_Task, "PMSensor_Task", 2048, NULL, 10, NULL);
    Serial.println("done");
    mcu.buzzer_beep(2);
    mcu.TickBuildinLED(1);
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    esp_task_wdt_reset();
    vTaskDelay(500);
}
