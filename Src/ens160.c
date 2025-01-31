#include "ens160.h"
#include "i2c.h"

static inline void ENS160_uint16_to_uint8(uint16_t value, uint8_t arr[2])
{
    arr[0] = (uint8_t)(value & 0xFF);
    arr[1] = (uint8_t)((value >> 8) & 0xFF);
}

HAL_StatusTypeDef ENS160_WriteRegister(ENS160_DeviceType *dev, uint8_t dev_register, uint8_t *pData, uint16_t size)
{
    return ENS160_IOResultHandler(HAL_I2C_Mem_Write(dev->i2cHandle, ENS160_ADDRESS, dev_register, I2C_MEMADD_SIZE_8BIT, pData, size, HAL_MAX_DELAY),dev);
}

HAL_StatusTypeDef ENS160_ReadRegister(ENS160_DeviceType *dev, uint8_t dev_register, uint8_t *pData, uint16_t size)
{
    return ENS160_IOResultHandler(HAL_I2C_Mem_Read(dev->i2cHandle, ENS160_ADDRESS, dev_register, I2C_MEMADD_SIZE_8BIT, pData, size, HAL_MAX_DELAY), dev);
}

HAL_StatusTypeDef ENS160_IOResultHandler(HAL_StatusTypeDef result, ENS160_DeviceType *dev) {
    switch (result)
    {
    case HAL_BUSY: 
        dev->err_code = ENS160_BUSY;
        return result;
        break;
    case HAL_TIMEOUT: 
    case HAL_ERROR: 
        dev->err_code = ENS160_NO_ANSWER;
        return result;
        break;   
    default:
        return result;
        break;
    }
}


HAL_StatusTypeDef ENS160_Init(I2C_HandleTypeDef *i2cHandle, ENS160_DeviceType *dev)
{
    // Set device bus
    dev->i2cHandle = i2cHandle;

    // Set default temp
    dev->temp_in = ENS160_DEFAULT_TEMP_F;
    dev->rh_in = ENS160_DEFAULT_RH_F;

    // Set blank values
    dev->AQI = 0xFF;
    dev->TVOC = 0xFFFF;
    dev->ECO2 = 0xFFFF;
    dev->status = 0xFF;

    // Set default default mode
    dev->opmode = ENS160_OPMODE_IDLE;

    /*  Set default config
            INTPOL=1 (Active high)
            INT_CFG=1 (Push-pull)
            INTEN=1 (Enabled)
            INTDAT=1 (Assert on new data)
    */
    dev->config =
        ENS160_CONFIG_INTEN |
        ENS160_CONFIG_INTDAT |
        ENS160_CONFIG_INT_CFG |
        ENS160_CONFIG_INTPOL;

    // Sets error code as undefined
    dev->err_code = 0x80;

    uint8_t part_id[2] = {0};
    if (ENS160_ReadRegister(dev, ENS160_REG_PART_ID, part_id, 2) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (part_id[0] != ENS160_PART_ID_LSB || part_id[1] != ENS160_PART_ID_MSB)
    {
        return HAL_ERROR;
    }

    if (ENS160_UpdateEnvironment(dev, &dev->temp_in, &dev->rh_in) != HAL_OK ||
        ENS160_UpdateConfig(dev, dev->config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    // ENS160_FullRead(dev);
    return HAL_OK;
}

HAL_StatusTypeDef ENS160_UpdateConfig(ENS160_DeviceType *dev, uint8_t config)
{
    dev->config = config;
    return ENS160_WriteRegister(dev, ENS160_REG_CONFIG, &config, 1);
}

HAL_StatusTypeDef ENS160_UpdateEnvironment(ENS160_DeviceType *dev, float *temp, float *humidity)
{
    uint8_t env_data[4] = {0};

    ENS160_uint16_to_uint8(ENS160_CONVERT_TEMP(*temp), env_data);
    ENS160_uint16_to_uint8(ENS160_CONVERT_RH(*humidity), &env_data[2]);

    return ENS160_WriteRegister(dev, ENS160_REG_TEMP_IN, env_data, 4);
}

HAL_StatusTypeDef ENS160_ChangeMode(ENS160_DeviceType *dev, ENS160_OPMODE mode)
{
    dev->opmode = mode;
    return ENS160_WriteRegister(dev, ENS160_REG_OPMODE, &mode, 1);
}

#ifdef ENS160_ENABLE_FULL_READ
uint8_t ENS160_Memory[ENS160_MEMORY_SIZE] = {0};
HAL_StatusTypeDef ENS160_FullRead(ENS160_DeviceType *dev)
{
    return ENS160_ReadRegister(dev, ENS160_REG_PART_ID, ENS160_Memory, ENS160_MEMORY_SIZE);
}
#endif