
CXFLAGS+=-std=c++11 -O3 -fPIC -Wall -DX32
LDFLAGS+=-lpthread -Wl,-no-whole-archive

PHONY: send2fpga

send2fpga: clean
	g++ -o send2fpga $(CXFLAGS) send2fpga.cpp rpi_gpio.cpp $(LDFLAGS)

clean:
	rm -f send2fpga



