//Arduino_measurer_URC_en_3
// upgraded from _measurer_en_3
// A1 to A7 inputs for measuring (e.g.  Input 1 to Input 7 for SerialReader)
//next commands
//Ua where a = 0 is normal and a = 1 is negativ map of analog data
//Va where a is nr of analog pin 0 to 7 - default is A0 (e.g. Input 8 for SerialReader)
// unified commands A,B,C,D for measuring and control commands ...
// G command => Redled LOW
// H command => RedLed HIGH both for measuring in RC mode

//#define powerLED 13
#define greenLED 8
#define yellowLED 7
#define redLED 6

String sketchName = "Arduino_measurer_URC_en_3";

/*
#define analogPin7 7
#define analogPin6 6
#define analogPin5 5
#define analogPin4 4
#define analogPin3 3
#define analogPin2 2
#define analogPin1 1
#define analogPin0 0
*/

int analogPinA0 = 0;
int analogPinA1 = 1;
int analogPinA2 = 2;
int analogPinA3 = 3;
int analogPinA4 = 4;
int analogPinA5 = 5;
int analogPinA6 = 6;
int analogPinA7 = 7;

unsigned int analogValue;

unsigned int analogInput = 0;

unsigned int timeStart = 0;
unsigned int dataTime = 0;
unsigned int timeOfMeasuring = 0;
int dataStep = 20;
int dataOffset = 0;
int dataOffsetMax = 400;

int uMap = 0;  // 0 = normal, 1 = negative maps

int I = 0;
int J = 0;
int dataIndex = 0;
int dataWaitTime = 100;
//unsigned int dataOscArray[402];
int dataNumber = 400;
int dataNumberMax = 401;

int slaveNumber = 3;

bool menuOn = false;
bool serialPrintOn = false;
char markRx = 0;
byte dataMode = 0;

byte analogTest = 0;

int dataCycle = 0;
bool dataCycleOn = false;

void AnalogData(int analogInp) 
 {
  switch (analogInp) {
    case 0:
      {
        analogValue = analogRead(analogPinA0);
      }
      break;
    case 1:
      {
        analogValue = analogRead(analogPinA1);
      }
      break;
    case 2:
      {
        analogValue = analogRead(analogPinA2);
      }
      break;
    case 3:
      {
        analogValue = analogRead(analogPinA3);
      }
      break;
    case 4:
      {
        analogValue = analogRead(analogPinA4);
      }
      break;
    case 5:
      {
        analogValue = analogRead(analogPinA5);
      }
      break;
    case 6:
      {
        analogValue = analogRead(analogPinA6);
      }
      break;
    case 7:
      {
        analogValue = analogRead(analogPinA7);
      }
      break;
    default:
      {
        analogValue = analogRead(analogPinA7);
      }
      break;
   }
  if (uMap > 0) {
    analogValue = map(analogValue, 1, 1023, 1023, 1);
  analogTest = 0;  
  if (analogValue == 0)
    {
      analogTest = 1;
      digitalWrite(yellowLED, HIGH);       
    }
  if (analogValue > 1020)
    {
      analogTest = 2;
      digitalWrite(yellowLED, HIGH);       
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(analogPinA0, INPUT);
  pinMode(analogPinA1, INPUT);
  pinMode(analogPinA2, INPUT);
  pinMode(analogPinA3, INPUT);
  pinMode(analogPinA4, INPUT);
  pinMode(analogPinA5, INPUT);
  pinMode(analogPinA6, INPUT);
  pinMode(analogPinA7, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, HIGH);
  delay(1000);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, HIGH);
  delay(1000);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  analogInput = 0;  //analogInput = analogPinA0;
  dataStep = 20;
  dataNumber = 400;
  dataTime = 0;
  dataOffset = 0;
  dataCycle = 0;
  menuOn = true;
  I = 1;
}

void loop() {
  if (menuOn == true) {
    digitalWrite(greenLED, HIGH);
    Serial.println(sketchName);
    Serial.println();
    Serial.println("Menu:");
    Serial.println("A=read one data from AX=0to7, B=read data from inputs A0 and A3");
    Serial.println("C=read data from inputs A0 to A7, D=set of measuring in cycle");
    Serial.println("J=read analogInput and uMap info,");
    Serial.println("M=measuring data in balk from pre-set analogInput,");
    Serial.println("Exx=data step, Lxxx=nr of measured data, Oxxx=offset of data,");
    Serial.println("F=serial print details on, Q=serial print details off!");
    Serial.println("G=red led LOW, H=red led HIGH,");
    Serial.println("P or R=read parameters, S=Stop of data transfer,");
    Serial.println("U=0 normal 1=inverted data, Vx x=0 to 7 (A0 to A7) !");
    Serial.println();
    Serial.println("Data On :");
    Serial.print("*I=Slave Number,"); // for compability with master-slave system
    Serial.print(slaveNumber);
    Serial.println(",#,");
    Serial.print("*J=uMap,");
    Serial.print(analogInput);
    Serial.print(",");
    Serial.print(uMap);
    Serial.println(",#,");
    Serial.print("*P=Parameters,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataNumber);
    Serial.print(",");
    Serial.print(dataNumberMax);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.println(",#,");
    Serial.println();
                
    menuOn = false;
    delay(500);
    //digitalWrite(yellowLED,LOW);
    dataMode = 89;
  }

  if (Serial.available()) 
   {
    //digitalWrite(yellowLED,HIGH);
    markRx = Serial.read();

    if (markRx == 65) 
     {
      dataMode = 65;  // read one data
      dataIndex = 0;
     }

    if (markRx == 66) 
     {
      dataMode = 66;  // B - read analog data in cycle in format *B,data,#,CRLF
      digitalWrite(yellowLED, LOW);
      dataIndex = 0;
      timeStart = millis();
     }

      
    if (markRx == 67)  // C = data for serial reader in format data CRLF
    {      
      digitalWrite(yellowLED, LOW);
      dataTime = 0;
      dataIndex = 0;
      timeStart = millis();
      if (serialPrintOn) 
       {
        Serial.print("*C,");
        Serial.print(dataStep);
        Serial.print(",");
        Serial.print(dataNumber);
        Serial.print(",");
        Serial.print(dataTime);
        Serial.print(",");
        Serial.print(dataOffset);
      }
      dataMode = 67;
    }

    if (markRx == 68)  // D1 - set measuring in cycle
    {
      digitalWrite(yellowLED, LOW);
      String Pstring = Serial.readString();
      Pstring.trim();
      dataCycle = Pstring.toInt();
      if (dataCycle>0)
       {
        dataCycleOn = true;
       }
      else
       {
        dataCycleOn = false;
       } 
      dataMode = 89;
    }

    if (markRx == 69)  // Exx data step
    {
      dataMode = 69;  // E - Data step
      String Pstring = Serial.readString();
      Pstring.trim();
      dataStep = Pstring.toInt();
      dataMode = 80;
    }

    if (markRx == 70)  // F - stop of data transfer and print on
    {
      menuOn = true;
      serialPrintOn = true;
      dataMode = 89;
    }

    if (markRx == 71)  // G - yellow LED on
    {
      digitalWrite(redLED, LOW);
      //delay(1000);
      //digitalWrite(yellowLED, LOW);
      dataMode = 89;
    }

    if (markRx == 72)  // H - red LED on
    {
      //digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, HIGH);
      //delay(1000);
      //digitalWrite(redLED, LOW);
      //menuOn = false;
      dataMode = 89;
      //Pline = 1;
    }

    if (markRx == 73)  // I - info of master not used
    {
      Serial.print("*I,");
      Serial.print(slaveNumber);
      Serial.println(",#,");
      dataMode = 89;
    }

    if (markRx == 74)  // J - analogInput and uMap
    {
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, HIGH);
      Serial.print("*J,");
      Serial.print(analogInput);
      Serial.print(",");
      Serial.print(uMap);
      Serial.println(",#,");
      delay(200);
      digitalWrite(greenLED, LOW);
      dataMode = 89;
    }

    if (markRx == 75)  // Kx - not used here, so
    {
      String pomString = Serial.readString();
      slaveNumber = pomString.toInt();
      Serial.print("*K,");
      Serial.print(slaveNumber);
      Serial.println(",#,");
      dataMode = 89;
    }


    if (markRx == 76)  // Lxxx - nr of data for M measuring to memory
    {
      String Pstring = Serial.readString();
      Pstring.trim();
      dataNumber = Pstring.toInt();
      dataMode = 80;
    }

    if (markRx == 77)  // M - read analog data and saved them to internal memory
    {
      dataMode = 77;
      I = 1;
    }

    if (markRx == 78)  // N - read and transmitt data from memory
    {
      dataMode = 78;
    }

    if (markRx == 79)  // O - data offset
    {
      String Pstring = Serial.readString();
      dataOffset = Pstring.toInt();
      if (dataOffset > dataOffsetMax) {
        dataOffset = dataOffsetMax;
      }
      dataMode = 80;
    }


    if (markRx == 80)  // P  read parameters
    {
      dataMode = 80;
    }

    if (markRx == 81)  // Q - serial print off
    {
      serialPrintOn = false;
      dataMode = 89;
    }

    if (markRx == 82)  // R read parameters
    {
      dataMode = 82;
    }


    if (markRx == 83) {
      dataMode = 83;
    }

    if (markRx == 84)  // T = wait time not used
    {
      String Pstring = Serial.readString();
      Pstring.trim();
      dataWaitTime = Pstring.toInt();
      dataMode = 84;
    }

    if (markRx == 85)  // U = data normal = 0 or inverted = 1
    {
      String Pstring = Serial.readString();
      Pstring.trim();
      uMap = Pstring.toInt();
      Serial.print("*U,");
      Serial.print(uMap);
      Serial.println(",#,");
      dataMode = 89;
    }

    if (markRx == 86)  // V = analogPi 0 to 7
    {
      String Pstring = Serial.readString();
      Pstring.trim();
      analogInput = Pstring.toInt();
      if (analogInput > 7) {
        analogInput = 7;
      }
      
      Serial.print("*V,");
      Serial.print(analogInput);
      Serial.println(",#,");
      dataMode = 89;
    }

    markRx = 0;
    Serial.flush();
  }  // end of Serial available

  if (dataMode == 65)  // A - data from analogInput one
  {
    dataIndex = dataIndex + 1;
    if (dataStep > 0) 
     {
      delay(dataStep);
     }
    AnalogData(analogInput);
    //analogValue = analogRead(analogPinA0);
    Serial.print("*A,");
    Serial.print(analogValue + dataOffset);
    Serial.println(",#,");
    if (!dataCycleOn)
     {
       dataMode = 89;
     }
  }


  if (dataMode == 66)  // B - read data in  block A) and A! and or no in cycle
  {
    dataIndex = dataIndex + 1;
    if (dataStep > 0) 
     {
      delay(dataStep);
     }
    Serial.print("*B,");
    for (I=0; I<4; I++)
     {
      //analogInput = I;
      AnalogData(I);
      Serial.print(analogValue + dataOffset);
      Serial.print(",");
     }
    //analogData(analogInput);
    //Serial.print(analogValue + dataOffset);
    Serial.println("#,");
    if (!dataCycleOn)
     {
       dataMode = 89;
     }
  }

  if (dataMode == 67)  // C - data in block A0 to A7
  {
    dataIndex = dataIndex + 1;
    if (dataStep > 0) 
    {
      delay(dataStep);
    }
    Serial.print("*C,");
    //analogValue = analogRead(analogPin) + dataOffset;
    for (I=0; I<8; I++)
     {
      //analogInput = I;
      AnalogData(I);
      Serial.print(analogValue + dataOffset);
      Serial.print(",");
     }
    Serial.println("#,");
    if (!dataCycleOn)
     {
       dataMode = 89;
     }  
  }


  if (dataMode == 77)  // M - read analog data and save them to memory
  {
    Serial.print("*M,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataNumber);
    Serial.print(",");
    Serial.print(0);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.println(",#,");
    timeStart = millis();
    J = 1;
OpakM:
    AnalogData(analogInput);
    //dataOscArray[J] = analogValue;
    Serial.println("*A,analogValue,#,");
    if (dataStep > 0) 
     {
      delay(dataStep);
     }
    J = J + 1;
    if (J < dataNumber)
     {
       goto OpakM;
     }
    timeOfMeasuring = millis() - timeStart;
    dataMode = 89;
  }

  /*
  if (dataMode == 78)  // N - read and transmitt data from memory
  {
    dataTime = timeOfMeasuring;
    Serial.print("*N,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataNumber);
    Serial.print(",");
    Serial.print(dataTime);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.print(",");
    I = 1;
OpakN:
    Serial.print(dataOscArray[I]);
    Serial.print(",");
    if (I < dataNumber) {
      I = I + 1;
      goto OpakN;
    }
    Serial.println("#,");
    dataMode = 89;
  }
  */

  if (dataMode == 80) {
    //timeStart = millis();
    Serial.print("*P,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataNumber);
    Serial.print(",");
    Serial.print(dataNumberMax);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.println(",#,");
    //delay(250);
    dataMode = 89;
  }

     /*
      if (dataMode ==  81)
        {
            Serial.print("*Q,");
            Serial.print(dataStep);
            Serial.print(",");
            Serial.print(dataNumber);
            Serial.print(",");
            Serial.print(dataNumberMax);
            Serial.print(",");
            Serial.print(dataOffset);
            Serial.println(",#,");
            serialPrintOn = false;
            dataMode = 89;
        }    
       */

  if (dataMode == 82) {
    Serial.print("*R,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataNumber);
    Serial.print(",");
    Serial.print(dataTime);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.println(",#,");
    dataMode = 89;
  }

  if (dataMode == 83)  // S - stop of reading data in cycle
  {
    dataTime = millis() - timeStart;
    Serial.print("*S,");
    Serial.print(dataStep);
    Serial.print(",");
    Serial.print(dataIndex);
    Serial.print(",");
    Serial.print(dataTime);
    Serial.print(",");
    Serial.print(dataOffset);
    Serial.println(",#,");
    dataMode = 89;
  }

  if (dataMode == 84)  // S - stop of reading data in cycle
  {
    dataTime = millis() - timeStart;
    Serial.print("*T,");
    Serial.print(dataWaitTime);
    Serial.println(",#,");
    dataMode = 89;
  }

  if (dataMode == 89) {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    //digitalWrite(redLED, LOW);
    //delay(1);
  }
}
