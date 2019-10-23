#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sde_sc.h"
#define VERSION_STRING " Version 3.1"
#define LSB_TO_5V 1.868
#define LSB_TO_24V 8.88
#define LSB_TO_12V 4.43
#define LSB_TO_3V3 1.20
#define LSB_TO_1V8 0.674
#define LSB_TO_1V2 0.421
#define LSB_TO_1V0 0.366
#define SC_ADDR		0x0f
char buf[160];
short int adc_buffer[MAX_VARS];
static const char *optString = "d:w:VW:klL:rsStp:v:P::I::Aah?";
/* Read SlowControl serial Number */
void sc_serial ( int file, char *b)
{   char reg[] ={0x01, 0x00};

//	reg[0] = 0x01;
//  reg[2] = 0x00;
	  if (write(file, reg, 2) != 2) {
	          	 	exit(3);
	           	}
	  usleep (100000);
	  if (read(file,b,8)!= 8) {
		  	  	  	exit(4);
	  }
	  return ;
}
void sc_status (int file, char *b)
{   char reg[] ={0x02, 0x00};
     if (write(file, reg, 2) != 2) {
        	 	exit(3);
     }
     usleep (100000);
     if (read(file,b,2)!= 2) {
	  	  	  	exit(4);
	 }
     return ;
}
void sc_livetime (int file , int *l)
{char reg [6] = {0x03, 0x00, 0x0, 0x0, 0x0, 0x0};
 char b[4];
	if (write(file, reg, 2) != 2) {
        	 	exit(3);
    }
	usleep (100000);
	if (read(file,b,4)!= 4) {
		  	  	exit(4);
	}
//	printf("b: 0x%x 0x%x 0x%x 0x%x\n", b[0], b[1], b[2], b[3]);
	*l =  b[0]
		+ ( b[1] << 8 )
		+ ( b[2] << 16 )
		+ ( b[3] << 24 );
}
void sc_watchdog (int file, int value)
{	char reg [4] = {0x07, 0x00, 0x0, 0x0};
	reg [2] = (char) (value & 0xff);
	if (write (file, reg, 4) != 4) {
		exit (3);
	}

	return;
}
void sc_ADC_en (int file, int value)
{	char reg [4] = {0x0d, 0x00, 0x0, 0x0};
	reg [2] = (char) (value & 0xff);
	if (write (file, reg, 4) != 4) {
		exit (3);
	}

	return;
}
void sc_ANALOG ( int file, int value)
{	char reg [4] = {0x10, 0x00, 0x0, 0x0};
	reg [2] = (char) (value & 0xff);
	if (write (file, reg, 4) != 4) {
		exit (3);
	}
}
void kill( int file )
{
	char reg[2] = {0x0b, 0x00};
	if (write (file, reg, 2) != 2) {
		exit (3);
	}

}
void radio_rst (int file)
{
	char reg[2] = {0x0c, 0x00};
	if (write (file, reg, 2) != 2) {
		exit (3);
	}


}
void sc_version ( int file, char*b )
{ char reg[] = {0x0e,0x00};
if (write(file, reg, 2) != 2) {
        	 	exit(3);
     }
     usleep (100000);
     if (read(file,b,2)!= 2) {
     	  	  	  	exit(4);
     }
     return ;
}
void sc_test_reg (int file, char *b)
{   char reg[] ={0x0a, 0x00};
     if (write(file, reg, 2) != 2) {
        	 	exit(3);
     }
     usleep (100000);
     if (read(file,b,2)!= 2) {
	  	  	  	exit(4);
	 }
     return ;
}
void sc_get_ADC_values (int file) //, char *b)
{   char reg[] ={0x09, 0x00};
     if (write(file, reg, 2) != 2) {
        	 	exit(3);
     }
     usleep (100000);
     if (read(file,(char *)adc_buffer,2*MAX_VARS)!= 2*MAX_VARS) {
	  	  	  	exit(4);
	 }

     return ;
}
void sc_powerControl_reg (int file, char *b)
{   char reg[] ={0x04, 0x00};
     if (write(file, reg, 2) != 2) {
        	 	exit(3);
     }
     usleep (100000);
     if (read(file,b,2)!= 2) {
	  	  	  	exit(4);
	 }
     return ;
}
void sc_powerControl_reg_w (int file, char *b)
{   char reg[4] ={0x04, 0x00, 0x0, 0x0};
	 reg [2] = *b++;
	 reg [3] = *b;
	 if (write(file, reg, 4) != 4) {
        	 	exit(3);
     }
     usleep (100000);
     return ;
}
void sc_ident_reg_w( int file, char*buf)
{
int i;
char reg[6] ={0x0f, 0x00, 0x0, 0x0, 0x0,0x0};
	for (i=0;i<4; i++) {
		reg[i+2] = buf[i];

	}
	 if (write(file, reg, 6) != 6) {
       	 	exit(3);
    }
	 return;
}
void sc_ident_reg( int file, char*buf)
{
char reg[5] ={0x0f, 0x00, 0x0, 0x0, 0x0};
	 if (write(file, reg, 2) != 2) {
       	 	exit(3);
    }
     usleep (100000);
     if (read(file,buf,4)!= 4) {
	  	  	  	exit(4);
	 }
     return;
}
void sc_set_dac (int file, int chan, int value)
{	char reg[4] ={0x05, 0x00, 0x0, 0x0};
	static char ch_msk[6] = {0x00, 0x20, 0x60, 0x30, 0x40, 0x50};
		chan = chan -1;
		if ( (chan >= 0 && chan <=5) &&
				(value >=0 && value <= 4095) ) {
		 reg [2] = (char) (value & 0xff);
		 reg [3] = ch_msk[chan] | (char) ((value >>8) & 0x0f);
		 printf ("DAC: 0x%.2x%.2x\n",reg[3],reg[2]);
		 if (write(file, reg, 4) != 4) {
	        	 exit(3);
	     	 	 }
	     usleep (100000);
		 } else printf ( "invalid value %d %d\n", chan+1, value);
	 return ;


}
//sStp:v:P::Aah?
void display_usage( char *s )
{
    puts( "Version 3.1 \n Usage:" );
    printf ( "%s [-aAsStlV] [-I[NUM]] [-L 0|1] [-d 0|1] [-wW ARG] [-P[HEX]] [-p P_ARG -v V_ARG]\n", s);
    puts ("Options:");
    puts ("-a \t show a map of environment variables in human readable form");
    puts ("-A \t show a map of environment variables as hex raw data");
    puts ("-S \t show content of status register in hex");
    puts ("-r \t radio reset");
    puts ("-s \t show serial number");
    puts ("-l \t show lifetime [s]");
    puts ("-L \t set ADC enable signal low or high");
    puts ("-t \t show content of test register [0x4321]");
    puts ("-w or -W \t Watchdog control");
    puts ("   \t ARG = 1 Slowcontrol managing WD");
    puts ("   \t ARG = 0 ZYNQ is managing WD");
    puts ("-P \t if [HEX] omitted show content of power control register in hex");
    puts ("   \t no space between arg and option");
    puts ("   \t with [HEX] set power control register to [HEX]");
    puts ("   \t the program takes care that you cannot switch off power supplies used by FPGA");
    puts ("-p");
    puts ("-v \t -p -v are used to set the high voltage for PMT1-6 P_ARG=(1-6) ");
    puts ("   \t to value V_ARG(0x0....0x0fff)");
    puts ("-V \t shows version of slowc and scu firmware ");
    puts ("-I \t with arg stores NUM (1-4 digit number) into EEPROM, without prints NUM ");
    puts ("   \t no space between arg and option");
    puts ("-d \t analog +/-3V3 control");
    puts ("   \t ARG = 1 enable analog 3V3");
    puts ("   \t ARG = 0 disable ");
    puts ("   \t for further information refer to the slow control register map documentation");
    exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] )
{

    int opt = 0;


	int file, i, j, ch, val;
	char filename[20];

	opt = getopt( argc, argv, optString );
  /* Geraetedatei oeffnen */

//  printf("Opening device... ");
  snprintf(filename, 19, "/dev/i2c-0");
  	file = open(filename, O_RDWR);
  	if (file < 0) {
//  			printf("error opening %s\n",filename);
  		perror(" Error opening file");
  			exit (1);
  	}
  	if (ioctl(file, I2C_SLAVE, SC_ADDR) < 0) {
  			printf("Fail to setup slave addr!\n");
  			exit (1);
  	}


//  printf(" OK\n");
//  dac_ok = 0x0;

  while( opt != -1 ) {
	  int lt;
         switch( opt ) {
             case 's': // show serial number
            	 sc_serial(file, buf);
            	   printf("SN: "); for (i=1; i<6; i++) printf("%.2x-", buf[i]);
            	   printf("%.2x\n", buf[i]);
            	 break;
             case 'S':
            	 sc_status (file, buf);
            	 printf("Status Register : 0x%.2x", buf[1]);
            	 printf("%.2x\n", buf[0]);

            	 break;
             case 'I':
            	 if (optarg) {
            		 i = (int) strtol (optarg,NULL, 0);
            		 sprintf(buf, "%04d",i);
            		 sc_ident_reg_w (file,buf);

            	 }
            	 else {
            	 sc_ident_reg(file, buf);
            	 printf ("ID: %4s\n", buf);
            	 }

            	 break;
             case 'V':
            	 sc_version (file, buf);
            	 printf ("%s %s scu FW Version %d.%d\n",argv[0],VERSION_STRING, buf[1], buf[0]);
            	 break;
             case 'k':
            	 kill (file);
            	 break;
             case 'r':
            	 radio_rst (file);
            	 break;
             case 'l':
            	 sc_livetime (file, &lt );
            	 printf("%d\n",lt);
            	 break;
             case 'p': // pmt 1-6
            	 printf(" opt p val %s\n",optarg);
            	 ch = (int) strtol (optarg,NULL, 0);
            	 break;
             case 'v': // voltage 0-4095
            	 printf(" opt v val %s\n",optarg);
            	 val = (int) strtol (optarg,NULL, 0);

            	 sc_set_dac (file, ch, val);
            	 break;
             case 'w':
             case 'W':
            	 val = (int) strtol (optarg, NULL, 0);
            	 sc_watchdog ( file, val);
            	 break;
             case 'L':
            	 val = (int) strtol (optarg, NULL, 0);
            	 sc_ADC_en ( file, val);
            	 break;
             case 'A': // get adc - raw data
            	 sc_get_ADC_values (file);
//				 for (i=0; i<=7;i++) {
//					 for (j=0; j<=7; j++) printf(" 0x%.2x%.2x",buf[2*(i*8+j)+1],buf[2*(i*8+j)]);


            	 for (j=0; j<64; j++) {
            		 if (j%8 == 0) printf ("\n");
            		 	 printf(" 0x%.4x ",adc_buffer[j]);
            	 }
            	 printf ("\n");
            	 break;
             case 'a':
            	 sc_get_ADC_values (file);
            	 printf("PMT Stat: HVmon[V]\tImon[uA] \t Tmon[C]\n");
            	 printf ("PMT1");
            	 printf ("\t %.1f",(float)adc_buffer[PMT1_HVM] *LSB_TO_5V*1.25);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT1_CM]*LSB_TO_5V);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT1_TM]*0.3662/2.)-273.15);
            	 printf ("\nPMT2");
            	 printf ("\t %.1f",(float)adc_buffer[PMT2_HVM]*LSB_TO_5V*1.25);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT2_CM]*LSB_TO_5V);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT2_TM]*0.3662/2.)-273.15);
            	 printf ("\nPMT3");
            	 printf ("\t %.1f",(float)adc_buffer[PMT3_HVM]*LSB_TO_5V*1.25);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT3_CM]*LSB_TO_5V);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT3_TM]*0.3662/2.)-273.15); //0.3662 = mV/adc count; measured over 2kOhm
            	 printf ("\nSSD_PMT");
            	 printf ("\t %.1f",(float)adc_buffer[PMT4_HVM]*LSB_TO_5V/1875.*1500.);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT4_CM]*LSB_TO_5V/10.);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT4_TM]*0.3662/2.)-273.15);
            	 printf ("\nSM_PMT");
            	 printf ("\t %.1f",(float)adc_buffer[PMT5_HVM]*LSB_TO_5V*1.25);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT5_CM]*LSB_TO_5V);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT5_TM]*0.3662/2.)-273.15);
            	 printf ("\nPMT6");
            	 printf ("\t %.1f",(float)adc_buffer[PMT6_HVM]*LSB_TO_5V*1.25);
            	 printf ("\t\t %.1f",(float)adc_buffer[PMT6_CM]*LSB_TO_5V);
            	 printf ("\t\t %.1f",(float)(adc_buffer[PMT6_TM]*0.3662/2.)-273.15);
            	 printf ("\n");
            	 printf ("\nPower supplies");
            	 printf ("\nNominal \t Actual \t Current");
            	 printf ("\n10V \t");
            	 printf ("\t %.1f %s",(float)adc_buffer[V_10V]*LSB_TO_12V,"[mV] ");
            	 printf ("\n1V \t");
            	 printf ("\t %.1f %s",(float)adc_buffer[V_1V0]*LSB_TO_1V0,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_1V0]*LSB_TO_1V0/60.*41.67,"[mA] "); // 41.67=1/0.024
            	 printf("\n1V2\t");
            	 printf("\t %.1f %s",(float)adc_buffer[V_1V2]*LSB_TO_1V2,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_1V2]*LSB_TO_1V0/60.*10.,"[mA] ");   // 10=1/.1
            	 printf( "\n1V8\t");
            	 printf("\t %.1f %s",(float)adc_buffer[V_1V8]*LSB_TO_1V8,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_1V8]*LSB_TO_1V0/60.*30.3,"[mA] "); // 30.3 =1/0.033
            	 printf("\n3V3\t");
            	 printf("\t %.1f %s",(float)adc_buffer[V_3V3]*LSB_TO_3V3,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_3V3]*LSB_TO_3V3/60.*16.13,"[mA] "); // 16.13 = 1/0.062
            	 printf("\t %.1f %s",(float)adc_buffer[I_3V3_SC]*LSB_TO_1V0/60.*1.22,"[mA SC] "); // 1.22 = 1/0.82
            	 printf("\nP3V3\t");
            	 printf("\t %.1f %s",(float)adc_buffer[V_AN_P5V]*LSB_TO_3V3,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_P5V_ANA]*LSB_TO_1V0/60.*12.2,"[mA] ");
            	 printf("\nN3V3\t");
            	 float Ua =(float)adc_buffer[V_AN_N5V]*LSB_TO_1V0;
            	 printf("\t %.1f %s",(14.*((2400.-Ua)/8.2-Ua/10.)-Ua),"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_N5V_ANA]*LSB_TO_1V0/60.*12.2,"[mA] ");
            	 printf("\n5V\t");
            	 printf("\t %.1f %s",(float)adc_buffer[V_GPS_5V]*LSB_TO_5V,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_GPS_5V]*LSB_TO_1V0/60.*10,"[mA] ");
            	 printf("\n12V Radio");
            	 printf("\t %.1f %s",(float)adc_buffer[V_RADIO_12V]*LSB_TO_12V,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_RADIO_12V]*LSB_TO_1V0/60.*30.3,"[mA] ");
            	 printf("\n12V PMTs");
            	 printf("\t %.1f %s",(float)adc_buffer[V_PMTS_12V]*LSB_TO_12V,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_PMTS_12V]*LSB_TO_1V0/60.*30.3,"[mA] ");
            	 printf("\n24V EXT1/2");
            	 printf("\t %.1f %s",(float)adc_buffer[V_EXT1_24V]*LSB_TO_24V,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[V_EXT2_24V]*LSB_TO_24V,"[mV] ");
            	 printf("\t %.1f %s",(float)adc_buffer[I_V_INPUTS]*LSB_TO_1V0/60.*21.28,"[mA] "); // 21.28=1/0.047
            	 printf("\nTPCB \n");
            	 printf("BAT1/2/EXT_TEMP \t%.2f %.2f %.2f\n",
            			 adc_buffer[BAT1_TEMP]*0.3662/2.-273.15,
						 adc_buffer[BAT2_TEMP]*0.3662/2.-273.15,
						 adc_buffer[EXT_TEMP]*0.3662/2.-273.15);
            	 printf("BAT_CENT/OUT\t%.2f %.2f [V]\n",
            			 (float)adc_buffer[BAT_CENT]*LSB_TO_5V*7.6/1000.,
						 (float)adc_buffer[BAT_OUT]*LSB_TO_5V*7.8/1000.);
            	 printf("LOADCURR    \t%.2f [A]\n",adc_buffer[LOADCURR]*LSB_TO_5V/48.);
            	 printf("SP_VOLT/OUT \t%.2f [V] %.2f [A]\n",
            			 (float)adc_buffer[SP_VOLT]*LSB_TO_5V*50./5000.,
						 (float)adc_buffer[SP_CURR]*LSB_TO_5V*5./1000.);
//            	 printf("P12V_LI P12V_HI1/2/3 \t %.2f %.2f %.2f %.2f\n",
//            			 (float)adc_buffer[P12V_LI],
//						 (float)adc_buffer[P12V_HI_1],
//						 (float)adc_buffer[P12V_HI_2],
//						 (float)adc_buffer[P12V_HI_3]);
            	 printf("\nSensors ");
            	 printf ("\n T= %.1f C\n P= %.1f mBar\n H= %.1f %% \n",
            			 (float) adc_buffer[T_AIR]/10.,
            			 (float) adc_buffer[P_AIR]/10.,
            			 (float) adc_buffer[H_AIR]/10.);
            	 printf (" T_WAT %.1f K",adc_buffer[WAT_LVL]*0.3662/2.);


            	 printf ("\n");
                 break;
             case 'P': // switch ps on/of

            	 if (optarg) {
            		 i = (int) strtol (optarg,NULL, 0);


            		 buf [1] = (char) (i >> 8);
            		 buf [0] = (char) (i & 0x00ff);
            		 sc_powerControl_reg_w (file,buf);

            	 }
            	 else {
            	 sc_powerControl_reg(file, buf);
            	 printf ("Powercontrol reg: ");
            	 printf("0x%x", buf[1]);
            	 printf("%x\n", buf[0]);
            	 }

            	 break;
             case 'd':
            	 val = (int) strtol (optarg, NULL, 0);
            		 sc_ANALOG (file,val);


            	 break;

             case 't': // test register (returns 0x4321)
            	 sc_test_reg(file, buf);
            	 printf ("Testreg: ");
            	 printf("0x%x", buf[1]);
            	 printf("%x\n", buf[0]);
            	 break;

             case 'h':   /* fall-through is intentional */
             case '?':
                 display_usage(argv[0]);
                 break;

             default:
                 /* You won't actually get here. */
                 break;
         }

         opt = getopt( argc, argv, optString );
     }




  return 0;
  }




