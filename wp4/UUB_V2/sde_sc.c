#include <stdio.h>
#include <stdlib.h>
#include "Energia.h"
#include "sde_sc.h"
#include "wiring.h"
#include "USCI_I2C_slave.h"
#include "USCIAB0.h"
#include "MISC_func.h"
#include "IO_func.h"
#include "DAC_func.h"
#include "DAC_led_func.h"
#include "ds18x20.h"
#define I2C_IF           0x06      // I2C bits
#define EnWDOG    WDTCTL = (WDTPW | WDTCNTCL) & ~WDTHOLD
#define DisWDOG   WDTCTL = WDTPW | WDTHOLD
#include "adc.h"
#include "ishan.h"
#include "DS28CM00_func.h"
//#include "BMP180_func.h"
//
//
#define FIELD 1 // uncomment if field version
void POR_FPGA();
void POR_B_FPGA();
void SRST_B_FPGA();
void QSPI_RST();
void receive_cb(unsigned char receive);
void transmit_cb(unsigned char volatile *receive);
void start_cb();
void stop_cb();
void gencall_cb();
void Parse_UART();
void show_lifetime();
unsigned char flag = 0;
unsigned char flag1 = 0;
unsigned char TXData[260];
unsigned char RXData[260];
unsigned char stop=0;
unsigned char start=0;
unsigned char gencall=0;
unsigned char gcall=0;
unsigned char rcv_bytes=0;
unsigned char burst;
unsigned char direction,direction_r;
unsigned char reading;
unsigned char mTX[3];
//#define LSB_TO_5V 1.8814
#define LSB_TO_5V 1.868		// Vref*10.2/2/4095
#define LSB_TO_24V 8.88
#define LSB_TO_12V 4.43
#define LSB_TO_3V3 1.20
#define LSB_TO_1V8 0.674
#define LSB_TO_1V2 0.421
#define LSB_TO_1V0 0.366
#define CNV_TEMP 1.831
//
#define U_CRITICAL 0x5a0 	//crtical batterie state, switch PS off
#define U_WARN 0x5e5	//warn DAQ (Ux*1000/(LSB*7.8)) for TPCB 7.5 
#define U_HIGH 0x73d	//recover from critical state
// uncomment for TPCB
//#define U_CRITICAL 0x5da        //crtical batterie state, switch PS off
//#define U_WARN 0x622    //warn DAQ (Ux*1000/(LSB*7.8)) for TPCB 7.5 
//#define U_HIGH 0x787    //recover from critical state

#define U_BAT_CRIT 0x0004
#define U_BAT_WARN 0x0100
volatile extern unsigned long Second;
unsigned long WD_TIME;
unsigned long UpdateSensor;
#define DELTAUPDATE 10
#define PIN_ON                  P1OUT &=~0x02; P1DIR |= 0x02;
#define PIN_OFF                  P1OUT |= 0x02; P1DIR |= 0x02;
#define I2C_ADDRESS 0x0f	// I2C slave adress
#define PRESCALER 12
unsigned char x=0;
unsigned int  has_DS1820;
char buffer [80];
//unsigned int status_reg; // now in sde_sc.h
#define WD_INTERVAL 	5	// Watchdog Interval [sec]
#define T1		300	//Time delay after start powersupplies in ms
//
void check_battery()
{ 
	if (adc_results[BAT_OUT] < U_CRITICAL ) status_reg |= U_BAT_CRIT;
	if (adc_results[BAT_OUT] < U_WARN ) status_reg |= U_BAT_WARN;
	if (adc_results[BAT_OUT] > U_WARN + 0xa0) status_reg &= ~U_BAT_WARN; //release warn at ~10% above
	if (adc_results[BAT_OUT] > U_HIGH) status_reg &= ~U_BAT_CRIT;
}
void SlowControl_WD_Enable ()
{
        pinMode(SL_WD_EN_PIN, OUTPUT);
        digitalWrite (SL_WD_EN_PIN,HIGH);

}

int main ()
{
init();
setup();
while (1) loop();
return 0;
}

// the setup routine runs once when you press reset:
void setup() {                
 	MISC_Config();
 	IO_Config();
// Hold lines DONE, SLAVE_SDA, SLAVE_SCL low till FPGA 3V3 enabled
	pinMode(FPGA_DONE_PIN, OUTPUT);
	digitalWrite(FPGA_DONE_PIN, LOW);
	pinMode(SLAVE_SDA_PIN, OUTPUT);
	digitalWrite(SLAVE_SDA_PIN, LOW);
	pinMode(SLAVE_SCL_PIN, OUTPUT);
	digitalWrite(SLAVE_SCL_PIN, LOW);
//
 	led_fblink (50);           //set LED1 blink 5Hz
 	UART_Config();
  	delay(1000);               // wait for a second
 
//
 	UART_sprint("Auger SD Slow Control system ready \n\r");
//
// Read DS28CM00 Serial Number
//
	uprintf (PF, "Software Version: %d.%d", VERSION>>8, VERSION & 0xff);
 	UART_sprint(" \r");
// DEBUG: comment out I2C comunication before FPGS initialized
 	UART_sprint("Serial: ");
	Read_DS28CM00_ASCII ( buffer );
 	UART_sprint(buffer);
 	UART_sprint(" \n\r>");
/*
	if (BMP180_IsConnected ()) {
		 BMP180_GetValues(&adc_results[T_AIR], &adc_results[P_AIR]);
		 uprintf (PF, "BMP180 is connected, T= %d *0.1K, P= %d mBar\r",adc_results[T_AIR],adc_results[P_AIR]);
		bmp180_update_time = Second + UPD_BMP180_Delta;	 
	}
	if ( ResetDS1820 () ) {
		adc_results[T_WAT] = GetData();
		uprintf (PF, "DS18x20 connected, data: %d\r>", adc_results[T_WAT]);
	}
*/
	UART_sprint("Starting FPGA:");
//	P5DIR |= B3V3_QSPI_B | NMI_FPGA | RST_FPGA; // 
	P5DIR |= NMI_FPGA | RST_FPGA; // 
#ifdef FIELD
	PON_SEQ ();	// New Power ON Sequence
	UART_sprint(" ........ ");
#else 
               uprintf (PF,"Version TEST %d\r");
//             
          P2OUT |= (PS_PMT_12V_EN | PS_RADIO_12V_EN);     // Enable ADC Powersupplies

                P1OUT &= ~(FPGA_CORE_PS);
                P1OUT &= ~(PS_AN_EN | PS_EXT1_24V_EN | PS_EXT2_24V_EN);  // Enable ADC Powersupplies 
                P2OUT &= ~(PS_PMT_12V_EN | PS_RADIO_12V_EN);     // Enable ADC Powersupplies

#endif
 	UART_sprint("Initialize ADCs \r");

	dac_init();
//	dac_set (1, 1640); // set dac chan 1 to 1V, reference for adc
	adc_init();

  TI_USCI_I2C_slaveinit(start_cb, stop_cb, gencall_cb, transmit_cb, receive_cb, I2C_ADDRESS);
 led_fblink (0);           //set LED1 blink 5Hz
}
void check_stat()
{
        if (P2IN & DONE) {              // FPGA booted
		status_reg |= FPGA_DONE_STAT;
		led_fblink (0);
	} else {
		status_reg &= ~FPGA_DONE_STAT;
	}
	if (SC_WD_STATE) {
			if ( WD_TIME == Second ) {
				digitalToggle ( SL_WD_PIN);
				WD_TIME = Second + WD_INTERVAL;
			}
	}
	if (SRST_B_STATE) {
		delay (1000);
		digitalWrite (NMI_FPGA_PIN, HIGH);
		status_reg &= ~SRST_B;
	}
}
// the loop routine runs over and over again forever:
void loop() {
char b[10];
#ifdef FIELD 
  
  if (act_mask & QSPI_RST_N_IRQ ) {
long dt;	
	dt = millis();
	uprintf (PF, "P4DIR: 0x%x P4IN: 0x%x \r",P4DIR, P4IN);
	uprintf (PF, "P5DIR: 0x%x P5IN: 0x%x \r",P5DIR, P5IN);
  	uprintf (PF, "QSPI_RST_N ... " );
        digitalWrite (QSPI_3V3_B_PIN,LOW);
        pinMode(RST_FPGA_PIN, OUTPUT);
        digitalWrite (RST_FPGA_PIN,LOW);

        delayMicroseconds(6800);
        digitalWrite (QSPI_3V3_B_PIN,HIGH);
        delay (16);
        digitalWrite (RST_FPGA_PIN,HIGH);
        pinMode(RST_FPGA_PIN, INPUT);
	dt = millis() -dt;
	uprintf (PF," time %d ms\n",dt);
        P1IE |= QSPI_RST_N;
        act_mask &= ~QSPI_RST_N_IRQ;

  }
#endif
//        if (BMP180_IsConnected () && ( Second == bmp180_update_time) ) {
//		 BMP180_GetValues(&adc_results[T_AIR], &adc_results[P_AIR]);
//		bmp180_update_time += UPD_BMP180_Delta;
//		if ( ResetDS1820 () ) {
//			adc_results[T_WAT] = GetData();
//		}
//	}
	check_stat ();
  if (EOT) Parse_UART();
  EOT = 0;
  adc_update();

if (act_mask & UPD_ADC ) {
                        act_mask &= ~UPD_ADC;
                        adcctl = ADC_DOCONV;
                        adc_start_conversion();
        }
// check if conversion pending
	if (0 == adcctl) { // conversion done
		check_battery ();
	}


  if ( stop == 1 ) {
unsigned char *cp;
int i;
unsigned int reg, mask;
	reg = RXData[1]<<8 | RXData[0];
    switch (reg) {  
      case 0x1:
	cp = Read_DS28CM00_BIN();
	for (i=0; i<=7; i++) TXData[i] = *cp++;
	
	break;
      case 0x2:
		TXData[0] = (char) (status_reg & 0xff);
		TXData[1] = (char) ((status_reg >> 8) & 0xff);
	break;
      case 0x3:
                TXData[0] = (char) (Second & 0xff);
                TXData[1] = (char) ((Second >> 8) & 0xff);
                TXData[2] = (char) ((Second >> 16) & 0xff);
                TXData[3] = (char) ((Second >> 24) & 0xff);
	break;
      case 0x4:
	if (rcv_bytes == 2 ) {
		TXData[0] = P1IN;
		TXData[1] = P2IN & 0x03; 
 	} else
	{
		mask = 0x3a | RXData[2];
		P1OUT = mask; // make sure not to switch off FPGA
		if (rcv_bytes > 3 ) {
			mask = P2IN & 0xfc;
			P2OUT = mask | (RXData[3] & 0x3);
		}
	}	
	break;
      case 0x5: // DAC control
	if (rcv_bytes == 4 ) {
	long int li1, li2;
		li1 = (RXData[3] & 0x70)>>4;
                li2 = (RXData[3] & 0x0f)<<8 | RXData[2];
                if ( (li1 >=0 && li1 <=7 ) && (li2 >=0 && li2 < 4096 ) ) {
                     dac_set ( li1, li2 );
                }

	}
	break;
      case 0x8: // LED DAC control
        if (rcv_bytes == 4 ) {
        long int li1, li2;
                li1 = (RXData[3] & 0x70)>>4;
                li2 = (RXData[3] & 0x0f)<<8 | RXData[2];
                if ( (li1 >=0 && li1 <=3 ) && (li2 >=0 && li2 < 4096 ) ) {
                     dac_led_set ( li1, li2 );
                }

        }
        break;

      case 0x6: // Watchdog Control ON
		 SlowControl_WD_Enable ();
	break;
      case 0x7: // SC_WD ON/OFF
	SlowControl_WD_Enable ();
  	uprintf (PF, "Data: %x %x\r",RXData[2],RXData[3]  );
	if ( RXData[2] ) {
		SC_WD_ENABLE;
                WD_TIME = Second + 1;
                uprintf(PF," SlowControl WD running \r");

	}
	else {
		SC_WD_DISABLE;

	}
	break;
      case 0x9: // ADC values
	cp = (char *) adc_results;
	for (i=0; i<2*MAX_VARS; i++) TXData[i] = *cp++;
	break;
     case 0xa:  // Test
	TXData[0] = 0x21;
        TXData[1] = 0x43;
	break;
      case 0xb:
//	while (1); // Stop, wait for WD
	break;
      case 0xc:	   // Radio reset, pin 47 (P5_3) low for 300ms
	pinMode ( RAD_RST_OUT_PIN, OUTPUT );
	digitalWrite ( RAD_RST_OUT_PIN, LOW);
	delay (300);
	pinMode ( RAD_RST_OUT_PIN, INPUT );
	break;
      case 0xd: // temporary DAC reset
	if ( RXData[2] ) {
		P4OUT |= LED1_IO_PIN;
	}
	else {
		P4OUT &= ~LED1_IO_PIN;
	}
	break;
      default:
	break;
   }
   rcv_bytes=0;
  }
  stop = 0;
  __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
//   _EINT();
}
void start_cb(){
  flag1 = 0;
  flag = 0;
}

void stop_cb(){
  stop = direction;
  gencall = gcall;
  burst = RXData[0]+2;
  direction = 0;
  gcall = 0;
}

void gencall_cb(){
  gcall = 1;
}

void receive_cb(unsigned char receive){
  direction = 1;
  RXData[flag1++] = receive;
  rcv_bytes++;
}

void transmit_cb(unsigned char volatile *byte){
  direction = 0;
  *byte = TXData[flag++];
}
char * pEnd;
// long int li1, li2, li3;
int li1, li2, li3;
char str[24];
void print_f ( float f, char *str) 
{
int fi;
	fi = f;
  	uprintf (PF, "%d %s ", fi, str); 
}
void Parse_UART()
{
 	UART_sprint ("\r");
	switch (uart_buffer[0])
	{
		case 'r': //status_reg
			if (status_reg & PS_AN)  uprintf (PF, "\r%s","PS_AN");
			if (status_reg & PS_5V)  uprintf (PF, "\r%s","PS_5V");
			if (status_reg & PS_3V3) uprintf (PF, "\r%s","PS_3V3");
			if (status_reg & PS_1V8) uprintf (PF, "\r%s","PS_1V8");
			if (status_reg & PS_1V)  uprintf (PF, "\r%s","PS_1V");
			if (status_reg & PS_EXT1) uprintf (PF, "\r%s","PS_EXT1");
			if (status_reg & PS_EXT2) uprintf (PF, "\r%s","PS_EXT2");
			if (status_reg & U_BAT_CRIT) uprintf (PF, "\r%s","U_BAT_CRIT");
			if (status_reg & BAT_LOW) uprintf (PF, "\r%s","BAT_WARN");
			if (status_reg & CUR_ERR) uprintf (PF, "\r%s","CUR_ERR");
			if (status_reg & TMP_ERR) uprintf (PF, "\r%s","TMP_ERR");
			if (status_reg & PS_MAIN) uprintf (PF, "\r%s","PS_MAIN");
			if (status_reg & WD_STAT) uprintf (PF, "\r%s","WD_STAT");
			if (status_reg & SRST_B) uprintf (PF, "\r%s","SRST_B");
			if (status_reg & FPGA_DONE_STAT) uprintf (PF, "\r%s\r U_BAT:","FPGA_DONE");
		 uprintf (PF," adc: %d \r", adc_results[BAT_OUT]);

			break;
		case 'R': // test
			UART_sprint("Port1:");
			UART_sprintx(P1IN);
 	UART_sprint ("\r");
			UART_sprint(" Port2:");
			UART_sprintx(P2IN);
 	UART_sprint ("\r");
			UART_sprint(" Port4:");
			UART_sprintx(P4IN);
 	UART_sprint ("\r");
			UART_sprint(" Port5:");
			UART_sprintx(P5IN);
			break;

		case 'a':
			UART_sprint("PMT Stat:  HVmon   Imon   Tmon\r");
			uprintf( PF, "\r%s     ","PMT1"); 
				pf ((float)adc_results[PMT1_HVM] *LSB_TO_5V); 
				pf ((float)adc_results[PMT1_CM]*LSB_TO_5V);
				pf ((float)adc_results[PMT1_TM]*CNV_TEMP);
                        uprintf( PF, "\r%s     ","PMT2");
                                pf ((float)adc_results[PMT2_HVM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT2_CM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT2_TM]*CNV_TEMP);
                        uprintf( PF, "\r%s     ","PMT3");
                                pf ((float)adc_results[PMT3_HVM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT3_CM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT3_TM]*CNV_TEMP);

                        uprintf( PF, "\r%s     ","PMT4");
                                pf ((float)adc_results[PMT4_HVM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT4_CM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT4_TM]*CNV_TEMP);
                        uprintf( PF, "\r%s     ","PMT5");
                                pf ((float)adc_results[PMT5_HVM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT5_CM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT5_TM]*CNV_TEMP);

                        uprintf( PF, "\r%s     ","PMT6");
                                pf ((float)adc_results[PMT6_HVM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT6_CM]*LSB_TO_5V);
                                pf ((float)adc_results[PMT6_TM]*CNV_TEMP);

                        uprintf( PF, "\r%s     ","Power supplies");
                        uprintf( PF, "\r Nominal Actual Current");
			uprintf( PF, "\r 1V      "); 
			print_f((float)adc_results[V_1V0]*LSB_TO_1V0,"[mV] ");
			print_f((float)adc_results[I_1V0]*LSB_TO_1V0/60.*41.67,"[mA] "); // 41.67=1/0.024
			if (check (adc_results[V_1V0], 0xaaa, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r 1V2     ");
                        print_f((float)adc_results[V_1V2]*LSB_TO_1V2,"[mV] ");
			print_f((float)adc_results[I_1V2]*LSB_TO_1V0/60.*10.,"[mA] ");   // 10=1/.1
			if (check (adc_results[V_1V2], 0xb21, 5) ) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r 1V8     ");
                        print_f((float)adc_results[V_1V8]*LSB_TO_1V8,"[mV] ");
                        print_f((float)adc_results[I_1V8]*LSB_TO_1V0/60.*30.3,"[mA] "); // 30.3 =1/0.033
			if (check (adc_results[V_1V8], 0xa70, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r 3V3     ");
                        print_f((float)adc_results[V_3V3]*LSB_TO_3V3,"[mV] ");
                        print_f((float)adc_results[I_3V3]*LSB_TO_3V3/60.*16.13,"[mA] "); // 16.13 = 1/0.062
                        print_f((float)adc_results[I_3V3_SC]*LSB_TO_1V0/60.*1.22,"[mA SC] "); // 1.22 = 1/0.82
			if (check (adc_results[V_3V3], 0xac6, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r P3V3     ");
                        print_f((float)adc_results[V_AN_P5V]*LSB_TO_3V3,"[mV] ");
                        print_f((float)adc_results[I_P5V_ANA]*LSB_TO_1V0/60.*12.2,"[mA] ");
			if (check (adc_results[V_3V3], 0xac6, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r N3V3     ");
                        print_f(3500. - (float)(adc_results[V_AN_N5V]*LSB_TO_1V0),"[mV] ");
                        print_f((float)adc_results[I_N5V_ANA]*LSB_TO_1V0/60.*12.2,"[mA] ");
			uprintf( PF, "\r 5V       ");
                        print_f((float)adc_results[V_GPS_5V]*LSB_TO_5V,"[mV] ");
                        print_f((float)adc_results[I_GPS_5V]*LSB_TO_1V0/60.*10,"[mA] ");
			if (check (adc_results[V_GPS_5V], 0xac6, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r12V Radio ");
                        print_f((float)adc_results[V_RADIO_12V]*LSB_TO_12V,"[mV] ");
                        print_f((float)adc_results[I_RADIO_12V]*LSB_TO_1V0/60.*30.3,"[mA] ");
			if (check (adc_results[V_RADIO_12V], 0xa94, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r12V PMTs  ");
                        print_f((float)adc_results[V_PMTS_12V]*LSB_TO_12V,"[mV] ");
                        print_f((float)adc_results[I_PMTS_12V]*LSB_TO_1V0/60.*30.3,"[mA] ");
			if (check (adc_results[V_PMTS_12V], 0xa94, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\r24V EXT1/2  ");
                        print_f((float)adc_results[V_EXT1_24V]*LSB_TO_24V,"[mV] ");
                        print_f((float)adc_results[V_EXT2_24V]*LSB_TO_24V,"[mV] ");
                        print_f((float)adc_results[I_V_INPUTS]*LSB_TO_1V0/60.*21.28,"[mA] "); // 21.28=1/0.047
			if (check (adc_results[V_EXT1_24V], 0xa8d, 5)) uprintf (PF, "<----- ERROR");
                        uprintf( PF, "\rSensors ");
                        uprintf (PF, " \rT= %d *0.1K, P= %d mBar TW = ",adc_results[T_AIR],adc_results[P_AIR]);
                        uprintf (PF, "%d *0.1K",adc_results[T_WAT]);
				                        
			break;
		case 'A':
			adc_dump();
			break;
		case 'D':
                        pEnd = &uart_buffer[1];
                        li1 = strtol (pEnd, &pEnd, 10);
                        li2 = strtol (pEnd, &pEnd, 10);
                        if ( (li1 >=0 && li1 <=4 ) && (li2 >=0 && li2 < 4096 ) ) {
uprintf (PF,"li1: 0x%x, li2: 0x%d\r",li1,li2); 
                                dac_led_set ( li1, li2 );
                                }
                        else {
                                UART_sprint (" out of range");
                        }
                        break;
     
			break;
		case 'd':
			pEnd = &uart_buffer[1];
			li1 = strtol (pEnd, &pEnd, 10);
			li2 = strtol (pEnd, &pEnd, 10);
			if ( (li1 >=0 && li1 <=8 ) && (li2 >=0 && li2 < 4096 ) ) {
uprintf (PF,"li1: 0x%x, li2: 0x%x\r",li1,li2); 
				dac_set ( li1, li2 );
				}
			else {
				UART_sprint (" out of range");
			}	
			break;
		case 'h':
		case 'H':
			uprintf( PF,"s or S - show serial number\r");
			uprintf( PF,"v or V - show software version\r");
			uprintf( PF,"l or L - show Lifetime\r");

			uprintf( PF,"p or P n - enable/disable power supplies\r");
			uprintf( PF,"       n =1 12V PMT\r");
			uprintf( PF,"       n =2 12V RADIO\r");
			uprintf( PF,"       n =3 24V EXT1/EXT2\r");
			uprintf( PF,"       n =4 +/- analog 3V3\r");
			uprintf( PF,"       n =5 +/- FPGA core power supplies (FPGA will not boot after disabling)\r");
			uprintf( PF,"d or D chan val - set DAC channnel chan [0..8] to value val [0..4095]\r");
			uprintf( PF,"a - show analog variables in physical units\r");
			uprintf( PF,"A - show analog variables as raw data in HEX\r");
			uprintf( PF,"w or W and X foe WD handling\r");
			uprintf( PF,"h or H - show this help");
			break;
		case 'v':
		case 'V':
#ifdef FIELD
				uprintf (PF, "Software Version: %d.%d", VERSION>>8, VERSION & 0xff);
#else
				uprintf (PF, "TEST Software Version: %d.%d", VERSION>>8, VERSION & 0xff);

#endif
			break;
		case 's':
		case 'S':
			Read_DS28CM00_ASCII ( buffer );
        		uprintf(PF, "SN: %s",buffer);
			break;
                case 'b':
		case 'B':
			show_button_t();
			break;
		case 'L':
                        pEnd = &uart_buffer[1];
                        li1 = strtol (pEnd, &pEnd, 10);
        		if (li1) 
				P4OUT |= LED1_IO_PIN;
			else
				P4OUT &= ~LED1_IO_PIN;
			break;
		case 'l':
			show_lifetime();
			break;
		case 'p':
                        pEnd = &uart_buffer[1];
                        li1 = strtol (pEnd, &pEnd, 10);
                        switch (li1) {
                                case 1:
                                        P2OUT |= (PMT_CORE_PS );        // Enable PMT Powersupplies
                                        uprintf(PF," 12V PMT ON \r");
                                        break;
                                case 2:
                                        P2OUT |= (RADIO_CORE_PS );      // Enable RADIO Powersupplies
                                        uprintf(PF," 12V RADIO ON \r");


                                        break;
                                case 3:
                                        P1OUT |= (EXT_CORE_PS );        // Enable EXT1/2 Powersupplies
                                        uprintf(PF," 24V EXT1/2 ON \r");

                                        break;
                                case 4:
                                        P1OUT |= (PS_AN_EN);        // Enable ADC +-3V3 Powersupplies
                                        uprintf(PF," +/-3V3 ADC ON \r");

                                        break;
                                case 5:
                                        P1OUT |= (FPGA_CORE1_PS );       // Enable EXT1/2 Powersupplies
                                        P1OUT |= (FPGA_CORE2_PS );       // Enable EXT1/2 Powersupplies
                                        uprintf(PF,"  1V0 1V8 3V3 FPGA ON \r");

                                default:
                                        break;
                        }

			break;
		case 'P':		// disable supplies
                        pEnd = &uart_buffer[1];
                        li1 = strtol (pEnd, &pEnd, 10);
                        switch (li1)  {
                               case 1:
                                        P2OUT &= ~(PMT_CORE_PS );        // PMT Powersupplies
                                        uprintf(PF," 12V PMT OFF \r");
                                        break;
                                case 2:
                                        P2OUT &= ~(RADIO_CORE_PS );      // RADIO Powersupplies
                                        uprintf(PF," 12V RADIO OFF \r");


                                        break;
                                case 3:
                                        P1OUT &= ~(EXT_CORE_PS );        // EXT1/2 Powersupplies
                                        uprintf(PF," 24V EXT1/2 OFF \r");

                                        break;
                                case 4:
                                        P1OUT &= ~(PS_AN_EN );        // ADC +-3V3 Powersupplies
                                        uprintf(PF," +/-3V3 ADC OFF \r");

                                        break;
                                case 5:
                                        P1OUT &= ~(FPGA_CORE1_PS );       // FPGA Powersupplies
                                        P1OUT &= ~(FPGA_CORE2_PS );       // Powersupplies
                                        uprintf(PF,"  1V0 1V8 3V3 FPGA OFF \r");

                                default:
                                        break;
                        }



			break;
		case 'w':
			SC_WD_ENABLE;
			WD_TIME = Second + 1;
                        uprintf(PF," SlowControl WD running \r");
			break;
		case 'W':
			SC_WD_DISABLE;
                        uprintf(PF," SlowControl WD stopped \r");
			break;
		case 'x':
		case 'X':
			SlowControl_WD_Enable (); 
                        uprintf(PF," SlowControl WD enabled \r");
			break;
		default:
			break;
	}
 	UART_sprint ("\r>");

}
void show_lifetime()
{ char b[12];
	UART_sprint("Lifetime [s]: ");
	sprintf ( b, "%lu", Second);
	UART_sprint (b);
}
void show_button_t()
{ char b[12];
        UART_sprint("Pressed [ms]: ");
        sprintf ( b, "%lu", t0_srst_b);
        UART_sprint (b);
}


int check ( int v, int n, int t) // v= value, n= nominal, t= tolerance in %
{
float min, max;
int ret;
 min = (float) n * (1.- (float) t/100.); 
 max = (float) n * (1.+ (float) t/100.); 
 ret = 0;
 if ( v < (int) min ) ret =1;
 if ( v > (int) max ) ret =1;
 return (ret);

}
void POR_FPGA()
{
	delay (200); // to do: wait for 3V3 exceeds 2.7 V
// drive RST_FPGA low
//drive 3V3_QSPI_B high
	P4DIR |= B3V3_QSPI_B;	// B3V3_QSPI_B set to low
	delay (15);		// wait 15 ms
	P5DIR &= ~NMI_FPGA; 	// release NMI_FPGA
	delay (175);		// wait 175 ms (total 200)
	P5DIR &= ~RST_FPGA; 	// release NMI_FPGA
	
}	 
void POR_B_FPGA()
{
// drive RST_FPGA low
	delay (500);		// to be changed later for interrupt
	P5DIR |= RST_FPGA;	// B3V3_QSPI_B set to low
//drive 3V3_QSPI_B high
        P4DIR &= ~B3V3_QSPI_B;   // release 3v3_QSPI 
        delay (10);             // wait 15 ms
	P4DIR |= B3V3_QSPI_B;	// B3V3_QSPI_B set to low
        delay (200);             // wait 15 ms
        P5DIR &= ~RST_FPGA;     // release NMI_FPGA

}

void SRST_B_FPGA()
{
// drive RST_FPGA low
        P5DIR |= NMI_FPGA;      // B3V3_QSPI_B set to low
//drive 3V3_QSPI_B high
        P4DIR &= ~B3V3_QSPI_B;   // release 3v3_QSPI
        delay (10);             // wait 15 ms
        P4DIR |= B3V3_QSPI_B;   // B3V3_QSPI_B set to low
        delay (100);             // wait 15 ms
        P5DIR &= ~NMI_FPGA;     // release NMI_FPGA

}



void QSPI_RST()
{
//drive 3V3_QSPI_B high
        P4DIR &= ~B3V3_QSPI_B;   // release 3v3_QSPI (goes high)
        P4OUT |= B3V3_QSPI_B;
        P5DIR |= PS_POR_B;   // set PS_POR_B to OUPUT -> goes low
	delay (4);	
        P4OUT &= ~B3V3_QSPI_B;  
	delay (4);	
	P4DIR |= B3V3_QSPI_B;	// B3V3_QSPI_B set to low
        P5DIR &= ~PS_POR_B;   // release 3v3_QSPI (goes high)
//  look on QSPI_RST_N == 0
}
//  Power ON sequence for gen. II UUB
void PON_SEQ()
{
// RST_FPGA -> LOW
	pinMode(NMI_FPGA_PIN, OUTPUT);
	digitalWrite (NMI_FPGA_PIN,LOW); 
        P1OUT =(FPGA_CORE1_PS);  // Enable 1V PS
        P1OUT |=(FPGA_CORE2_PS);  // Enable 1V2,  3V3 + 5V PS
//        delayMicroseconds(500);
	pinMode(FPGA_DONE_PIN, INPUT);
	pinMode(SLAVE_SDA_PIN, INPUT);
	pinMode(SLAVE_SCL_PIN, INPUT);
	delay (4);
        P1OUT |=(PS_1V8_EN);  // Enable 1V8 Must be after 3V3??

	pinMode(RST_FPGA_PIN, OUTPUT);
	digitalWrite (RST_FPGA_PIN,LOW); 
// NMI_FPGA -> HIGH
	pinMode(NMI_FPGA_PIN, OUTPUT);
	digitalWrite (NMI_FPGA_PIN,HIGH); 
// QSPI_3V3_B -> LOW
        pinMode(QSPI_3V3_B_PIN, OUTPUT);
        digitalWrite (QSPI_3V3_B_PIN,LOW);
// SL_WD -> LOW
        pinMode(SL_WD_PIN, OUTPUT);
        digitalWrite (SL_WD_PIN,LOW);

// Wait T1
	delay (T1);
// RST_FPGA -> HIGH
        digitalWrite (RST_FPGA_PIN,HIGH);
// QSPI_3V3_B -> HIGH
        digitalWrite (QSPI_3V3_B_PIN,HIGH);
//        delay (200); 	// wait 200 ms
        delay (200); // wait 200 ms
        P1OUT |= (PS_AN_EN | PS_EXT1_24V_EN | PS_EXT2_24V_EN);  // Enable ADC Powersupplies 
        P2OUT |= (PS_PMT_12V_EN | PS_RADIO_12V_EN);     // Enable ADC Powersupplies
	SlowControl_WD_Enable ();
        SC_WD_ENABLE;
        WD_TIME = Second + 1;


}
