/*  Тестер емкости конденсатора 0,1 - 3000 мкФ
 *   
 *    MOSI - pin 11
 *    MISO - pin 12
 *    CLK -  pin 13
 *    CS -   pin 10 
 *    
 *    TC = R * C
 *    TC = период в сек.
 *    C = емкость в F (1 mcF = 0.000001 F)
 *    отдача тока до 63,2% от полного разряда
 *  Кондёр между ц пинами, через
 *  R на разряд 5-10 кОм, точное значение в переменную; на заряд 220 Ом  
 *  Аналоговый напрямую
 *  Прикрутить ESR-тест
 *  Вывод в COM и на 1602
 *  запись на MicroSD
 */
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#define analogPin      0         
#define chargePin      9         
#define dischargePin   8         
#define resistorValue  10000.0F   

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
                                 
const int chipSelect = 10;
unsigned long startTime;
unsigned long elapsedTime;
float microFarads;               
float nanoFarads;

void setup(){
  lcd.begin(16,2);
  pinMode(chargePin, OUTPUT);   
  digitalWrite(chargePin, LOW);  

  Serial.begin(9600);  
  if (!SD.begin(chipSelect)) {
        Serial.println("Card error");
        return;
    }            
}

void loop(){
  //charging
  digitalWrite(chargePin, HIGH);  
  startTime = millis();

  while(analogRead(analogPin) < 648){       // 647 is 63.2% of 1023
  }

  elapsedTime= millis() - startTime;
 // convert ms to s and F to mcF  
  microFarads = ((float)elapsedTime / resistorValue) * 1000;  
  Serial.print(elapsedTime);       // print to serial port
  Serial.print(" mS    ");
  lcd.print(elapsedTime);      
  lcd.print(" mS    ");

  if (microFarads > 1){
    Serial.print((long)microFarads);      
    Serial.println(" mF");  
    lcd.print((long)microFarads);
    lcd.print("mF"); 
    void resToSD();   
  }
  else
  {
    nanoFarads = microFarads * 1000.0;     
    Serial.print((long)nanoFarads);        
    Serial.println(" nF");   
    lcd.print((long)nanoFarads);
    lcd.print(" nF");  
    void resToSD();   
  }
  // dicharge
  digitalWrite(chargePin, LOW);            
  pinMode(dischargePin, OUTPUT);          
  digitalWrite(dischargePin, LOW);          
  while(analogRead(analogPin) > 0){         // ждать пока польностью разрядится
  }

  pinMode(dischargePin, INPUT);            // stop
}

//сбор на SD
   void resToSD() {
     File dataFile = SD.open("result.txt", FILE_WRITE);
     if (dataFile) {
        // в строку
        dataFile.print(elapsedTime);
        if(microFarads > 1){
          dataFile.println(microFarads);
        }
        else{
          dataFile.println(nanoFarads);
        }
        dataFile.close();
        
    } else {
        lcd.setCursor(0,1);
        lcd.print("sd error");
        Serial.println("SD card error");
    }
   }
