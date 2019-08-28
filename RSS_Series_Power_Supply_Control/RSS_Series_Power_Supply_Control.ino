//Authored by Gokul


//for AD5760 Controller
#define MISO 46
#define MOSI 47
#define SCK 48
#define SS 49
#define LDAC 37
#define RESET 36




#define MISO_91_3P 22 //SDOUT
#define MOSI_91_3P 23 //SDIN
#define SCK_91_3P 24 //CLK
#define SS_91_3P 25 //Sync
#define LDAC_91_3P 26 //ldac: 
#define RESET_91_3P 27 //reset
#define CLEAR_91_3P 28 //clear 


#define MISO_91_6N 30 //SDOUT
#define MOSI_91_6N 31 //SDIN
#define SCK_91_6N 32 //CLK
#define SS_91_6N 33 //Sync
#define LDAC_91_6N 34 //ldac: 
#define RESET_91_6N 35 //reset
#define CLEAR_91_6N 36 //clear 

#define MISO_91_6N_2 42 //SDOUT
#define MOSI_91_6N_2 43 //SDIN
#define SCK_91_6N_2 44 //CLK
#define SS_91_6N_2 45 //Sync
#define LDAC_91_6N_2 46 //ldac: 
#define RESET_91_6N_2 47 //reset
#define CLEAR_91_6N_2 48 //clear 




float voltage = 0;
float _step = 0.0003;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String commandString = "";
int led1Pin = 7;

float sentData;
String recievedString;
String convertedString;
float covertedFloat;

boolean isConnected = false;

static const int V_refn =  -10;
static const int V_refp =  10;


static const uint32_t WRITE_CONTROL_REGISTER_INTIALIZE = 0B001000000000001010000010;
static const uint32_t WRITE_DAC_REGISTER_INTIALIZE = 0B0001; //(<20) | databits;

void setup() {
  Serial.begin(9600);

  // set up pins 5760
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SS, OUTPUT);
  pinMode(LDAC, OUTPUT);
  pinMode(RESET, OUTPUT);

  // set up pins 3P 5791
  pinMode(MISO_91_3P, INPUT);
  pinMode(MOSI_91_3P, OUTPUT);
  pinMode(SCK_91_3P, OUTPUT);
  pinMode(SS_91_3P, OUTPUT);
  pinMode(LDAC_91_3P, OUTPUT);
  pinMode(RESET_91_3P, OUTPUT);
  pinMode(CLEAR_91_3P, OUTPUT);

 /* // set up pins spellman 5791
  pinMode(MISO_91_150, INPUT);
  pinMode(MOSI_91_150, OUTPUT);
  pinMode(SCK_91_150, OUTPUT);
  pinMode(SS_91_150, OUTPUT);
  pinMode(LDAC_91_150, OUTPUT);
  pinMode(RESET_91_150, OUTPUT);
  pinMode(CLEAR_91_150, OUTPUT);

  */
  // set up pins -6N 5791
  pinMode(MISO_91_6N, INPUT);
  pinMode(MOSI_91_6N, OUTPUT);
  pinMode(SCK_91_6N, OUTPUT);
  pinMode(SS_91_6N, OUTPUT);
  pinMode(LDAC_91_6N, OUTPUT);
  pinMode(RESET_91_6N, OUTPUT);
  pinMode(CLEAR_91_6N, OUTPUT);

  // set up pins -6N 5791
  pinMode(MISO_91_6N_2, INPUT);
  pinMode(MOSI_91_6N_2, OUTPUT);
  pinMode(SCK_91_6N_2, OUTPUT);
  pinMode(SS_91_6N_2, OUTPUT);
  pinMode(LDAC_91_6N_2, OUTPUT);
  pinMode(RESET_91_6N_2, OUTPUT);
  pinMode(CLEAR_91_6N_2, OUTPUT);


  //intial state + power up ad5760 + 5791
  digitalWrite(SS, HIGH);
  digitalWrite(MOSI, LOW);
  digitalWrite(SCK, LOW);
  digitalWrite(RESET_91_3P, HIGH);
  digitalWrite(CLEAR_91_3P, HIGH);

  initialize();

  // setVoltage(5000);
  //setVoltage(790); //set in actual desired voltage out. The voltage set is divided by a factor of 1000 for 10N, 600 for 6N, 300 for 3P
}

void loop() {
  if (stringComplete)
  {
  
    stringComplete = false;
    getCommand();

    if (commandString.equals("STOP"))
    {
      turnLedOff(led1Pin);
    }
    else if (commandString.equals("SEND"))
    {
      String text = getTextNumber();
      recievedString = text;
      float data = (float) text.toFloat();
      Serial.print("Float sent");
      Serial.print(data);
      Serial.print("\n");
      sentData = data;
      sendSerialFloat(data, Serial1);
    }
    else if (commandString.equals("LED1"))
    {
      boolean LedState = getLedState();
      if (LedState == true)
      {
        turnLedOn(led1Pin);
      } else
      {
        turnLedOff(led1Pin);
      }

    }
    else if (commandString.equals("SVT1"))
    {
      String text = getTextNumber();
      recievedString = text;
      float data = (float) text.toFloat();
      Serial.print("Float sent");
      Serial.print(data);
      Serial.print("\n");
      //setVoltage(data);
      sendVoltageDACRegister_3P(data);
    }
    else if (commandString.equals("SVT2"))
    {
      String text = getTextNumber();
      recievedString = text;
      float data = (float) text.toFloat();
      Serial.print("Float sent");
      Serial.print(data);
      Serial.print("\n");
      // setVoltage(data);
      sendVoltageDACRegister_6N(data);
    }
     else if (commandString.equals("SVT3"))
    {
      String text = getTextNumber();
      recievedString = text;
      float data = (float) text.toFloat();
      Serial.print("Float sent");
      Serial.print(data);
      Serial.print("\n");
      // setVoltage(data);
      sendVoltageDACRegister_6N_2(data);
    }
     else if (commandString.equals("SVT4"))
    {
      String text = getTextNumber();
      recievedString = text;
      float data = (float) text.toFloat();
      Serial.print("Float sent");
      Serial.print(data);
      Serial.print("\n");
      // setVoltage(data);
      sendVoltageDACRegister_3P(data);
    }
    inputString = "";
  }
}

void setVoltage(double voltage) {
  uint16_t data = (uint16_t) ((voltage / 600) * 65535 / 10);
  data = data & 0xffff;
  Serial.print("Voltage Set: ");
  Serial.print(voltage, 1);
  Serial.print("   Binary Voltage: ");
  Serial.print(data, BIN);

  uint8_t first = 0b00010000 | (data >> 12);
  uint16_t second = (data << 4) & (0xffff);

  Serial.print("   Binary Control Signal: ");
  Serial.print(first, BIN);
  Serial.println(second, BIN);

  digitalWrite(SS, LOW);
  sendByte(first);
  sendByte((uint8_t)((second >> 8) & 0xff));
  sendByte((uint8_t)((second) & 0xf0));
  digitalWrite(SS, HIGH);
}



void initialize() {
  digitalWrite(SS, LOW);
  uint8_t high = 0b01000000;
  uint8_t mid = 0b00000000;
  uint8_t low = 0b00000000;
  sendByte(high);
  //digitalWrite(MOSI, LOW);
  sendByte(mid);
  //digitalWrite(MOSI, LOW);
  sendByte(low);
  //digitalWrite(MOSI, LOW);
  digitalWrite(SS, HIGH);

  digitalWrite(SS, LOW);
  uint8_t c_high = 0b00100000;
  uint8_t c_mid = 0b00000000;
  uint8_t c_low = 0b00010010;
  sendByte(c_high);
  //digitalWrite(MOSI, LOW);
  sendByte(c_mid);
  //digitalWrite(MOSI, LOW);
  sendByte(c_low);
  //digitalWrite(MOSI, LOW);
  digitalWrite(SS, HIGH);

  setControlReg_91_3P();
}

/*
  void updateBuffer(){
  digitalWrite(LDAC, HIGH);
  delayMicroseconds(5);
  digitalWrite(LDAC, LOW);
  }
*/
void sendByte(uint8_t value) {
  int i = 0;
  for (i; i < 8; i++) {
    digitalWrite(SCK, HIGH);
    if ((value & 0x80) == 0x80) {
      digitalWrite(MOSI, HIGH);
      //delay(16);
      //Serial.println("y");
    } else {
      digitalWrite(MOSI, LOW);
      //delay(16);
      //Serial.println("n");
    }
    //delayMicroseconds(16);
    value = value << 1;
    digitalWrite(SCK, LOW);
    //delayMicroseconds(16);
  }
}



boolean getLedState()
{
  boolean state = false;
  if (inputString.substring(5, 7).equals("ON"))
  {
    state = true;
  } else
  {
    state = false;
  }
  return state;
}

void getCommand()
{
  if (inputString.length() > 0)
  {
    commandString = inputString.substring(1, 5);
  }
}

void turnLedOn(int pin)
{
  digitalWrite(pin, HIGH);
}

void turnLedOff(int pin)
{
  digitalWrite(pin, LOW);
}

String getTextNumber()
{
  String value = inputString.substring(5, inputString.length() - 1);
  recievedString = value;
  return value;
}

void sendSerialFloat(float myfloat, Stream& Serialx) {
  Serialx.write((const char*)& myfloat, sizeof(float));
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}



void sendVoltageDACRegister_3P(double voltage) {

  uint32_t dataStream = ((voltage / 300) * (1048575)) / (V_refp - V_refn);
  Serial.println(dataStream, BIN);

  dataStream = (dataStream) & (0xfffff);
  Serial.println(dataStream, BIN);

  uint32_t signalStream = ((WRITE_DAC_REGISTER_INTIALIZE & 0B00001111) << 20) |  dataStream;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_3P, LOW);
  digitalWrite(LDAC_91_3P, LOW);

  sendyte_3P(first);
  sendyte_3P(second);
  sendyte_3P(third);

  digitalWrite(SS_91_3P, HIGH);
  digitalWrite(LDAC_91_3P, HIGH);

}

void setControlReg_91_3P() {

  uint32_t signalStream = WRITE_CONTROL_REGISTER_INTIALIZE;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_3P, LOW);
  digitalWrite(LDAC_91_3P, LOW);

  sendyte_3P(first);
  sendyte_3P(second);
  sendyte_3P(third);

  digitalWrite(SS_91_3P, HIGH);
  digitalWrite(LDAC_91_3P, HIGH);

}



void sendyte_3P(uint8_t stream) {
  int i = 0;
  for (i; i < 8; i++) {
    digitalWrite(SCK_91_3P, HIGH);
    if ((stream & 0x80) == 0x80) {
      digitalWrite(MOSI_91_3P, HIGH);

    } else {
      digitalWrite(MOSI_91_3P, LOW);
    }
    stream = stream << 1;
    digitalWrite(SCK_91_3P, LOW);
  }
}








//6N


void sendVoltageDACRegister_6N(double voltage) {

  uint32_t dataStream = ((voltage / 600) * (1048575)) / (V_refp - V_refn);
  Serial.println(dataStream, BIN);

  dataStream = (dataStream) & (0xfffff);
  Serial.println(dataStream, BIN);

  uint32_t signalStream = ((WRITE_DAC_REGISTER_INTIALIZE & 0B00001111) << 20) |  dataStream;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_6N, LOW);
  digitalWrite(LDAC_91_6N, LOW);

  sendyte_6N(first);
  sendyte_6N(second);
  sendyte_6N(third);

  digitalWrite(SS_91_6N, HIGH);
  digitalWrite(LDAC_91_6N, HIGH);

}

void setControlReg_91_6N() {

  uint32_t signalStream = WRITE_CONTROL_REGISTER_INTIALIZE;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_6N, LOW);
  digitalWrite(LDAC_91_6N, LOW);

  sendyte_6N(first);
  sendyte_6N(second);
  sendyte_6N(third);

  digitalWrite(SS_91_6N, HIGH);
  digitalWrite(LDAC_91_6N, HIGH);

}



void sendyte_6N(uint8_t stream) {
  int i = 0;
  for (i; i < 8; i++) {
    digitalWrite(SCK_91_6N, HIGH);
    if ((stream & 0x80) == 0x80) {
      digitalWrite(MOSI_91_6N, HIGH);

    } else {
      digitalWrite(MOSI_91_6N, LOW);
    }
    stream = stream << 1;
    digitalWrite(SCK_91_6N, LOW);
  }
}




//6N_2


void sendVoltageDACRegister_6N_2(double voltage) {

  uint32_t dataStream = ((voltage / 600) * (1048575)) / (V_refp - V_refn);
  Serial.println(dataStream, BIN);

  dataStream = (dataStream) & (0xfffff);
  Serial.println(dataStream, BIN);

  uint32_t signalStream = ((WRITE_DAC_REGISTER_INTIALIZE & 0B00001111) << 20) |  dataStream;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_6N_2, LOW);
  digitalWrite(LDAC_91_6N_2, LOW);

  sendyte_6N_2(first);
  sendyte_6N_2(second);
  sendyte_6N_2(third);

  digitalWrite(SS_91_6N_2, HIGH);
  digitalWrite(LDAC_91_6N_2, HIGH);

}

void setControlReg_91_6N_2() {

  uint32_t signalStream = WRITE_CONTROL_REGISTER_INTIALIZE;
  Serial.println(signalStream, BIN);

  uint8_t first =   (uint8_t)((signalStream & 0B111111110000000000000000) >> 16); //mask and extract first 8 bits
  Serial.println(first, BIN);
  uint8_t second =  (uint8_t)((signalStream & 0B000000001111111100000000) >> 8);  //mask and extract second 8 bits
  Serial.println(second, BIN);
  uint8_t third =   (uint8_t)((signalStream & 0B000000000000000011111111));       //mask and extract third 8 bits
  Serial.println(third, BIN);


  digitalWrite(SS_91_6N_2, LOW);
  digitalWrite(LDAC_91_6N_2, LOW);

  sendyte_6N_2(first);
  sendyte_6N_2(second);
  sendyte_6N_2(third);

  digitalWrite(SS_91_6N_2, HIGH);
  digitalWrite(LDAC_91_6N_2, HIGH);

}



void sendyte_6N_2(uint8_t stream) {
  int i = 0;
  for (i; i < 8; i++) {
    digitalWrite(SCK_91_6N_2, HIGH);
    if ((stream & 0x80) == 0x80) {
      digitalWrite(MOSI_91_6N_2, HIGH);

    } else {
      digitalWrite(MOSI_91_6N_2, LOW);
    }
    stream = stream << 1;
    digitalWrite(SCK_91_6N_2, LOW);
  }
}
