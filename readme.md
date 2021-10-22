# ESP Embedded File System DataBase (ESP-EFSDB) for ESP Arduino framework

ESP-EFSDB is an Arduino library designed to standardize the basic operations on log files on ESP8266 and ESP32 SoC.
It is build to work on internal Flash and external SPI based Micro SD Card.
This is mainly fixed length type record logger.

## Features
* Methods to monitor log dimension, to avoid collision with other files.
* Support to multiple log files to store different information.
* User can read any records in database.
* The records are stored and recall in First In First Out (FIFO) manner.
* Once the database is full or maximum number of records reached, library overwrites first (oldest) record and so on. 
* If user disable this feature then logging will be stopped when database is full.
* log records are always present in file for backup even if they pop'd. They completely vanished by 'clear()' function.

## Installation
If you desire the lastest Github version, you can download this repo and copy the entire folder in your Arduino Library directory. More info at [Arduino Library](https://www.arduino.cc/en/Guide/Libraries).

## API and Examples
After installing, you can see the examples to understand how easy is mastering this library. For advacend usages (and advanced users), the full library specifications can be found directy in *commented* header (*.h) files.

### Flash based initializaton

* Intialize File System
```
SPIFFS.begin(true);
```
* Intialize database
```
const char *db_name = "edbtest.db";
ESPEFSDB myLog(SPIFFS, db_name);
// configure related parameters
myLog.begin();
myLog.setRecordSize(100); // size of each record
myLog.setMaxRecordLimit(5); // store 5 number of records
// myLog.setFileSizeLimit(102400);  // 100KBytes for database
myLog.setRollOver(false);  // true: roll over records when memory full, false: stop when maximum records reached.
```

### Micro SD Card based initializaton

* Intialize File System
```
SD.begin(); // mount with default SPI pins.
// SPI1.begin(SD_SPI_SCLK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
// SD.begin(SD_SPI_CS_PIN, SPI1); // mount with user assigned pins.
```
* Intialize database
```
const char *db_name = "edbtest.db";
ESPEFSDB myLog(SD, db_name);
// configure related parameters
myLog.begin();
myLog.setRecordSize(100); // size of each record
myLog.setMaxRecordLimit(5);  // store 5 number of records
// myLog.setFileSizeLimit(102400);  // 100KBytes for database
myLog.setRollOver(false);  // true: roll over records when memory full, false: stop when maximum records reached.
```

[![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)](CHANGELOG.md)
[![GitHub commits](https://img.shields.io/github/commits-since/umeshwalkar/esp-efsdb-lib/v1.0.0.svg)](https://github.com/umeshwalkar/esp-efsdb-lib/compare/v1.0.0)
[![GitHub issues](https://img.shields.io/github/issues/umeshwalkar/esp-efsdb-lib.svg)](https://github.com/umeshwalkar/esp-efsdb-lib/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/umeshwalkar/esp-efsdb-lib.svg)](https://github.com/umeshwalkar/esp-efsdb-lib/pulls)
[![codacy](https://img.shields.io/codacy/grade/4ccbea0317c4415eb2d1c562feced407/master.svg)](https://app.codacy.com/manual/umeshwalkar/esp-efsdb-lib/dashboard)
[![license](https://img.shields.io/github/license/umeshwalkar/esp-efsdb-lib.svg)](LICENSE)
<br />
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8HEWFU6AKMTXL&lc=US&no_note=0&currency_code=INR)

