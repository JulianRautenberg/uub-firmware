 /***************************************************************************/
 /*
 * @file xiicps_polled_master_example.c
 *
 * A design example of using the device in polled mode as master.
 *
 * The example uses buffer size 132. Please set the send buffer of the
 * Aardvark device to be continuous 64 bytes from 0x00 to 0x3F.
  *
  **************************************************************************/
 
 /***************************** Include Files ******************************/
 #include "stdio.h"
 #include "xparameters.h"
 #include "xiicps.h"
 #include "new_uub.h"

 /************************** Constant Definitions **************************/
 
 /* The following constants map to the XPAR parameters created in the
  * xparameters.h file. They are defined here such that a user can easily
  * change all the needed parameters in one place. */
 #define IIC_DEVICE_ID    XPAR_XIICPS_1_DEVICE_ID
 
 #define BUF_SIZE                35 // Tansmit and receive buffer size 
 #define RCV_BUF_SIZE           256 // Tansmit and receive buffer size 
 #define nb_initData_SI5347    1560 // Nb init data for SI5347 component

 /**************************** Type Definitions ****************************/
 
 /************************** Variable Definitions **************************/

 static XIicPs Iic;    /* Instance of the IIC Device */
 
 /* The following buffers are used to send and receive data with the IIC. */
 /* Buffer for Transmitting Data */
 static u8 SendBuffer[BUF_SIZE ]    = {0x00,0x20,
                                       0x01,0x01,0xB4,0x01,0x02,0x50,0x40,0x00,
                                       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                       0x00,0x00,0x00,0x00,0x6D,0x02,0x00,0x00,
                                       0x99,0x03,0x32,0x68,0x00,0x40,0x02,0x08};
 static u8 RecvBuffer[RCV_BUF_SIZE] = {0x00}; /* Buffer for Receiving Data */
 
 static u8 SI5347_INIT[nb_initData_SI5347 + 1] = 
  { 0x0B,0x24,0xD8,0x0B,0x25,0x00,                                              // bank 0xB
    0x00,0x0B,0x6C,0x00,0x16,0x0F,0x00,0x17,0x3C,0x00,0x18,0xFF,0x00,0x19,0xFF, // bank 0    
    0x00,0x1A,0xFF,0x00,0x20,0x00,0x00,0x2B,0x0A,0x00,0x2C,0x01,0x00,0x2D,0x01,
    0x00,0x2E,0xE4,0x00,0x2F,0x00,0x00,0x30,0x00,0x00,0x31,0x00,0x00,0x32,0x00,
    0x00,0x33,0x00,0x00,0x34,0x00,0x00,0x35,0x00,0x00,0x36,0xE4,0x00,0x37,0x00,
    0x00,0x38,0x00,0x00,0x39,0x00,0x00,0x3A,0x00,0x00,0x3B,0x00,0x00,0x3C,0x00,
    0x00,0x3D,0x00,0x00,0x3F,0x11,0x00,0x40,0x04,0x00,0x41,0x0D,0x00,0x42,0x00,
    0x00,0x43,0x00,0x00,0x44,0x00,0x00,0x45,0x0C,0x00,0x46,0x32,0x00,0x47,0x00,
    0x00,0x48,0x00,0x00,0x49,0x00,0x00,0x4A,0x32,0x00,0x4B,0x00,0x00,0x4C,0x00,
    0x00,0x4D,0x00,0x00,0x4E,0x05,0x00,0x4F,0x00,0x00,0x51,0x03,0x00,0x52,0x00,
    0x00,0x53,0x00,0x00,0x54,0x00,0x00,0x55,0x03,0x00,0x56,0x00,0x00,0x57,0x00,
    0x00,0x58,0x00,0x00,0x59,0x03,0x00,0x5A,0x00,0x00,0x5B,0x00,0x00,0x5C,0x40,
    0x00,0x5D,0x01,0x00,0x5E,0x00,0x00,0x5F,0x00,0x00,0x60,0x00,0x00,0x61,0x00,
    0x00,0x62,0x00,0x00,0x63,0x00,0x00,0x64,0x00,0x00,0x65,0x00,0x00,0x66,0x00,
    0x00,0x67,0x00,0x00,0x68,0x00,0x00,0x69,0x00,0x00,0x92,0x00,0x00,0x93,0x00,
    0x00,0x94,0x00,0x00,0x95,0x00,0x00,0x96,0x00,0x00,0x97,0x00,0x00,0x98,0x00,
    0x00,0x99,0x00,0x00,0x9A,0x01,0x00,0x9B,0x03,0x00,0x9C,0x00,0x00,0x9D,0x00,
    0x00,0x9E,0x02,0x00,0x9F,0x00,0x00,0xA0,0x00,0x00,0xA1,0x00,0x00,0xA2,0x01,
    0x00,0xA3,0xB8,0x00,0xA4,0x32,0x00,0xA5,0x01,0x00,0xA6,0x00,0x00,0xA7,0x00,
    0x00,0xA8,0x00,0x00,0xA9,0x00,0x00,0xAA,0x00,0x00,0xAB,0x00,0x00,0xAC,0x00,
    0x00,0xAD,0x00,0x00,0xAE,0x00,0x00,0xAF,0x00,0x00,0xB0,0x00,0x00,0xB1,0x00,
    0x00,0xB2,0x00,0x00,0xB3,0x00,0x00,0xB4,0x00,0x00,0xB5,0x00,0x00,0xB6,0x00,
    0x01,0x02,0x01,0x01,0x08,0x02,0x01,0x09,0x09,0x01,0x0A,0x3D,0x01,0x0B,0x00, // bank 1
    0x01,0x0C,0x01,0x01,0x12,0x02,0x01,0x13,0x09,0x01,0x14,0x3D,0x01,0x15,0x00,
    0x01,0x16,0x01,0x01,0x17,0x02,0x01,0x18,0x09,0x01,0x19,0x3D,0x01,0x1A,0x00,
    0x01,0x1B,0x01,0x01,0x1C,0x02,0x01,0x1D,0x09,0x01,0x1E,0x3D,0x01,0x1F,0x00,
    0x01,0x20,0x01,0x01,0x26,0x02,0x01,0x27,0x09,0x01,0x28,0x3D,0x01,0x29,0x00,
    0x01,0x2A,0x01,0x01,0x2B,0x02,0x01,0x2C,0x09,0x01,0x2D,0x3D,0x01,0x2E,0x00,
    0x01,0x2F,0x01,0x01,0x30,0x01,0x01,0x31,0x09,0x01,0x32,0x3B,0x01,0x33,0x00,
    0x01,0x34,0x00,0x01,0x3A,0x01,0x01,0x3B,0x09,0x01,0x3C,0x3B,0x01,0x3D,0x00,
    0x01,0x3E,0x00,0x01,0x3F,0x00,0x01,0x40,0x00,0x01,0x41,0x40,0x01,0x42,0xFF,
    0x02,0x02,0x00,0x02,0x03,0x00,0x02,0x04,0x00,0x02,0x05,0x00,0x02,0x06,0x00, // bank 2
    0x02,0x08,0x3C,0x02,0x09,0x00,0x02,0x0A,0x00,0x02,0x0B,0x00,0x02,0x0C,0x00,
    0x02,0x0D,0x00,0x02,0x0E,0x01,0x02,0x0F,0x00,0x02,0x10,0x00,0x02,0x11,0x00,
    0x02,0x12,0x00,0x02,0x13,0x00,0x02,0x14,0x00,0x02,0x15,0x00,0x02,0x16,0x00,
    0x02,0x17,0x00,0x02,0x18,0x00,0x02,0x19,0x00,0x02,0x1A,0x00,0x02,0x1B,0x00,
    0x02,0x1C,0x00,0x02,0x1D,0x00,0x02,0x1E,0x00,0x02,0x1F,0x00,0x02,0x20,0x00,
    0x02,0x21,0x00,0x02,0x22,0x00,0x02,0x23,0x00,0x02,0x24,0x00,0x02,0x25,0x00,
    0x02,0x26,0x00,0x02,0x27,0x00,0x02,0x28,0x00,0x02,0x29,0x00,0x02,0x2A,0x00,
    0x02,0x2B,0x00,0x02,0x2C,0x00,0x02,0x2D,0x00,0x02,0x2E,0x00,0x02,0x2F,0x00,
    0x02,0x31,0x01,0x02,0x32,0x01,0x02,0x33,0x01,0x02,0x34,0x01,0x02,0x35,0x00,
    0x02,0x36,0x00,0x02,0x37,0x00,0x02,0x38,0x00,0x02,0x39,0x94,0x02,0x3A,0x00,
    0x02,0x3B,0x00,0x02,0x3C,0x00,0x02,0x3D,0x00,0x02,0x3E,0x80,0x02,0x40,0x00,
    0x02,0x41,0x00,0x02,0x42,0x00,0x02,0x43,0x00,0x02,0x44,0x00,0x02,0x45,0x00,
    0x02,0x46,0x00,0x02,0x4A,0x02,0x02,0x4B,0x00,0x02,0x4C,0x00,0x02,0x50,0x02,
    0x02,0x51,0x00,0x02,0x52,0x00,0x02,0x53,0x02,0x02,0x54,0x00,0x02,0x55,0x00,
    0x02,0x56,0x02,0x02,0x57,0x00,0x02,0x58,0x00,0x02,0x5C,0x02,0x02,0x5D,0x00,
    0x02,0x5E,0x00,0x02,0x5F,0x02,0x02,0x60,0x00,0x02,0x61,0x00,0x02,0x62,0x00,
    0x02,0x63,0x00,0x02,0x64,0x00,0x02,0x68,0x00,0x02,0x69,0x00,0x02,0x6A,0x00,
    0x02,0x6B,0x30,0x02,0x6C,0x00,0x02,0x6D,0x00,0x02,0x6E,0x00,0x02,0x6F,0x00,
    0x02,0x70,0x00,0x02,0x71,0x00,0x02,0x72,0x00,
    0x03,0x02,0x00,0x03,0x03,0x00,0x03,0x04,0x00,0x03,0x05,0x80,0x03,0x06,0x12, // bank 3
    0x03,0x07,0x00,0x03,0x08,0x00,0x03,0x09,0x00,0x03,0x0A,0x00,0x03,0x0B,0xF0,
    0x03,0x0D,0x00,0x03,0x0E,0x00,0x03,0x0F,0x00,0x03,0x10,0x80,0x03,0x11,0x00,
    0x03,0x12,0x00,0x03,0x13,0x00,0x03,0x14,0x00,0x03,0x15,0x00,0x03,0x16,0x80,
    0x03,0x18,0x00,0x03,0x19,0x00,0x03,0x1A,0x00,0x03,0x1B,0x80,0x03,0x1C,0x00,
    0x03,0x1D,0x00,0x03,0x1E,0x00,0x03,0x1F,0x00,0x03,0x20,0x00,0x03,0x21,0x80,
    0x03,0x23,0x00,0x03,0x24,0x00,0x03,0x25,0x00,0x03,0x26,0x80,0x03,0x27,0x00,
    0x03,0x28,0x00,0x03,0x29,0x00,0x03,0x2A,0x00,0x03,0x2B,0x00,0x03,0x2C,0x80,
    0x03,0x39,0x00,0x03,0x3B,0x00,0x03,0x3C,0x00,0x03,0x3D,0x00,0x03,0x3E,0x00,
    0x03,0x3F,0x00,0x03,0x40,0x00,0x03,0x41,0x00,0x03,0x42,0x00,0x03,0x43,0x00,
    0x03,0x44,0x00,0x03,0x45,0x00,0x03,0x46,0x00,0x03,0x47,0x00,0x03,0x48,0x00,
    0x03,0x49,0x00,0x03,0x4A,0x00,0x03,0x4B,0x00,0x03,0x4C,0x00,0x03,0x4D,0x00,
    0x03,0x4E,0x00,0x03,0x4F,0x00,0x03,0x50,0x00,0x03,0x51,0x00,0x03,0x52,0x00,
    0x04,0x02,0x01,0x04,0x08,0x10,0x04,0x09,0x22,0x04,0x0A,0x09,0x04,0x0B,0x08, // bank 4
    0x04,0x0C,0x1F,0x04,0x0D,0x3F,0x04,0x0E,0x10,0x04,0x0F,0x24,0x04,0x10,0x09,
    0x04,0x11,0x08,0x04,0x12,0x1F,0x04,0x13,0x3F,0x04,0x15,0x00,0x04,0x16,0x00,
    0x04,0x17,0x00,0x04,0x18,0x00,0x04,0x19,0xB4,0x04,0x1A,0x00,0x04,0x1B,0x00,
    0x04,0x1C,0x00,0x04,0x1D,0x00,0x04,0x1E,0x00,0x04,0x1F,0x80,0x04,0x21,0x31,
    0x04,0x22,0x01,0x04,0x23,0xE3,0x04,0x24,0xA5,0x04,0x25,0x9B,0x04,0x26,0x04,
    0x04,0x27,0x00,0x04,0x28,0x00,0x04,0x29,0x00,0x04,0x2A,0x00,0x04,0x2B,0x01,
    0x04,0x2C,0x0F,0x04,0x2D,0x03,0x04,0x2E,0x15,0x04,0x2F,0x13,0x04,0x31,0x00,
    0x04,0x32,0x42,0x04,0x33,0x03,0x04,0x34,0x00,0x04,0x36,0x0C,0x04,0x37,0x00,
    0x04,0x38,0x01,0x04,0x39,0x00,
    0x05,0x02,0x01,0x05,0x08,0x00,0x05,0x09,0x00,0x05,0x0A,0x00,0x05,0x0B,0x00, // bank 5
    0x05,0x0C,0x00,0x05,0x0D,0x00,0x05,0x0E,0x00,0x05,0x0F,0x00,0x05,0x10,0x00,
    0x05,0x11,0x00,0x05,0x12,0x00,0x05,0x13,0x00,0x05,0x15,0x00,0x05,0x16,0x00,
    0x05,0x17,0x00,0x05,0x18,0x80,0x05,0x19,0x00,0x05,0x1A,0x00,0x05,0x1B,0x00,
    0x05,0x1C,0x00,0x05,0x1D,0x00,0x05,0x1E,0x00,0x05,0x1F,0x80,0x05,0x21,0x21,
    0x05,0x22,0x01,0x05,0x23,0x00,0x05,0x24,0x00,0x05,0x25,0x00,0x05,0x26,0x00,
    0x05,0x27,0x00,0x05,0x28,0x00,0x05,0x29,0x00,0x05,0x2A,0x01,0x05,0x2B,0x01,
    0x05,0x2C,0x0F,0x05,0x2D,0x03,0x05,0x2E,0x00,0x05,0x2F,0x00,0x05,0x31,0x00,
    0x05,0x32,0x00,0x05,0x33,0x04,0x05,0x34,0x00,0x05,0x36,0x0E,0x05,0x37,0x00,
    0x05,0x38,0x00,0x05,0x39,0x00,
    0x06,0x02,0x01,0x06,0x08,0x00,0x06,0x09,0x00,0x06,0x0A,0x00,0x06,0x0B,0x00, // bank 6
    0x06,0x0C,0x00,0x06,0x0D,0x00,0x06,0x0E,0x00,0x06,0x0F,0x00,0x06,0x10,0x00,
    0x06,0x11,0x00,0x06,0x12,0x00,0x06,0x13,0x00,0x06,0x15,0x00,0x06,0x16,0x00,
    0x06,0x17,0x00,0x06,0x18,0x80,0x06,0x19,0x00,0x06,0x1A,0x00,0x06,0x1B,0x00,
    0x06,0x1C,0x00,0x06,0x1D,0x00,0x06,0x1E,0x00,0x06,0x1F,0x80,0x06,0x21,0x21,
    0x06,0x22,0x01,0x06,0x23,0x00,0x06,0x24,0x00,0x06,0x25,0x00,0x06,0x26,0x00,
    0x06,0x27,0x00,0x06,0x28,0x00,0x06,0x29,0x00,0x06,0x2A,0x00,0x06,0x2B,0x01,
    0x06,0x2C,0x0F,0x06,0x2D,0x03,0x06,0x2E,0x00,0x06,0x2F,0x00,0x06,0x31,0x00,
    0x06,0x32,0x00,0x06,0x33,0x04,0x06,0x34,0x00,0x06,0x36,0x0E,0x06,0x37,0x00,
    0x06,0x38,0x00,0x06,0x39,0x00,
    0x07,0x02,0x01,0x07,0x09,0x00,0x07,0x0A,0x00,0x07,0x0B,0x00,0x07,0x0C,0x00, // bank 7
    0x07,0x0D,0x00,0x07,0x0E,0x00,0x07,0x0F,0x00,0x07,0x10,0x00,0x07,0x11,0x00,
    0x07,0x12,0x00,0x07,0x13,0x00,0x07,0x14,0x00,0x07,0x16,0x00,0x07,0x17,0x00,
    0x07,0x18,0x00,0x07,0x19,0x80,0x07,0x1A,0x00,0x07,0x1B,0x00,0x07,0x1C,0x00,
    0x07,0x1D,0x00,0x07,0x1E,0x00,0x07,0x1F,0x00,0x07,0x20,0x80,0x07,0x22,0x21,
    0x07,0x23,0x01,0x07,0x24,0x00,0x07,0x25,0x00,0x07,0x26,0x00,0x07,0x27,0x00,
    0x07,0x28,0x00,0x07,0x29,0x00,0x07,0x2A,0x00,0x07,0x2B,0x00,0x07,0x2C,0x01,
    0x07,0x2D,0x0F,0x07,0x2E,0x03,0x07,0x2F,0x00,0x07,0x30,0x00,0x07,0x32,0x00,
    0x07,0x33,0x00,0x07,0x34,0x04,0x07,0x35,0x00,0x07,0x37,0x0E,0x07,0x38,0x00,
    0x07,0x39,0x00,0x07,0x3A,0x00,
    0x09,0x0E,0x02,0x09,0x43,0x00,0x09,0x49,0x01,0x09,0x4A,0x01,                // bank 9
    0x0A,0x03,0x01,0x0A,0x04,0x00,0x0A,0x05,0x01,                               // bank A
    0x0B,0x44,0xEF,0x0B,0x45,0x0E,0x0B,0x46,0x00,0x0B,0x47,0x00,0x0B,0x48,0x0E,
    0x0B,0x4A,0x0E,                                                             // bank B
    0x04,0x14,0x01,                                                             // bank 4
    0x00,0x1C,0x01,                                                             // bank 0
    0x0B,0x24,0xDB,0x0B,0x25,0x02};                                             // bank B

 /*************************************************************************
 *
 * This function sends data and expects to receive data from slave as modular
 * of 64.
 *
 * This function uses interrupt-driven mode of the device.
 *
 * @param DeviceId is the Device ID of the IicPs Device and is the
 *    XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
 *
 * @return  XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note    None.
 *
 **************************************************************************/

 int IicPsInitClockGenerator(u16 DeviceId)
 {
  int Status;
  XIicPs_Config *Config;

  /* Initialize the IIC driver so that it's ready to use Look up the 
   * configuration in the config table, then initialize it. */
  Config = XIicPs_LookupConfig(DeviceId);
  if (NULL == Config) { return XST_FAILURE; }
 
  Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
  if (Status != XST_SUCCESS) { return XST_FAILURE; }
 
  /* Perform a self-test to ensure that the hardware was built correctly. */
  Status = XIicPs_SelfTest(&Iic);
  if (Status != XST_SUCCESS) { return XST_FAILURE; }
 
  /* Set the IIC serial clock rate. */
  XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
 
  while (XIicPs_BusIsBusy(&Iic)) { /* NOP */ }

  Status = XIicPs_MasterSendPolled(&Iic, SendBuffer,34, IIC_SLAVE_CDCEL913);
  if (Status == XST_SUCCESS) { return XST_SUCCESS; } else {return  XST_FAILURE;}
}

  //************************************************************************  
  //                  initialisation du Cleaner jitter                     *
  //************************************************************************  
  
 int IicPsInitJitterCleaner(u16 DeviceId)
 {
  int Status, k;
  
   // test if cleaner jitter component is ready 
  SendBuffer[0] = 0xFE;  // l'infromation READY est � l'@ 0xFE
  XIicPs_MasterSendPolled(&Iic, SendBuffer,1, IIC_SLAVE_SI5347);  // initialisation de l'@ de depart

  Status = IsJitterCleanerReady;
  if (Status == XST_FAILURE) { return XST_FAILURE;}// the component is never ready, return with error
   
  // register initialization for accessing to the Page 0x0 to make hard RESET 
   
  while (XIicPs_BusIsBusy(&Iic)) { /* NOP */ }     // SI5347 component ready ?
  SendBuffer[0] = 0x01;
  SendBuffer[1] = 0x00;
  XIicPs_MasterSendPolled(&Iic, SendBuffer,2, IIC_SLAVE_SI5347); // init page number to 0
  SendBuffer[0] = 0x1E;
  SendBuffer[1] = 0x02;
  XIicPs_MasterSendPolled(&Iic, SendBuffer,2, IIC_SLAVE_SI5347); // Hard reset generated

 // wait until the SI5347 component ready flag is ON
  Status = IsJitterCleanerReady();
  if (Status == XST_FAILURE) {return XST_FAILURE;} // the component is never ready, return with error
  
  /* Send all the init configuration data to the SI5347 component register */
  
  for ( k = 0; k < nb_initData_SI5347; k=k+3)
    {
      while (XIicPs_BusIsBusy(&Iic)) { /* NOP */}          // I2C bus ready ?
      SendBuffer[0] = 0x01;
      SendBuffer[1] = SI5347_INIT[k];                      // init page number
      XIicPs_MasterSendPolled(&Iic, SendBuffer,2, IIC_SLAVE_SI5347);
      SendBuffer[0] = SI5347_INIT[k+1];
      SendBuffer[1] = SI5347_INIT[k+2];                    // send dat register
      XIicPs_MasterSendPolled(&Iic, SendBuffer,2, IIC_SLAVE_SI5347);
    }
  
//   ReadJitterCleanerValue();

  // all the initialization are made, the test is passed
   return XST_SUCCESS;
 }
    
    
  /***************************************************************/
  /* Read the Jitter Clearner internal register and display them */
  /***************************************************************/

 int ReadJitterCleanerValue(void)
 {
   int i,j,k;
   int data_display;
   unsigned int *state_register;
   
   for (i = 0; i<258; i++) { RecvBuffer[i] = 0;} // set the receiver buffer to 0
   
   for (j =0x0; j<0xc; j++)  // read the internal register bank by bank
   {
     SendBuffer[0] = 0x01;
     SendBuffer[1] = j;
     while (XIicPs_BusIsBusy(&Iic)) { /* NOP */}        // I2C bus ready ?
     XIicPs_MasterSendPolled(&Iic, SendBuffer,2, IIC_SLAVE_SI5347);  // to bank x
     SendBuffer[0] = 0;
     while (XIicPs_BusIsBusy(&Iic)) { /* NOP */}        // I2C bus ready ?
     XIicPs_MasterSendPolled(&Iic, SendBuffer,1, IIC_SLAVE_SI5347);  // beginning read @
     while (XIicPs_BusIsBusy(&Iic)) { /* NOP */}         // I2C bus ready ?
     XIicPs_MasterRecvPolled(&Iic, RecvBuffer,256, IIC_SLAVE_SI5347);  // read bank x
  
     printf("\r\n Valeur page %x \r\n",j);
     for (i = 0; i<255; i=i+16)
     {
       for (k=0; k<16;k++) { printf(" %2x", RecvBuffer[i+k]);}
       print(" \r\n");
     }
   }
    
   // Wait the all switches go to ON ( Low level )
   state_register = UUB_STATE_REGISTER;
   data_display = *state_register;
   while( (data_display & 0xff) != 0x0 ) { data_display = *state_register;}
   return XST_SUCCESS;
 }

 //********************************************
 //*    Is Jitter Cleaner Ready ?
 //********************************************
 
 int IsJitterCleanerReady(void)
 {
   int k;
   
   for (k=0; k<1000; k++)
    {
      XIicPs_MasterRecvPolled(&Iic,RecvBuffer,1,IIC_SLAVE_SI5347);  // read data
      if (RecvBuffer[0] == 0x0F) { return XST_SUCCESS; }
    }
   return XST_FAILURE;
}