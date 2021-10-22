#include <FS.h>
#ifdef ESP32
#include <SPIFFS.h>
#endif

#include "esp-efsdb.h"

// #define USE_SD

const char *db_name = "/log.db";

#ifdef USE_SD
#include "SD.h"
#include "SPI.h"

//define SD card pins, default are //SCLK = 18, MISO = 19, MOSI = 23, SS = 5
//All pins must be defined, otherwise considered default PINS
//also refer wESP pins. you can find it in arduino folder
//#define SD_SPI_SCLK_PIN             18 //14
//#define SD_SPI_MISO_PIN             35 //39
//#define SD_SPI_MOSI_PIN             23 //13
//#define SD_SPI_CS_PIN               5 //32

ESPEFSDB myLog(SD, db_name);
#else
ESPEFSDB myLog(SPIFFS, db_name);
#endif

typedef unsigned long int UNIX_TIME;
typedef unsigned int PARAMVALUE;
typedef char RECORD_NO[5];

typedef union
{
  unsigned char Array[sizeof(UNIX_TIME) + sizeof(PARAMVALUE) + sizeof(RECORD_NO)];
  struct
  {
    RECORD_NO logNo;
    UNIX_TIME logDT;
    PARAMVALUE logPV;
  };
} TRANSACTION;

TRANSACTION trans;
unsigned int numLog = 0;
unsigned int logcount = 0;

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void printMenu(void)
{
  Serial.println("===Menu===");
  Serial.println("1: create record");
  Serial.println("2: Read particular record");
  Serial.println("3: read first record and delete it");
  Serial.println("4: clear all record");
  Serial.println("5: total unread records");
  Serial.println("M: Menu");
}

void setup()
{
  // -------------------------------------------------------------------------
  // Basic modules, will always run
  // -------------------------------------------------------------------------

  // Serial debug
  Serial.begin(115200);

  delay(500);

  Serial.printf("ESP-EFSDB\n");
  Serial.printf("%s %s\n", __DATE__, __TIME__);

  delay(5000);

#ifndef USE_SD
  if (!SPIFFS.begin())
  {
    if (!SPIFFS.begin(true))
    {
      Serial.printf("SPIFFS not init\n");
    }
  }

  Serial.printf("[INIT] Flash size (SDK):  %8u bytes\n", ESP.getFlashChipSize());
  Serial.printf("[INIT] Firmware size:     %8u bytes\n", ESP.getSketchSize());
  Serial.printf("[INIT] OTA size:          %8u bytes\n", ESP.getFreeSketchSpace());

  Serial.printf("[MAIN] SPIFFS total size   : %8u bytes\n", SPIFFS.totalBytes());
  Serial.printf("[MAIN]        used size    : %8u bytes\n", SPIFFS.usedBytes());
  File root = SPIFFS.open("/");

  File file = root.openNextFile();

  while (file)
  {
    Serial.printf("[MAIN] FILE   : %s, Size: %8u bytes\n", file.name(), file.size());
    file = root.openNextFile();
  }
  Serial.printf(PSTR("\n"));
#endif

#ifdef USE_SD
  //initialise vspi with default pins
  //SCLK = 18, MISO = 19, MOSI = 23, SS = 5
  //vspi->begin(25, 26, 27, 32);
#if defined(SD_SPI_SCLK_PIN) && defined(SD_SPI_MISO_PIN) && defined(SD_SPI_MOSI_PIN) && defined(SD_SPI_CS_PIN)
  SPI1.begin(SD_SPI_SCLK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  if (!SD.begin(SD_SPI_CS_PIN, SPI1))
  {
    Serial.print("[SD] Mount Failed\n");
  }
#else
  //init with default SPI pins, SCLK = 18, MISO = 19, MOSI = 23, SS = 5
  if (!SD.begin())
  {
    Serial.print("[SD] Mount Failed\n");
  }
#endif
#endif

  if (!myLog.begin())
  {
    Serial.println("file system need to format...");
  }

  myLog.setRecordSize(sizeof(TRANSACTION));
  //myLog.setMaxRecordLimit(5); // max records
  myLog.setFileSizeLimit(102400); //100KBytes
  myLog.setRollOver(false);       //true: roll over records when memory full

  Serial.printf("One Record Size: %d, Max Records= %d\n", myLog.getRecordSize(), myLog.getMaxRecordLimit());

  printMenu();

  pinMode(0, OUTPUT);
}

void loop()
{
  // uint8_t hbLedState = 0;
  static uint32_t _hdledTick = 0;
  // put your main code here, to run repeatedly:

  if (millis() - _hdledTick > 500)
  {
    digitalWrite(0, !(digitalRead(0)));
    _hdledTick = millis();
  }

#if 1
  if (Serial.available())
  {
    char ch = Serial.read();
    switch (ch)
    {
    case '1': //create a log
      numLog++;
      sprintf(trans.logNo, "%d", numLog);
      trans.logDT = millis();
      trans.logPV = random(1, 125);

      if (myLog.push(trans.Array))
      {
        Serial.println("log pushed");
      }
      else
      {
        Serial.println("log push error");
      }
      break;

    case '2': //read a specific log
      Serial.println("\nEnter record number to be read: ");
      while (!Serial.available())
      {
        ;
      }
      if (Serial.available())
      {
        ch = Serial.read();
      }

      if (myLog.get(trans.Array, ch - '0'))
      {
        Serial.println("\nlog read");
        Serial.print(trans.logNo);
        Serial.print(',');
        Serial.print(trans.logDT, DEC);
        Serial.print(',');
        Serial.print(trans.logPV, DEC);
        Serial.print('\n');
      }
      else
      {
        Serial.println("log read error");
      }
      break;

    case '3': //read and delete first log
      if (myLog.pop(trans.Array))
      {
        Serial.println("\nlog poped");
        Serial.print(trans.logNo);
        Serial.print(',');
        Serial.print(trans.logDT);
        Serial.print(',');
        Serial.print(trans.logPV);
        Serial.print('\n');
      }
      else
      {
        Serial.println("log pop error");
      }
      break;

    case '4': //clear all logs
      if (myLog.clear())
      {
        Serial.println("log clear");
      }
      else
      {
        Serial.println("log clear error");
      }
      break;

    case '5': //get number of logs
      myLog.count(&logcount);
      Serial.print("Total logs: ");
      Serial.println(logcount);
      break;

    default:
      printMenu();
      break;
    }
  }
#endif
}
