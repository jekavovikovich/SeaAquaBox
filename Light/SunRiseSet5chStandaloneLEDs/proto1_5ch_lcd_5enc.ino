// TODO:
// добавить логику обработки удержания кнопки энкодера и:
// добавить загрузку дефолтных настроек, подготовить их, возможно предусмотреть выбор и сохранение пресетов
// добавить настройку времени
// добавить регулировку общей яркости

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include <EEPROM.h>
#include "RTClib.h"

#define BRIGHTNESS_ADDR 0  // номер ячейки для хранения массива с данными яркости
#define INIT_ADDR 3333  // номер резервной ячейки для определения первого запуска контроллера после прошивки
#define INIT_KEY 33     // ключ первого запуска. 0-254, на выбор

// ON/OFF switch pin
#define ONOFF A15

// WC encoder pins
#define CLK1 24
#define DT1 22
#define SW1 26

// WW encoder pins
#define CLK2 23
#define DT2 25
#define SW2 27

// BL encoder pins
#define CLK3 30
#define DT3 28
#define SW3 32

// VI encoder pins
#define CLK4 31
#define DT4 29
#define SW4 33

// UV encoder pins
#define CLK5 36
#define DT5 34
#define SW5 38

// color LED pins to MOSFET Gates
#define WCpin 6
#define WWpin 7
#define BLpin 11
#define VIpin 5
#define UVpin 8

// пин пищалки
#define speaker_pin 12

LiquidCrystal_I2C lcd(0x27,20,4);  // LCD 4x20 on i2c bus, autodetect
RTC_DS1307 RTC;
Encoder encWC(CLK1, DT1, SW1);  // WC encoder
Encoder encWW(CLK2, DT2, SW2);  // WW encoder
Encoder encBL(CLK3, DT3, SW3);  // BL encoder
Encoder encVI(CLK4, DT4, SW4);  // VI encoder
Encoder encUV(CLK5, DT5, SW5);  // UV encoder

// начальный режим работы энкодеров в меню, 0 - регулировка яркости, 1 - выбор текущего часа, 2 - альтернативная функция пока не реализована
int encWCmode = 0;
int encWWmode = 0;
int encBLmode = 0;
int encVImode = 0;
int encUVmode = 0;

// переменные для выбранного в меню часа для каждого канала
int WCMenuSelectedHour;
int WWMenuSelectedHour;
int BLMenuSelectedHour;
int VIMenuSelectedHour;
int UVMenuSelectedHour;

// brightness for each channel, add here read from EEPROM
int BrightnessColHr[5][24]
{//0  1  2  3  4  5  6  7   8   9   10  11  12  13  14  15  16  17  18  19  20  21 22 23  часы
  {0, 0, 0, 0, 0, 0, 6, 18, 24, 39, 51, 69, 99, 99, 90, 81, 72, 69, 51, 39, 18, 3, 0, 0 },  // COLD WHITE, percents(%) per each hour from 0 to 99
  {0, 0, 0, 0, 0, 0, 3, 6, 12, 21, 30, 45, 72, 72, 63, 60, 51, 42, 27, 15, 6, 0, 0, 0 },  // WARM WHITE
  {0, 0, 0, 0, 3, 6, 9, 21, 33, 51, 72, 81, 90, 99, 99, 90, 81, 72, 60, 39, 30, 9, 3, 0 },  // BLUE
  {0, 0, 0, 0, 0, 3, 9, 33, 60, 63, 63, 51, 39, 36, 33, 30, 30, 30, 42, 48, 30, 9, 0, 0 },  // VIOLET
  {0, 0, 0, 0, 0, 0, 9, 15, 30, 30, 33, 33, 48, 51, 51, 63, 60, 33, 30, 42, 39, 12, 0, 0 },  // Ultra-Violet
};

int CurHour; // текущий час выбранный в меню, на старте приравнивается к прочитанному из RTC значению
uint32_t SaveTimer = 0;  // переменная таймера записи в eprom
boolean SaveFlag = false;  // флаг необходимости записи в eprom
uint32_t RTCTimer;  // переменная таймера опроса RTC
DateTime now; // переменная для чтения времени из RTC
boolean OFFflag = false;  // флаг активации кнопки отключения
int MaxBrightness = 255;  // максимальная яркость, задает предел заполнения ШИМ

#include "encoderWC.h"
#include "encoderWW.h"
#include "encoderBL.h"
#include "encoderVI.h"
#include "encoderUV.h"


void setup() {
    pinMode(LED_BUILTIN, OUTPUT); // инициализируем пин лампочки на борде, наверно можно отключить
    pinMode(speaker_pin, OUTPUT); // инициализируем пин пищалки
    pinMode(ONOFF, INPUT_PULLUP); // инициализируем пин выключателя с подтяжкой вверх тк при активации он будет замыкать на минус
    RTCTimer = millis();
    RTC.begin();
    now = RTC.now();
    //RTC.adjust(DateTime(__DATE__, __TIME__));  // настройка времени
    CurHour = now.hour();
    WCMenuSelectedHour = CurHour;
    WWMenuSelectedHour = CurHour;
    BLMenuSelectedHour = CurHour;
    VIMenuSelectedHour = CurHour;
    UVMenuSelectedHour = CurHour;

    Wire.begin();
    lcd.init();                       //  Инициируем работу с LCD дисплеем
    lcd.backlight();                  //  Включаем подсветку LCD дисплея

    if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск, по умолчанию память заполнена значениями 255
      EEPROM.write(INIT_ADDR, INIT_KEY);    // записали уникальный ключ
      EEPROM.put(BRIGHTNESS_ADDR, BrightnessColHr); // и сохранили начальные значения массива
    }
    EEPROM.get(BRIGHTNESS_ADDR, BrightnessColHr); // загрузка массива из eprom

// авто режим опроса энкодеров
    encWC.setTickMode(AUTO);
    encWW.setTickMode(AUTO);
    encBL.setTickMode(AUTO);
    encVI.setTickMode(AUTO);
    encUV.setTickMode(AUTO);

    lcd.setCursor(0, 0);
    lcd.print(" WC  WW  BL  VI  UV "); // выводим имена каналов по цветам
    
    OutBrHrToLCD(); // в процедуре выводим на экран текущий час и текущую яркость для всех каналов

// выводим указатели начального режима работы энкодеров в меню, на старте тоже одинаковы для всех и установлены в регулировку яркости
    lcd.setCursor(0, 2); lcd.print(">");                     // WC
    lcd.setCursor(4, 2); lcd.print(">");                     // WW
    lcd.setCursor(8, 2); lcd.print(">");                     // BL
    lcd.setCursor(12, 2); lcd.print(">");                    // VI
    lcd.setCursor(16, 2); lcd.print(">");                    // UV

// инициализируем color LED pins
    pinMode(WCpin, OUTPUT);
    pinMode(WWpin, OUTPUT);
    pinMode(BLpin, OUTPUT);
    pinMode(VIpin, OUTPUT);
    pinMode(UVpin, OUTPUT);

    if (digitalRead(ONOFF) == HIGH) {OFFflag = true;} else {OFFflag = false;} // опрашиваем кнопку выключения при запуске и выставляем флаг
    SetBrightness(); // в процедуре выставляем яркость в зависисмости от статуса кнопки выключения
    //Serial.begin(115200);
}





void loop() {

if (digitalRead(ONOFF) == HIGH) {OFFflag = true;} else {OFFflag = false;} // опрашиваем кнопку выключения один раз в цикле и выставляем флаг

if (SaveFlag && (millis() - SaveTimer >= 15000)) {   // если флаг поднят и с последнего нажатия прошло 15 секунд (15 000 мс), работает до 30с
  SaveFlag = false;                                  // опустили флаг
  RTCTimer = millis();                               // сбросили соседний таймер чтоб он не сработал мнгновенно например после долгого редактирования настроек
  EEPROM.put(BRIGHTNESS_ADDR, BrightnessColHr);      // записали в EEPROM, раскоментить перед продакшеном
  lcd.setCursor(0, 3); lcd.print("SETTINGS SAVED!    "); // уведомляем о сохранении настроек в строке статуса
  tone(speaker_pin, 1200, 50); //conflicts with port 10, dont use this port along with tone function
}

if ((SaveFlag == false) && (millis() - RTCTimer >= 30000)) {  // если НЕ установлен флаг записи в eprom И с последнего сабатывания прошло 30 секунд (30 000 мс), работает до 30с
  RTCTimer = millis();  // сбросили таймер
  EEPROM.get(BRIGHTNESS_ADDR, BrightnessColHr); // загрузка массива из eprom
  now = RTC.now();
  CurHour = now.hour(); // получаем текущий час
  WCMenuSelectedHour = CurHour;  // устанавливаем значение текущего часа для всех каналов
  WWMenuSelectedHour = CurHour;
  BLMenuSelectedHour = CurHour;
  VIMenuSelectedHour = CurHour;
  UVMenuSelectedHour = CurHour;
  encWCmode = 0; // устанавливаем по умолчанию режим регулировки яркости
  encWWmode = 0;
  encBLmode = 0;
  encVImode = 0;
  encUVmode = 0;
  lcd.setCursor(0, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
  lcd.setCursor(4, 1); lcd.print(" ");
  lcd.setCursor(8, 1); lcd.print(" ");
  lcd.setCursor(12, 1); lcd.print(" ");
  lcd.setCursor(16, 1); lcd.print(" ");
  lcd.setCursor(0, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
  lcd.setCursor(4, 2); lcd.print(">");                     // WW
  lcd.setCursor(8, 2); lcd.print(">");                     // BL
  lcd.setCursor(12, 2); lcd.print(">");                    // VI
  lcd.setCursor(16, 2); lcd.print(">");                    // UV

  OutBrHrToLCD(); // в процедуре выводим на экран текущий час и текущую яркость для всех каналов
  SetBrightness(); // в процедуре выставляем яркость в зависисмости от статуса кнопки выключения
}

///  в процедурах обрабатываем повороты и нажатия энкодеров
DoEncoderWC();
DoEncoderWW();
DoEncoderBL();
DoEncoderVI();
DoEncoderUV();

}



void OutBrHrToLCD() {
  // выводим текущий час для каждого пина, он ессно будет одинаков на старте
    lcd.setCursor(1, 1); lcd.print(CurHour); lcd.print(" ");                     // WC
    lcd.setCursor(5, 1); lcd.print(CurHour); lcd.print(" ");                     // WW
    lcd.setCursor(9, 1); lcd.print(CurHour); lcd.print(" ");                     // BL
    lcd.setCursor(13, 1); lcd.print(CurHour); lcd.print(" ");                    // VI
    lcd.setCursor(17, 1); lcd.print(CurHour); lcd.print(" ");                    // UV
  // выводим текущую яркость для текущего часа
    lcd.setCursor(1, 2); lcd.print(BrightnessColHr[0][CurHour]); lcd.print(" ");   // WC
    lcd.setCursor(5, 2); lcd.print(BrightnessColHr[1][CurHour]); lcd.print(" ");   // WW
    lcd.setCursor(9, 2); lcd.print(BrightnessColHr[2][CurHour]); lcd.print(" ");   // BL
    lcd.setCursor(13, 2); lcd.print(BrightnessColHr[3][CurHour]); lcd.print(" ");  // VI
    lcd.setCursor(17, 2); lcd.print(BrightnessColHr[4][CurHour]); lcd.print(" ");  // UV
}




void SetBrightness() {
int NextHour; // переменная для вычисления следующего часа и плавного перехода поминутно

if ((CurHour + 1) > 23) { NextHour = 0; } else { NextHour = CurHour + 1; }  // вычисляем следующий час
int8_t BrightnessToSetWC = map(now.minute(), 0, 59, BrightnessColHr[0][CurHour], BrightnessColHr[0][NextHour]); // вычисляем процент яркости между часами
int8_t BrightnessToSetWW = map(now.minute(), 0, 59, BrightnessColHr[1][CurHour], BrightnessColHr[1][NextHour]);
int8_t BrightnessToSetBL = map(now.minute(), 0, 59, BrightnessColHr[2][CurHour], BrightnessColHr[2][NextHour]);
int8_t BrightnessToSetVI = map(now.minute(), 0, 59, BrightnessColHr[3][CurHour], BrightnessColHr[3][NextHour]);
int8_t BrightnessToSetUV = map(now.minute(), 0, 59, BrightnessColHr[4][CurHour], BrightnessColHr[4][NextHour]);

if (OFFflag == true) { //если поднят флаг активации кнопки отключения
// устанавливаем значение яркости в 0
    analogWrite(WCpin, 0);
    analogWrite(WWpin, 0);
    analogWrite(BLpin, 0);
    analogWrite(VIpin, 0);
    analogWrite(UVpin, 0);
// выводим в строке статуса сообщение что все отключено
    lcd.setCursor(0, 3); lcd.print("OFF SWITCH ACIVATED");
}
else {  // флаг кнопки включения опущен, обычный режим работы, все включено
// устанавливаем значение яркости из массива BrightnessColHr соответственно CurHour, яркость меняется раз в час
    //analogWrite(WCpin, map(BrightnessColHr[0][CurHour], 0, 99, 0, MaxBrightness));
    //analogWrite(WWpin, map(BrightnessColHr[1][CurHour], 0, 99, 0, MaxBrightness));
    //analogWrite(BLpin, map(BrightnessColHr[2][CurHour], 0, 99, 0, MaxBrightness));
    //analogWrite(VIpin, map(BrightnessColHr[3][CurHour], 0, 99, 0, MaxBrightness));
    //analogWrite(UVpin, map(BrightnessColHr[4][CurHour], 0, 99, 0, MaxBrightness));
// устанавливаем значение яркости по значению между часами, плавный переход
    analogWrite(WCpin, map(BrightnessToSetWC, 0, 99, 0, MaxBrightness));
    analogWrite(WWpin, map(BrightnessToSetWW, 0, 99, 0, MaxBrightness));
    analogWrite(BLpin, map(BrightnessToSetBL, 0, 99, 0, MaxBrightness));
    analogWrite(VIpin, map(BrightnessToSetVI, 0, 99, 0, MaxBrightness));
    analogWrite(UVpin, map(BrightnessToSetUV, 0, 99, 0, MaxBrightness));

// выводим текущее время в строке статуса
    lcd.setCursor(0, 3); lcd.print("TIME  ");
    lcd.setCursor(6, 3);
    if (now.hour() < 10) { lcd.print("0"); lcd.print(now.hour()); } else { lcd.print(now.hour()); }
    lcd.print(":");
    if (now.minute() < 10) { lcd.print("0"); lcd.print(now.minute()); } else { lcd.print(now.minute()); }
    lcd.print("        ");
}
}


