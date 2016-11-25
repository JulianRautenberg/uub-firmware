// Data Acquisition from ADC data to file
// implemented USB write file if memory key is connected
// written by R.Assiro and G.Marsella 02 2016

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char *argv[])
{
	int fd, file,i,j, Status, data_trig, ord;
	int nbin = 4096;
	int int_trig =  0x55000000;
	int stop_trig = 0x56000000;
	int value = 0;
	unsigned page_addr, page_offset;
	void *ptr,*pt[5],*ptrt,*ptrt1;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	page_offset = 16;
	FILE *fp, *fp1, *fp2;
	int nevt=0;
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (argc>1) ord = argv[1];
	char filename1[100];
	char filename2[100];

	if (argc < 2 || argc > 2){
			usage();
			exit(1);
	}
	printf("Acquire is running...\n");
	i=system ("umount /usb 2>/dev/null");
	i=system ("mountusb 2>/dev/null"); //mountusb is a bash script
//	printf ("The value returned was: %d.\n",i);
	if (i == 0){
		  sprintf(filename1,"/usb/adc_data%s%s.dat", argv[1], asctime(timeinfo));
		  printf("Writing file on USB...\n");
	}
	else {
		  sprintf(filename1,"/home/root/adc_data%s%s.dat", argv[1], asctime(timeinfo));
		  printf("Writing file on system...\n");
	}
	fp1 = fopen (filename1, "w" );
	char c;

	while(nevt<10)
	{
		fp = fopen ("/srv/www/adc_data.json", "w" );
		/* Open /dev/mem file */
		fd = open ("/dev/mem", O_RDWR);

		//////////////// TRIGGER ////////////////////////////////////////
		/* mmap the device into memory */
		page_addr = (int_trig & (~(page_size-1)));
		page_offset = int_trig - page_addr;
		ptrt = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);

		//////////////// INTERNAL TRIGGER  ////////////////////////////////////////
		if (!strcmp(argv[1], "-i")){
					value = 0b00000111; //bit 0 trigger interno a FPGA. Aquisisco segnali in BRAM (bit 1 = trigger interno)
					*((unsigned *)(ptrt + page_offset)) = value;// Write value to the device register
					value = 0b00000010; //Alzo segnale trigger interno ( acquisizione continua)
					*((unsigned *)(ptrt + page_offset)) = value;
		}
		//////////  EXTERNAL TRIGGER  //////////////////////////////////
		if (!strcmp(argv[1], "-e")){
			data_trig = 0;
			value = 0b10000000; //impostazione per trigger esterno
			*((unsigned *)(ptrt + page_offset)) = value;// Write value to the device register
			usleep(1);
			value = 0b00000000;
			*((unsigned *)(ptrt + page_offset)) = value;

			printf("Waiting trigger... ");
			page_addr = (stop_trig & (~(page_size-1)));	//		data_trig = *stop_trig;
			page_offset = stop_trig - page_addr;
			ptrt1 = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
			Status = 0;
			while(Status != 1)		// wait trigger
			{
				data_trig = *((unsigned *)(ptrt1 + page_offset));
				if (data_trig > 0b1000000000000000) {
					Status = 1;
					data_trig = data_trig & 0b0111111111111111;
					data_trig = (data_trig + 4095 * 4 + 500) % (4095 * 4);
					page_offset=data_trig;
				}
			}
			printf ("Event %d OK\n",nevt);
		}
		//////////////// FPGA  TRIGGER ////////////////////////////////////////
		if (!strcmp(argv[1], "-f")){
					data_trig = 0;
					value = 0b10010000; //impostazione per trigger da fpga
					*((unsigned *)(ptrt + page_offset)) = value;// Write value to the device register
						usleep(1);
					value = 0b00010000;
					*((unsigned *)(ptrt + page_offset)) = value;
					printf("Waiting trigger... ");
					page_addr = (stop_trig & (~(page_size-1)));	//		data_trig = *stop_trig;
					page_offset = stop_trig - page_addr;
					ptrt1 = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
					Status = 0;
					while(Status != 1)		// wait trigger
					{
						data_trig = *((unsigned *)(ptrt1 + page_offset));
						if (data_trig > 0b1000000000000000) {
								Status = 1;
								data_trig = data_trig & 0b0111111111111111;

				                page_offset=(data_trig + 4095 * 4 -100) % (4095 * 4);
					    }
					}
					printf ("Done! ... FPGA event %d\n",nevt);
					//	sleep (1);
		}

		//////////////// Nitz FPGA TRIGGER ////////////////////////////////////////
		if (!strcmp(argv[1], "-n")){
					data_trig = 0;
					value = 0b10100000; //impostazione per trigger da fpga
					*((unsigned *)(ptrt + page_offset)) = value;// Write value to the device register
						usleep(1);
					value = 0b00100000;
					*((unsigned *)(ptrt + page_offset)) = value;
					printf("Waiting trigger... ");
					page_addr = (stop_trig & (~(page_size-1)));	//		data_trig = *stop_trig;
					page_offset = stop_trig - page_addr;
					ptrt1 = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
					Status = 0;
					while(Status != 1)		// wait trigger
					{
						data_trig = *((unsigned *)(ptrt1 + page_offset));
						if (data_trig > 0b1000000000000000) {
								Status = 1;
								data_trig = data_trig & 0b0111111111111111;
				                page_offset=(data_trig + 4095 * 4 -100) % (4095 * 4);
					    }
					}
					printf ("Done! ... Nitz event %d\n",nevt);
					//	sleep (1);
		}

		unsigned int bram[5];
		int w, ADC0A[5], ADC0B[5];
		bram[0] = 0x50000000;// ADC 1
		bram[1] = 0x51000000;// ADC 2
		bram[2] = 0x52000000;// ADC 3
		bram[3] = 0x53000000;// ADC 4
		bram[4] = 0x54000000;// ADC 5
		fprintf(fp1,"\nEvent %d\n",nevt);

		for(i=0;i<5;i++){
			pt[i] = mmap(NULL, page_size*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, bram[i]);
		}
			nevt++;
			fprintf(fp,"[");
			for (i =0; i<nbin; i++) //
			{
				fprintf(fp,"{");
				for (j=0; j<5; j++) {
					ADC0A[j] = *((unsigned *)(pt[j] + page_offset));
					ADC0B[j] =ADC0A[j]&0x1fff;

					fprintf(fp1,"%4d\t",(ADC0A[j]>>16)&0x1fff);
					fprintf(fp1,"%4d\t", ADC0B[j]);
					fprintf  (fp,"\"adc%d\": \"%d\"",j*2, (ADC0A[j]>>16)&0x1fff);
					fprintf(fp,", \"adc%d\": \"%d\"",j*2+1, ADC0B[j]);
					if (j != 4) {fprintf(fp,", ");}
				}
				page_offset=(page_offset+4)&0x3ffc;
				fprintf(fp,"}");
				if (i!=nbin-1)  fprintf(fp,", ");
			}
			fprintf(fp,"]");
			fclose(fp);
			usleep(10000);
		} //close event loop!!
		printf("Done!\n\r");
		fclose(fp1);
		i=system ("umount /usb 2>/dev/null");
	//	return 0;


}
void usage(void)
{
	printf("____________________________\n");
	printf("|         ACQUIRE          |\n");
	printf("|   -e External SMA input  |\n");
	printf("|   -i internal trigger    |\n");
	printf("|   -f FPGA Lecce trigger  |\n");
	printf("|   -n FPGA Nitz trigger   |\n");
	printf("|                          |\n");
	printf("|    written by R.Assiro   |\n");
	printf("|__________________________|\n");
	exit(1);
}

