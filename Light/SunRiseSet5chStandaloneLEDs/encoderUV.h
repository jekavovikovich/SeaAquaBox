// encoder UV
void DoEncoderUV() {

if (encUV.isClick()) {  // нажата кнопка экодера - меняем его текущий режим - выбор яркости или часа
SaveTimer = millis();                   // сбросить таймер записи в eprom
encUVmode = ++encUVmode;
if (encUVmode > 1) encUVmode = 0;  // modes till 0 to 1, коротким нажатием выбираем только между яркостью и часом

if (encUVmode == 0) { // выбран режим регулировки яркости
lcd.setCursor(16, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(16, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

if (encUVmode == 1) { // выбран режим выбора часа
lcd.setCursor(16, 2); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(16, 1); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

tone(speaker_pin, 800, 30); //conflicts with port 10, dont use this port along with tone function
}


if (encUV.isHolded()) {  // кнопка экодера нажата с удержанием, энкодер переходит в альтернативный режим 2
encUVmode = 2;
SaveTimer = millis();                   // сбросить таймер записи в eprom
}


if (encUV.isRight()) {  // энкодер повернут вправо

if (encUVmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[4][UVMenuSelectedHour] = BrightnessColHr[4][UVMenuSelectedHour] + 3; // увеличиваем яркость
if (BrightnessColHr[4][UVMenuSelectedHour] > 99) BrightnessColHr[4][UVMenuSelectedHour] = 99; // ограничиваем макс.значение до 99 иначе не влезет в экран
if ((UVMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(UVpin, map(BrightnessColHr[4][UVMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываяем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encUVmode == 1) { // и выбран режим выбора часа
UVMenuSelectedHour = ++UVMenuSelectedHour;  // увеличиваем выбранный час
if (UVMenuSelectedHour > 23) UVMenuSelectedHour = 23; // ограничиваем макс.значение до 23ч
}
}



if (encUV.isLeft()) {  // энкодер повернут влево

if (encUVmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[4][UVMenuSelectedHour] = BrightnessColHr[4][UVMenuSelectedHour] - 3;  // уменьшаем яркость
if (BrightnessColHr[4][UVMenuSelectedHour] < 0) BrightnessColHr[4][UVMenuSelectedHour] = 0;  // ограничиваем мин.значение не ниже 0
if ((UVMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(UVpin, map(BrightnessColHr[4][UVMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encUVmode == 1) { // и выбран режим выбора часа
UVMenuSelectedHour = UVMenuSelectedHour - 1;  // уменьшаем выбранный час
if (UVMenuSelectedHour < 0) UVMenuSelectedHour = 0; // ограничиваем мин.значение до 0ч
}
}


if (encUV.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
SaveTimer = millis();                   // сбросить таймер записи в eprom
lcd.setCursor(17, 1); lcd.print(UVMenuSelectedHour); lcd.print(" ");  // выводим на экран выбранный час
lcd.setCursor(17, 2); lcd.print(BrightnessColHr[4][UVMenuSelectedHour]); lcd.print(" ");  // выводим на экран текущее значение яркости для выбранного часа
tone(speaker_pin, 1000, 20); //conflicts with port 10, dont use this port along with tone function
}
}
