///////////////////////////////////////
// Module:	nw_jtag_srv
// Author:	Nick Kovach
// Copyright (c)  2017 InproPlus Ltd
// Remarks:
//	Network Jtag Server for Linux, including RPI3 devices
//	TCP server which receives network commands for JTAG
///////////////////////////////////////

#include <stdio.h>
#include <fcntl.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h> //inet_addr
#include <netinet/tcp.h> //inet_addr
#include <unistd.h>    //write
#include <iostream>
#include <thread>
#include <mutex>
#include <math.h>

#include <stddef.h>
#include "rpi_gpio.h"
#include "send2fpga.h"

using namespace std;

void send_block(uint8_t* pdata, int len)
{
    uint16_t* pblk = (uint16_t*)pdata;
    uint32_t mask = 0xFFFF<<12;
    for(int i=0; i<len; i++)
    {
	uint32_t word = (pblk[i])<<12;
	uint32_t w_set = word | (1<<4);
	uint32_t w_clr = word ^ mask;
	GPIO_SET = w_set;
	GPIO_CLR = w_clr;
	GPIO_CLR = 1<<4;
	//GPIO_CLR = 1<<4;
    }
}

int main(int argc , char *argv[])
{
    cout << "Hello send2fpga!\n";
    if( setup_rpi_gpio() ) {
	cout << "Cannot map GPIO memory, probably use <sudo>\n";
	return -1;
    }

/*
    //JTAG pins used by nw_jtag_dll
    //turn then to input if used external JTAG programmer
    INP_GPIO(0);
    INP_GPIO(11);
    INP_GPIO(7);
    INP_GPIO(1);
*/

    //pin2 used as FIFO level signal
    INP_GPIO(2);
    //output data bus 16 bit
    for(int i=0; i<16; i++)
    {
	OUT_GPIO(12+i);
    }
    //pin4 used as write strobe
    OUT_GPIO(4);

    double t = 0;
    const uint32_t len = 256;
    uint8_t block[len*2];
    //make data block
    for(uint32_t i=0; i<len; i++)
    {
	block[i*2+0]=128+(int8_t)(127*sin( t )); t+=2.0*M_PI/256;
	block[i*2+1]=i;
    }

    while(1)
    {
	//wait FPGA says need data
	while(1)
	{
		//wait request from FPGA
		uint32_t req = GET_GPIO(2);
		if( req )
			break;
	}

	//send block to fpga
	send_block(block,len);
    }

    return 0;
}
