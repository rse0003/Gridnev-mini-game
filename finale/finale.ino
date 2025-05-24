// Вот простая программа-игра на Arduino Uno/Arduino Nano с использованием ресурсов, указанных вами:
// Что потребуется:

//     Матрица LED-дисплея размером 8x8 пикселей (LedControl библиотека)
//     Четыре тактильные кнопки (подключенные к цифровым портам Arduino)
//     Динамик-зуммер (для звуков победы/поражения).

// Пример подключения компонентов:

//     Матрица: соединяется через MAX7219 чип (например, модулем MAX72XX)
//     Кнопки (четыре): подключены к следующим выводам Arduino:
//         Кнопка №1 → D2
//         Кнопка №2 → D3
//         Кнопка №3 → D4
//         Кнопка №4 → D5
//     Динамик-зуммер: Подключён к выводу D6 Arduino.

// Библиотеки:

// Перед началом убедитесь, что установили библиотеку LedControl (можно скачать здесь: https://playground.arduino.cc/Main/LedControl/)

#include <LedControl.h> // подключение библиотеки управления матрицей

// Создаем объект LedControl для матрицы
const int dataPin = 12;     // Данные для драйвера MAX7219
const int clkPin = 11;      // Тактовый сигнал
const int csPin = 10;       // Выбор микросхемы
LedControl lc = LedControl(dataPin, clkPin, csPin, 1); // инициализация одного устройства

// Назначаем номера кнопок
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 4
#define BUTTON_4_PIN 5

// Определение выводов динамика
#define BUZZER_PIN 6

// Начальные координаты игрока и динозавра
byte playerX = 0, playerY = 0;
byte dinosaurX = 7, dinosaurY = 7;

// Текущие состояния игры
enum GameState {
  WAITING,
  PLAYING,
  GAME_OVER_WIN,
  GAME_OVER_LOSE
};
GameState currentState = WAITING;

void setup() {
  Serial.begin(9600);
  
  // Инициализируем дисплей
  for(int i=0;i<8;i++) {
    lc.shutdown(i,false);   // выключение режима энергосбережения
    lc.setIntensity(i,8);   // яркость (от 0 до 15)
    lc.clearDisplay(i);     // очищаем экран
  }

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_3_PIN, INPUT_PULLUP);
  pinMode(BUTTON_4_PIN, INPUT_PULLUP);
}

void loop() {
  switch(currentState){
    case WAITING:
      waiting();
      break;
      
    case PLAYING:
      playing();
      break;
    
    case GAME_OVER_WIN:
      gameOverWin();
      break;
        
    case GAME_OVER_LOSE:
      gameOverLose();
      break;
  }
}

void waiting(){
  if(digitalRead(BUTTON_1_PIN)==LOW || digitalRead(BUTTON_2_PIN)==LOW ||
     digitalRead(BUTTON_3_PIN)==LOW || digitalRead(BUTTON_4_PIN)==LOW ){
    currentState = PLAYING;
  }
}

void playing(){
  checkButtons(); // проверяем нажатие кнопок
  moveDinosaur(); // двигаем динозавра
  drawScreen();   // рисуем экран
  
  if(playerX==dinosaurX && playerY==dinosaurY){ // проверка столкновения
    currentState=GAME_OVER_LOSE;
  }
  
  if(dinosaurX<=0 || dinosaurY<=0){ // победа, если динозавр покинул поле
    currentState=GAME_OVER_WIN;
  }
}

void gameOverWin(){
  tone(BUZZER_PIN, 500, 500); // звук победы
  delay(500);
  clearMatrix();
  setPixel(3,3,true); // отмечаем победу звездочкой
  setPixel(4,3,true);
  setPixel(3,4,true);
  setPixel(4,4,true);
  delay(2000);
  resetGame();
}

void gameOverLose(){
  tone(BUZZER_PIN, 200, 500); // проигрышный звук
  delay(500);
  clearMatrix();
  setPixel(3,3,true); // крестик поражения
  setPixel(4,3,true);
  setPixel(3,4,true);
  setPixel(4,4,true);
  delay(2000);
  resetGame();
}

void checkButtons(){
  if(digitalRead(BUTTON_1_PIN)==LOW){
    playerX++;
    if(playerX>=8)playerX=7;
  }
  else if(digitalRead(BUTTON_2_PIN)==LOW){
    playerX--;
    if(playerX<0)playerX=0;
  }
  else if(digitalRead(BUTTON_3_PIN)==LOW){
    playerY++;
    if(playerY>=8)playerY=7;
  }
  else if(digitalRead(BUTTON_4_PIN)==LOW){
    playerY--;
    if(playerY<0)playerY=0;
  }
}

void moveDinosaur(){ // движение динозавра случайное
  randomSeed(millis());
  byte dir = random(4);
  if(dir==0)dinosaurX++;if(dinosaurX>=8)dinosaurX=7;
  if(dir==1)dinosaurX--;if(dinosaurX<0)dinosaurX=0;
  if(dir==2)dinosaurY++;if(dinosaurY>=8)dinosaurY=7;
  if(dir==3)dinosaurY--;if(dinosaurY<0)dinosaurY=0;
}

void drawScreen(){
  clearMatrix();
  setPixel(playerX, playerY, true); // игрок отображается зеленым
  setPixel(dinosaurX, dinosaurY, false); // динозавр отображается красным
}

void clearMatrix(){
  for(byte x=0;x<8;x++){
    for(byte y=0;y<8;y++){
      lc.setLed(0,x,y,false);
    }
  }
}

void setPixel(byte x, byte y, bool state){
  lc.setLed(0,x,y,state);
}

void resetGame(){
  playerX = 0;
  playerY = 0;
  dinosaurX = 7;
  dinosaurY = 7;
  currentState = WAITING;
}

// Объяснение работы программы:

//     Игровое пространство: Игрок представлен точкой в левом верхнем углу экрана (координата [0,0]), а динозавр начинается в правом нижнем углу ([7,7]).
//     Управление игроком: Используя четыре кнопки, игрок перемещается по игровому полю (движение вверх-вниз, вправо-влево). Если кнопка нажата, координаты игрока обновляются соответственно.
//     Ходы динозавра: После каждого хода игрока динозавр делает случайный ход в одну из четырех сторон (случайная смена координат).
//     Условия окончания игры:
//         Если игрок сталкивается с динозавром, игра заканчивается поражением.
//         Если динозавр выходит за пределы поля, игра считается выигранной.
//     Звуковые эффекты: Когда игрок побеждает или терпит неудачу, воспроизводится соответствующий звуковой эффект через динамик.

// Программу можно легко расширить добавлением бонусов ("бомб"), препятствий и новых уровней сложности!


