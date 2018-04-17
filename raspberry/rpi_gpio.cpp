///////////////////////////////////////
// Module:	rpi_gpio
// Remarks:
//	Raspberry PI3 GPIO access functions
///////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#include "rpi_gpio.h"

// I/O access
volatile unsigned *gpio;
int  mem_fd;
void *gpio_map;

//
// Set up a memory regions to access GPIO
//
int setup_rpi_gpio()
{
	bool pi_zero = false;
	try {
		ifstream t;
		t.open("/proc/cpuinfo");
		if (!t.is_open())
		{
			cout << "Cannot identify CPU model\n";
		}
		else
		{
			string s;
			int n = 0;
			while (getline(t, s))
			{
				size_t pos = s.find("BCM2835");
				if (pos != string::npos)
				{
					//found 2835, Pi-zero W
					cout << "CPU model BCM2835\n";
					pi_zero = true;
					break;
				}
				pos = s.find("BCM2709");
				if (pos != string::npos)
				{
					//found 2709, RPI3
					cout << "CPU model BCM2709\n";
					pi_zero = false;
					break;
				}
				n++;
				if (n == 128)
					break; //too many lines
			}
		}
	}
	catch (...) {
	}

	unsigned int gpio_base_addr = GPIO_BASE+ (pi_zero ? BCM2835_PERI_BASE : BCM2709_PERI_BASE);

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      return -1;
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      gpio_base_addr    //Offset to GPIO peripheral
   );
 
   close(mem_fd); //No need to keep mem_fd open after mmap
 
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      return -1;
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
   return 0;
}
