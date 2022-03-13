#ifndef ESP_EFSDB_H
#define ESP_EFSDB_H

#include <Arduino.h>
#include <fs.h>
#include "HardwareSerial.h"

#if !defined(ESP8266) && !defined(ESP32)
#error "Only ESP8266 and ESP32 architecture supported!!!"
#endif

/**
 * various error code.
 */
#define EFSDB_ERR_NONE 0                  // all ok
#define EFSDB_ERR_FILENAME_NOT_PROVIDED 1 // file name not provided while init
#define EFSDB_ERR_RECORDSIZE_PROVIDED 2   // one record size not provided
#define EFSDB_ERR_MAXRECORDS_PROVIDED 3   // max record count not provided
#define EFSDB_ERR_FS_NOT_MOUNTED 4        // NOT APPLICABLE, FS shall be mount from main application.
#define EFSDB_ERR_FILE_NOT_FOUND 5        // database file not found in FS
#define EFSDB_ERR_FILE_NOT_OPENED 6       // database file not opened for read/write
#define EFSDB_ERR_FILE_NOT_SEEKED 7       // moving file pointer within file failed
#define EFSDB_ERR_LESS_BYTES_WRITE 8      // read bytes count mismatched
#define EFSDB_ERR_LESS_BYTES_READ 9       // written byte count mismatched
#define EFSDB_ERR_PACKET_TERMINATION 10   // packet termination while packet inserting failed
#define EFSDB_ERR_DATABSE_FULL 11         // databse full
#define EFSDB_ERR_NO_LOG 12               // no unread log
#define EFSDB_ERR_INVALID_ARG 13          // argument passed to function is invalid
#define EFSDB_ERR_INITIALIZATION 14       // error while lib init

/**
 * structure to hold log info.
 */
typedef union
{
    uint8_t arr[sizeof(uint32_t) * 5];
    struct
    {
        uint32_t current;    /* holds the current position of un-poped log */
        uint32_t start;      /* holds the start position of un-poped log */
        uint32_t end;        /* holds the end position of un-poped log */
        uint32_t recordSize; /* holds one record size in logs. */
        uint32_t autoNumber; /* incrementing each time log is pushed. */
    };
} log_info_t;

/**
 * This class is responsible for a log file.
 */
class ESPEFSDB
{
public:
    /**
        @fn ESPEFSDB(fs::FS &fs, const char *file = NULL)
        @brief This is constructor.
        @param fs::FS &fs, pointer SPIFFS or SD file sile system.
        @param const char *file,  pointr to file name.
        @remark define the constructor at top of the pallication. hold the filename throughout the operations.
    */
    ESPEFSDB(fs::FS &fs, const char *file = NULL);

    /**
        @fn ~ESPEFSDB();
        @brief This is de-constructor.
        @remark calling closing procedures.
    */
    ~ESPEFSDB();

    /**
        @fn bool begin()
        @brief This function initiates logger
        @return true-success, false-fail
        @remark Start the Filesystem and then call this function before starting calling anyother operations.
    */
    bool begin();

    /**
        @fn bool count(unsigned int *pTotalLogs)
        @brief This function tells number of unread records in file
        @param unsigned int *pTotalLogs, a pointer to number of records
        @return true-success, false-fail
    */
    bool count(unsigned int *pTotalLogs);

    /**
        @fn bool clear(void)
        @brief This function clears all records from file and start from fresh
        @return true-success, false-fail
        @remark file is deleted and created again. so all previous records will be lost.
    */
    bool clear();

    /**
        @fn bool flush(unsigned int logCounts)
        @brief This function resets data header (unsend count) from file and skips "logCounts" records from queue.
        @return true-success, false-fail
        @remark this will help to skip some/all unsend records in queue without deleting them or clearing entire file.
    */
    bool flush(unsigned int logCounts);

    /**
        @fn bool push(const unsigned char *data)
        @brief This function appends record at end.
        @param const unsigned char *data, a pointer to buffer which holding record to be store
        @return true-success, false-fail
        @remark
    */
    bool push(const unsigned char *data);

    /**
        @fn bool pop(unsigned char *data)
        @brief This function fetches the first unread record and deletes it's entry
        @param unsigned char *data, a pointer to buffer to copy read record
        @return true-success, false-fail
        @remark
    */
    bool pop(unsigned char *data);

    /**
        @fn bool get(unsigned char *data, unsigned int logNum)
        @brief This function fetches the said record. but not deletes it's entry
        @param unsigned char *data, a pointer to buffer to copy read record
        @param unsigned int logNum, the log record number to be fetch
        @return true-success, false-fail
        @remark logNum==>'0' is first unread log
    */
    bool get(unsigned char *data, unsigned int logNum);

    /**
        @fn unsigned int getRecordSize(void)
        @brief This function returns one record size in bytes
        @return one record size in bytes
        @remark
    */
    unsigned int getRecordSize(void);

    /**
        @fn void setRecordSize(unsigned int recordSize)
        @brief This function sets one record size in bytes
        @param unsigned int recordSize, is record size in bytes
        @remark
    */
    void setRecordSize(unsigned int recordSize);

    /**
        @fn unsigned int getMaxRecordLimit(void)
        @brief This function returns the number of maximum records.
        @return returns maximum number of possible records from given filesize
        @remark
    */
    unsigned int getMaxRecordLimit(void);

    /**
        @fn void setMaxRecordLimit(unsigned int maxRecords)
        @brief This function sets maximum number of records to be handle
        @param unsigned int maxRecords, is number of maximum records
        @remark if this is used then no need to use setFileSizeLimit function
    */
    void setMaxRecordLimit(unsigned int maxRecords);

    /**
        @fn void setFileSizeLimit(unsigned int fileSizeLimit)
        @brief This function sets databse file size in bytes
        @param unsigned int fileSizeLimit, is filesize in bytes
        @remark if this is used then no need to use setMaxRecordLimit function
    */
    void setFileSizeLimit(unsigned int fileSizeLimit);

    /**
        @fn void setRollOver(bool rollover)
        @brief This function enable/disable rollover feature
        @param bool rollover, is set true to enable feature.
        @remark if enabled, then logging will be stoped when maximum records reached.
    */
    void setRollOver(bool rollover);

    /**
        @fn bool getLogHeader(log_info_t *logInfo)
        @brief This function fetches the log header.
        @param log_info_t *logInfo a pointer to log header
        @return true-success, false-fail
        @remark consider logInfo valid if this function returs true.
    */
    bool getLogHeader(log_info_t *logInfo);

    /**
        @fn int getLastError()
        @brief This function returns error code of last operation.
        @return integer error code.
        @remark
    */
    int getLastError();

    /**
        @fn void setDebugPort(HardwareSerial *SerialPort, bool debugEnable = false)
        @brief This function takes serial port.
        @param HardwareSerial *SerialPort pointer to Serial port
        @param bool debugEnable, true = enabled, false = disabled
        @remark
    */
    void setDebugPort(HardwareSerial *SerialPort, bool debugEnable = false);

    /**
        @fn void setDebug(bool debugEnable)
        @brief This function takes serial port.
        @param bool debugEnable, true = enabled, false = disabled
        @remark
    */
    void setDebug(bool debugEnable);

protected:
    /**
        Pointer to File System
    */
    fs::FS &_fs;

    /**
        Actual log file name
    */
    const char *_filePath;

    /**
        size of one record
    */
    unsigned int _recordSize;

    /**
        size of total number of rocords
    */
    unsigned int _maxRecords;

    /**
        size of log file in bytes
    */
    unsigned long _fileSizeLimit;

    /**
        stop logging after data base full or rollover
    */
    bool _rollOver;

    /**
        set when all required settings are done
    */
    unsigned char _efsdbInitDone;

    /**
        set when reached to maxRecords
    */
    unsigned char _dbFull;

    /**
        last operation error code
    */
    unsigned char _errorCode;

    /**
        debug serial port
    */
    HardwareSerial *_debugPort;

    /**
        debug enable
    */
    bool _debugEnabled;
};

#endif // ESP_EFSDB_H
