int DELAYTIME = 500;
int PIN2 = 2;
int PIN3 = 3;
int LEDPIN1 = 4;
int LEDPIN2 = 5;
int LEDPIN3 = 6;
int LEDPIN4 = 7;
int LEDPIN5 = 8;
int LEDPIN6 = 9;
int LEDPIN7 = 10;
int LEDPIN8 = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN2, INPUT);
  pinMode(PIN3, OUTPUT);
  
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);
  pinMode(LEDPIN4, OUTPUT);
  pinMode(LEDPIN5, OUTPUT);
  pinMode(LEDPIN6, OUTPUT);
  pinMode(LEDPIN7, OUTPUT);
  pinMode(LEDPIN8, OUTPUT);
}

bool started = false;
void loop() {
  // put your main code here, repeatedly:
  if (digitalRead(PIN2) == HIGH) {
    if (!started) {
      // blink LEDs
      for (int i = 0; i < 5; ++i) {
        digitalWrite(LEDPIN1, HIGH);
        digitalWrite(LEDPIN2, HIGH);
        digitalWrite(LEDPIN3, HIGH);
        digitalWrite(LEDPIN4, HIGH);
        digitalWrite(LEDPIN5, HIGH);
        digitalWrite(LEDPIN6, HIGH);
        digitalWrite(LEDPIN7, HIGH);
        digitalWrite(LEDPIN8, HIGH);
        
        delay(DELAYTIME);
        digitalWrite(LEDPIN1, LOW);
        digitalWrite(LEDPIN2, LOW);
        digitalWrite(LEDPIN3, HIGH);
        digitalWrite(LEDPIN4, HIGH);
        digitalWrite(LEDPIN5, HIGH);
        digitalWrite(LEDPIN6, HIGH);
        digitalWrite(LEDPIN7, HIGH);
        digitalWrite(LEDPIN8, HIGH);
        delay(DELAYTIME);
      }
      
      started = true;
    }
    
    digitalWrite(PIN3, HIGH);
    
  } else {
    digitalWrite(PIN3, LOW);
    started = false;
  }
  
}
