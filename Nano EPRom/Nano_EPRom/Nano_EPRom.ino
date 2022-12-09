#include <EEPROM.h>

void writeIntArrayIntoEEPROM( char data[], int arraySize)
{
  int addressIndex = 0;
  for (int i = 0; i < arraySize; i++) 
  {
    EEPROM.write(i, data[i]);
  }
}

void readIntArrayFromEEPROM( char data[], int arraySize)
{
  int addressIndex = 0;
  for (int i = 0; i < arraySize; i++)
  {
    data[i] = EEPROM.read(i);
  }
}

void setup() {
  Serial.begin(9600);
  
  const int ARRAY_SIZE = 300;
  char postData[ARRAY_SIZE];
  char cp[ARRAY_SIZE]={'A','B','C'};
  
  writeIntArrayIntoEEPROM(cp, ARRAY_SIZE);
  
  readIntArrayFromEEPROM(postData, ARRAY_SIZE);

  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    Serial.println(postData[i]);
  }
}

void loop() {}
