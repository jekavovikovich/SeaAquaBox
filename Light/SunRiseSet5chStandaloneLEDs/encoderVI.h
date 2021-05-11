// encoder VI
void DoEncoderVI() {

if (encVI.isClick()) {  // нажата кнопка экодера - меняем его текущий режим - выбор яркости или часа
SaveTimer = millis();                   // сбросить таймер записи в eprom
encVImode = ++encVImode;
if (encVImode > 1) encVImode = 0;  // modes till 0 to 1, коротким нажатием выбираем только между яркостью и часом

if (encVImode == 0) { // выбран режим регулировки яркости
lcd.setCursor(12, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(12, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

if (encVImode == 1) { // выбран режим выбора часа
lcd.setCursor(12, 2); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(12, 1); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

tone(speaker_pin, 800, 30); //conflicts with port 10, dont use this port along with tone function
}


if (encVI.isHolded()) {  // кнопка экодера нажата с удержанием, энкодер переходит в альтернативный режим 2
encVImode = 2;
SaveTimer = millis();                   // сбросить таймер записи в eprom
}


if (encVI.isRight()) {  // энкодер повернут вправо

if (encVImode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[3][VIMenuSelectedHour] = BrightnessColHr[3][VIMenuSelectedHour] + 3; // увеличиваем яркость
if (BrightnessColHr[3][VIMenuSelectedHour] > 99) BrightnessColHr[3][VIMenuSelectedHour] = 99; // ограничиваем макс.значение до 99 иначе не влезет в экран
if ((VIMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(VIpin, map(BrightnessColHr[3][VIMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываяем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encVImode == 1) { // и выбран режим выбора часа
VIMenuSelectedHour = ++VIMenuSelectedHour;  // увеличиваем выбранный час
if (VIMenuSelectedHour > 23) VIMenuSelectedHour = 23; // ограничиваем макс.значение до 23ч
}
}



if (encVI.isLeft()) {  // энкодер повернут влево

if (encVImode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[3][VIMenuSelectedHour] = BrightnessColHr[3][VIMenuSelectedHour] - 3;  // уменьшаем яркость
if (BrightnessColHr[3][VIMenuSelectedHour] < 0) BrightnessColHr[3][VIMenuSelectedHour] = 0;  // ограничиваем мин.значение не ниже 0
if ((VIMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(VIpin, map(BrightnessColHr[3][VIMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encVImode == 1) { // и выбран режим выбора часа
VIMenuSelectedHour = VIMenuSelectedHour - 1;  // уменьшаем выбранный час
if (VIMenuSelectedHour < 0) VIMenuSelectedHour = 0; // ограничиваем мин.значение до 0ч
}
}


if (encVI.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
SaveTimer = millis();                   // сбросить таймер записи в eprom
lcd.setCursor(13, 1); lcd.print(VIMenuSelectedHour); lcd.print(" ");  // выводим на экран выбранный час
lcd.setCursor(13, 2); lcd.print(BrightnessColHr[3][VIMenuSelectedHour]); lcd.print(" ");  // выводим на экран текущее значение яркости для выбранного часа
tone(speaker_pin, 1000, 20); //conflicts with port 10, dont use this port along with tone function
}
}
