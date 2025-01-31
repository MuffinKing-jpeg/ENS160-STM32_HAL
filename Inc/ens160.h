#ifndef __ENS160_H__
#define __ENS160_H__

#include <stdint.h>
#include "i2c.h"

// Use this to enable debuging function for reading ALL registers. Memory hungry.
// #define ENS160_ENABLE_FULL_READ

// Address select. Define ENS160_ADDR_PIN_HIGH if your pin (MOSI/ADDR) is high on start
#ifdef ENS160_ADDR_PIN_HIGH
#define ENS160_ADDRESS (0x53 << 1)
#else
#define ENS160_ADDRESS (0x52 << 1)
#endif

// ENS160 Register Map
#define ENS160_REG_PART_ID 0x00     // [2 Bytes] Device Identity (Read-Only)
#define ENS160_REG_OPMODE 0x10      // [1 Byte] Operating Mode (Read/Write)
#define ENS160_REG_CONFIG 0x11      // [1 Byte] Interrupt Pin Configuration (Read/Write)
#define ENS160_REG_COMMAND 0x12     // [1 Byte] Additional System Commands (Read/Write)
#define ENS160_REG_TEMP_IN 0x13     // [2 Bytes] Host Ambient Temperature Input (Read/Write)
#define ENS160_REG_RH_IN 0x15       // [2 Bytes] Host Relative Humidity Input (Read/Write)
#define ENS160_REG_DATA_STATUS 0x20 // [1 Byte] Status of Sensor Data (Read-Only)
#define ENS160_REG_DATA_AQI 0x21    // [1 Byte] Air Quality Index (Read-Only)
#define ENS160_REG_DATA_TVOC 0x22   // [2 Bytes] TVOC Concentration in ppb (Read-Only)
#define ENS160_REG_DATA_ECO2 0x24   // [2 Bytes] Equivalent CO2 Concentration in ppm (Read-Only)
#define ENS160_REG_DATA_T 0x30      // [2 Bytes] Temperature Used in Calculations (Read-Only)
#define ENS160_REG_DATA_RH 0x32     // [2 Bytes] Relative Humidity Used in Calculations (Read-Only)
#define ENS160_REG_DATA_MISR 0x38   // [1 Byte] Data Integrity Checksum (Read-Only)
#define ENS160_REG_GPR_WRITE 0x40   // [8 Bytes] General Purpose Write Registers (Read/Write)
#define ENS160_REG_GPR_READ 0x48    // [8 Bytes] General Purpose Read Registers (Read-Only)

// ENS160 Part ID
#define ENS160_PART_ID_LSB 0x60
#define ENS160_PART_ID_MSB 0x01

// Conversion macro for temperature
#define ENS160_CONVERT_TEMP(TEMP) (uint16_t)(((TEMP) + 273.15f) * 64.0f)

// Conversion macro for RH
#define ENS160_CONVERT_RH(RH) (uint16_t)((RH) * 512.0f)

#define ENS160_DEFAULT_TEMP_F 25.0f
// Default temperature value for 25C
// #define ENS160_DEFAULT_TEMP_U (uint16_t)((ENS160_DEFAULT_TEMP_F + 273.15f) * 64.0f)

#define ENS160_DEFAULT_RH_F 50.0f
// Default relative humidity value for 50%
// #define ENS160_DEFAULT_RH_U (uint16_t)(ENS160_DEFAULT_RH_F * 512.0f)

// // Bit Masks for Registers
// #define ENS160_OPMODE_DEEP_SLEEP    0x00  // Deep Sleep Mode
// #define ENS160_OPMODE_IDLE          0x01  // Idle Mode
// #define ENS160_OPMODE_STANDARD      0x02  // Standard Gas Sensing Mode

#define ENS160_CONFIG_INTPOL (1 << 6)  // INTn Pin Polarity: 0 = Active Low, 1 = Active High
#define ENS160_CONFIG_INT_CFG (1 << 5) // INTn Pin Drive: 0 = Open Drain, 1 = Push-Pull
#define ENS160_CONFIG_INTGPR (1 << 3)  // INTn Pin Asserted on GPR Read Update
#define ENS160_CONFIG_INTDAT (1 << 1)  // INTn Pin Asserted on Data Register Update
#define ENS160_CONFIG_INTEN (1 << 0)   // INTn Pin Enable

#define ENS160_STATUS_MODE (1 << 7)     // OPMODE Running Indicator
#define ENS160_STATUS_ERR (1 << 6)      // Error Indicator
#define ENS160_STATUS_VALIDITY (3 << 2) // Data Validity Flag
#define ENS160_STATUS_NEWDAT (1 << 1)   // New Data Available
#define ENS160_STATUS_NEWGPR (1 << 0)   // New GPR Data Available

 typedef enum 
{
    ENS160_OPMODE_DEEP_SLEEP, // Deep Sleep Mode
    ENS160_OPMODE_IDLE,       // Idle Mode
    ENS160_OPMODE_STANDARD,   // Standard Gas Sensing Mode
}ENS160_OPMODE;

typedef enum 
{
    ENS160_OK,           // All clear
    ENS160_NO_ANSWER,    // Communication problem
    ENS160_WRONG_DEVICE, // PART_ID register does not match
    ENS160_BUSY,         // Devise is busy
    ENS160_WARM_UP,      // Device in warm-up stage
}ENS160_ErrCode;

typedef struct
{
    I2C_HandleTypeDef   *i2cHandle;   // i2c HAL handler
    uint8_t             status;             // value from 0x20 register
    ENS160_OPMODE       opmode;             // operating mode
    float               temp_in;            // last temperature loaded to sensor
    float               rh_in;              // last relative humidity loaded to sensor
    uint8_t             AQI;                // AQI
    uint16_t            TVOC;               // ??? something, something volatile
    uint16_t            ECO2;               // equivalent CO2
    ENS160_ErrCode      err_code;           // result of last operation
    uint8_t             config;             // Value of config register

} ENS160_DeviceType;

// extern ENS160_DeviceType ENS160_Device;

HAL_StatusTypeDef ENS160_Init(I2C_HandleTypeDef *i2cHandle, ENS160_DeviceType *dev);
HAL_StatusTypeDef ENS160_ReadData(ENS160_DeviceType *dev);
HAL_StatusTypeDef ENS160_ChangeMode(ENS160_DeviceType *dev, ENS160_OPMODE mode);
HAL_StatusTypeDef ENS160_UpdateConfig(ENS160_DeviceType *dev, uint8_t config);
HAL_StatusTypeDef ENS160_UpdateEnvironment(ENS160_DeviceType *dev, float *temp, float *humidity);

// LOW level below
HAL_StatusTypeDef ENS160_WriteRegister(ENS160_DeviceType *dev, uint8_t dev_register, uint8_t *pData, uint16_t size);
HAL_StatusTypeDef ENS160_ReadRegister(ENS160_DeviceType *dev, uint8_t dev_register, uint8_t *pData, uint16_t size);
HAL_StatusTypeDef ENS160_IOResultHandler(HAL_StatusTypeDef result, ENS160_DeviceType *dev);

#ifdef ENS160_ENABLE_FULL_READ

#define ENS160_MEMORY_SIZE 0x48
extern uint8_t ENS160_Memory[ENS160_MEMORY_SIZE];
HAL_StatusTypeDef ENS160_FullRead(ENS160_DeviceType *dev);

#endif

#endif