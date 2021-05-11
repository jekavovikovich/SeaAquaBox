// encoder WW
void DoEncoderWW() {

if (encWW.isClick()) {  // нажата кнопка экодера - меняем его текущий режим - выбор яркости или часа
SaveTimer = millis();                   // сбросить таймер записи в eprom
encWWmode = ++encWWmode;
if (encWWmode > 1) encWWmode = 0;  // modes till 0 to 1, коротким нажатием выбираем только между яркостью и часом

if (encWWmode == 0) { // выбран режим регулировки яркости
lcd.setCursor(4, 1); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(4, 2); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

if (encWWmode == 1) { // выбран режим выбора часа
lcd.setCursor(4, 2); lcd.print(" ");  // затираем указатель во всех остальных позициях
lcd.setCursor(4, 1); lcd.print(">");  // ставим указатель в позицию регулировки яркости
}

tone(speaker_pin, 800, 30); //conflicts with port 10, dont use this port along with tone function
}


if (encWW.isHolded()) {  // кнопка экодера нажата с удержанием, энкодер переходит в альтернативный режим 2
encWWmode = 2;
SaveTimer = millis();                   // сбросить таймер записи в eprom
}


if (encWW.isRight()) {  // энкодер повернут вправо

if (encWWmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[1][WWMenuSelectedHour] = BrightnessColHr[1][WWMenuSelectedHour] + 3; // увеличиваем яркость
if (BrightnessColHr[1][WWMenuSelectedHour] > 99) BrightnessColHr[1][WWMenuSelectedHour] = 99; // ограничиваем макс.значение до 99 иначе не влезет в экран
if ((WWMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(WWpin, map(BrightnessColHr[1][WWMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываяем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encWWmode == 1) { // и выбран режим выбора часа
WWMenuSelectedHour = ++WWMenuSelectedHour;  // увеличиваем выбранный час
if (WWMenuSelectedHour > 23) WWMenuSelectedHour = 23; // ограничиваем макс.значение до 23ч
}
}



if (encWW.isLeft()) {  // энкодер повернут влево

if (encWWmode == 0) { // и выбран режим регулировки яркости
BrightnessColHr[1][WWMenuSelectedHour] = BrightnessColHr[1][WWMenuSelectedHour] - 3;  // уменьшаем яркость
if (BrightnessColHr[1][WWMenuSelectedHour] < 0) BrightnessColHr[1][WWMenuSelectedHour] = 0;  // ограничиваем мин.значение не ниже 0
if ((WWMenuSelectedHour == CurHour) && (OFFflag == false)) analogWrite(WWpin, map(BrightnessColHr[1][WWMenuSelectedHour], 0, 99, 0, MaxBrightness)); // записываем яркость в порт если выбранный час соответствует текущему реальному
SaveFlag = true;                        // поднять флаг для записи значения яркости в eprom
}

if (encWWmode == 1) { // и выбран режим выбора часа
WWMenuSelectedHour = WWMenuSelectedHour - 1;  // уменьшаем выбранный час
if (WWMenuSelectedHour < 0) WWMenuSelectedHour = 0; // ограничиваем мин.значение до 0ч
}
}


if (encWW.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
SaveTimer = millis();                   // сбросить таймер записи в eprom
lcd.setCursor(5, 1); lcd.print(WWMenuSelectedHour); lcd.print(" ");  // выводим на экран выбранный час
lcd.setCursor(5, 2); lcd.print(BrightnessColHr[1][WWMenuSelectedHour]); lcd.print(" ");  // выводим на экран текущее значение яркости для выбранного часа
tone(speaker_pin, 1000, 20); //conflicts with port 10, dont use this port along with tone function
}
}
