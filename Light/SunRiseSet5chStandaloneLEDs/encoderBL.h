// encoder BL
void DoEncoderBL() {

if (encBL.isClick()) {  // нажата кнопка экодера - меняем его текущий режим - выбор яркости или часа
SaveTimer = millis();                   // сбросить таймер записи в eprom
encBLmode = ++encBLmode;
if (encBLmode > 1) encBLmode = 0;  // modes till 0 to 1, коротким нажатием выбираем только между яркостью и часом

if (encBLmode == 0) { // выбран режим регулировки яркости
lcd.setCursor(8, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(8, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

if (encBLmode == 1) { // выбран режим выбора часа
lcd.setCursor(8, 2); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(8, 1); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

tone(speaker_pin, 800, 30); //conflicts with port 10, dont use this port along with tone function
}


if (encBL.isHolded()) {  // кнопка экодера нажата с удержанием, энкодер переходит в альтернативный режим 2
encBLmode = 2;
SaveTimer = millis();                   // сбросить таймер записи в eprom
}


if (encBL.isRight()) {  // энкодер повернут вправо

if (encBLmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[2][BLMenuSelectedHour] = BrightnessColHr[2][BLMenuSelectedHour] + 3; // увеличиваем яркость
if (BrightnessColHr[2][BLMenuSelectedHour] > 99) BrightnessColHr[2][BLMenuSelectedHour] = 99; // ограничиваем макс.значение до 99 иначе не влезет в экран
if ((BLMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(BLpin, map(BrightnessColHr[2][BLMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываяем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encBLmode == 1) { // и выбран режим выбора часа
BLMenuSelectedHour = ++BLMenuSelectedHour;  // увеличиваем выбранный час
if (BLMenuSelectedHour > 23) BLMenuSelectedHour = 23; // ограничиваем макс.значение до 23ч
}
}



if (encBL.isLeft()) {  // энкодер повернут влево

if (encBLmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[2][BLMenuSelectedHour] = BrightnessColHr[2][BLMenuSelectedHour] - 3;  // уменьшаем яркость
if (BrightnessColHr[2][BLMenuSelectedHour] < 0) BrightnessColHr[2][BLMenuSelectedHour] = 0;  // ограничиваем мин.значение не ниже 0
if ((BLMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(BLpin, map(BrightnessColHr[2][BLMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encBLmode == 1) { // и выбран режим выбора часа
BLMenuSelectedHour = BLMenuSelectedHour - 1;  // уменьшаем выбранный час
if (BLMenuSelectedHour < 0) BLMenuSelectedHour = 0; // ограничиваем мин.значение до 0ч
}
}


if (encBL.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
SaveTimer = millis();                   // сбросить таймер записи в eprom
lcd.setCursor(9, 1); lcd.print(BLMenuSelectedHour); lcd.print(" ");  // выводим на экран выбранный час
lcd.setCursor(9, 2); lcd.print(BrightnessColHr[2][BLMenuSelectedHour]); lcd.print(" ");  // выводим на экран текущее значение яркости для выбранного часа
tone(speaker_pin, 1000, 20); //conflicts with port 10, dont use this port along with tone function
}
}
