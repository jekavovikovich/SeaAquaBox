// encoder WC
void DoEncoderWC() {

if (encWC.isClick()) {  // нажата кнопка экодера - меняем его текущий режим - выбор яркости или часа
SaveTimer = millis();                   // сбросить таймер записи в eprom
encWCmode = ++encWCmode;
if (encWCmode > 1) encWCmode = 0;  // modes till 0 to 1, коротким нажатием выбираем только между яркостью и часом

if (encWCmode == 0) { // выбран режим регулировки яркости
lcd.setCursor(0, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(0, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

if (encWCmode == 1) { // выбран режим выбора часа
lcd.setCursor(0, 2); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(0, 1); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

tone(speaker_pin, 800, 30); //conflicts with port 10, dont use this port along with tone function
}


if (encWC.isHolded()) {  // кнопка экодера нажата с удержанием, энкодер переходит в альтернативный режим 2
encWCmode = 2;
SaveTimer = millis();                   // сбросить таймер записи в eprom
}


if (encWC.isRight()) {  // энкодер повернут вправо

if (encWCmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[0][WCMenuSelectedHour] = BrightnessColHr[0][WCMenuSelectedHour] + 3; // увеличиваем яркость
if (BrightnessColHr[0][WCMenuSelectedHour] > 99) BrightnessColHr[0][WCMenuSelectedHour] = 99; // ограничиваем макс.значение до 99 иначе не влезет в экран
if ((WCMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(WCpin, map(BrightnessColHr[0][WCMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываяем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encWCmode == 1) { // и выбран режим выбора часа
WCMenuSelectedHour = ++WCMenuSelectedHour;  // увеличиваем выбранный час
if (WCMenuSelectedHour > 23) WCMenuSelectedHour = 23; // ограничиваем макс.значение до 23ч
}
}



if (encWC.isLeft()) {  // энкодер повернут влево

if (encWCmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[0][WCMenuSelectedHour] = BrightnessColHr[0][WCMenuSelectedHour] - 3;  // уменьшаем яркость
if (BrightnessColHr[0][WCMenuSelectedHour] < 0) BrightnessColHr[0][WCMenuSelectedHour] = 0;  // ограничиваем мин.значение не ниже 0
if ((WCMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(WCpin, map(BrightnessColHr[0][WCMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encWCmode == 1) { // и выбран режим выбора часа
WCMenuSelectedHour = WCMenuSelectedHour - 1;  // уменьшаем выбранный час
if (WCMenuSelectedHour < 0) WCMenuSelectedHour = 0; // ограничиваем мин.значение до 0ч
}
}


if (encWC.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
SaveTimer = millis();                   // сбросить таймер записи в eprom
lcd.setCursor(1, 1); lcd.print(WCMenuSelectedHour); lcd.print(" ");  // выводим на экран выбранный час
lcd.setCursor(1, 2); lcd.print(BrightnessColHr[0][WCMenuSelectedHour]); lcd.print(" ");  // выводим на экран текущее значение яркости для выбранного часа
tone(speaker_pin, 1000, 20); //conflicts with port 10, dont use this port along with tone function
}
}
