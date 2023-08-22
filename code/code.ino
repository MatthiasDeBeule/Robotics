long potValue, potValuePrevious;
volatile long count, countPrevious;
const float maxSpinCount = 312.5;

bool manualInput = 0;
bool startupReset = 0;

volatile int slowSpeed = 170; // 145
volatile int fastSpeed = 210; //175

#define OPTOINPUT1 2
#define OPTOINPUT2 3
#define POTMETER 0

#define MODES_BUTTON 13
#define RESET_BUTTON 12
#define REVERSE_PIN 10
#define FORWARD_PIN 9
#define SPEED_PIN 11
#define RESET_INPUT 4

void setup() {
  attachInterrupt(digitalPinToInterrupt(OPTOINPUT1), revCounter, RISING);
  pinMode(OPTOINPUT1, INPUT_PULLUP);
  pinMode(OPTOINPUT2, INPUT_PULLUP);
  
  pinMode(REVERSE_PIN, OUTPUT);
  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);

  pinMode(MODES_BUTTON, INPUT_PULLUP); 
  pinMode(RESET_BUTTON, INPUT_PULLUP); 
  
  pinMode(RESET_INPUT, INPUT_PULLUP);

  analogWrite(SPEED_PIN, fastSpeed);
  
  potValuePrevious = analogRead(A0);
  
  Serial.begin(9600);
}

void loop() {
  Serial.println(potValue);

  if(startupReset == 0 || digitalRead(RESET_BUTTON) == LOW)
  {
    //speed reset
    analogWrite(SPEED_PIN, slowSpeed);
    digitalWrite(REVERSE_PIN, HIGH);
    
    while(digitalRead(RESET_INPUT) == HIGH){Serial.println(digitalRead(RESET_INPUT));}
    
    digitalWrite(REVERSE_PIN, LOW);
    analogWrite(SPEED_PIN, fastSpeed);
    potValuePrevious = 1023;
    startupReset = 1;
  }
  
  potValue = analogRead(A0);
  Serial.println(potValue);


  if (digitalRead(MODES_BUTTON) == LOW)
  {
    delay(100);

    manualInput = manualInput == 0 ? 1 : 0;

    while(digitalRead(MODES_BUTTON) != LOW)
    {
      Serial.println("Please enter a number (-10,0 - 10,0):");

      while (Serial.available() == 0) 
      {
        if(digitalRead(MODES_BUTTON) == LOW ) 
        { 
          delay(200); 
          return;
        }
      }

      float userNumber = Serial.parseInt();
      Serial.println(userNumber);

      potValue = map(userNumber, -10, 10, 0, 1028);
      count = calculateCount(potValue, potValuePrevious);
      
      Serial.println(potValue);
      Serial.println(potValuePrevious);
      Serial.println(count);

      executeMovement();

      potValuePrevious = potValue;
    }

    return;
  }
  
  
  if (abs(potValue - potValuePrevious) > 20 || manualInput == 1)
  {
    count = calculateCount(potValue, potValuePrevious);
    Serial.println(count);
    
    executeMovement();
    
    potValuePrevious = potValue;
  }
}

long calculateCount(long value1, long value2)
{
  long mappedValue1 = round((value1 / 1023.0) * maxSpinCount);
  long mappedValue2 = round((value2 / 1023.0) * maxSpinCount);

  long count = (mappedValue1 - mappedValue2) * 2;

  return count;
}

void revCounter(void)
{
  if (millis() - countPrevious > 2800)
  {
    if (digitalRead(OPTOINPUT2))
    {
      count++;
      Serial.println(count);
      Serial.println("-+-");
    }
    else
    {
      count--;
      Serial.println(count);
      Serial.println("---");
    }
    countPrevious = micros();
  }
}

void executeMovement(void)
{
  if(count < 1)
    {
      digitalWrite(FORWARD_PIN, HIGH);

      while(count < 1)
      {
        if(analogRead(0) != potValue && 1 == 0)
        {
          count = calculateCount(analogRead(0), potValuePrevious);
        }

        if(count < -50)
        {
          analogWrite(SPEED_PIN, fastSpeed);
        }
        if(count > -50)
        {
          analogWrite(SPEED_PIN, slowSpeed);
        }
      }
      
      digitalWrite(FORWARD_PIN, LOW);

      return;
    }

    if(count > 1)
    {
      digitalWrite(REVERSE_PIN, HIGH);

      while(count > 1)
      {
        if(count > 50)
        {
          analogWrite(SPEED_PIN, fastSpeed);
        }
        if(count < 50)
        {
          analogWrite(SPEED_PIN, slowSpeed);
        }
      }
      
      digitalWrite(REVERSE_PIN, LOW);

      return;
    }
}
