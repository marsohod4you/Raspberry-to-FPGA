
module m2rpi(
	input wire OSC,
	input wire [2:0]KEY,
	output wire [3:0]LED,
	inout wire [27:0]GPIO_A,
	inout wire [27:0]GPIO_B,
	
	//Raspberry GPIO pins
	input wire GPIO0,
	input wire GPIO1,
	output wire GPIO2,
	input wire GPIO3,
	input wire GPIO4,
	input wire GPIO5,
	input wire GPIO6,
	input wire GPIO7,
	input wire GPIO8,
	input wire GPIO9,
	input wire GPIO10,
	input wire GPIO11,
	input wire GPIO12,
	input wire GPIO13,
	input wire GPIO14, //can be Serial RX
   input wire GPIO15, //can Serial TX
	input wire GPIO16,
	input wire GPIO17,
	input wire GPIO18,
	input wire GPIO19,
	input wire GPIO20,
	input wire GPIO21,
	input wire GPIO22,
	input wire GPIO23,
	input wire GPIO24,
	input wire GPIO25,
	input wire GPIO26,
	input wire GPIO27
	);
	
wire w_clk;
wire w_clk1;
wire w_clk2;
wire w_locked;

pll my_pll_inst(
	.inclk0(OSC ),
	.c0( w_clk  ),
	.locked( w_locked )
	);

wire gpio_clk; assign gpio_clk = GPIO4;

wire [15:0]w_input_data;
assign w_input_data = 
		{	
			GPIO27,GPIO26,GPIO25,GPIO24,
			GPIO23,GPIO22,GPIO21,GPIO20,
			GPIO19,GPIO18,GPIO17,GPIO16,
			GPIO15,GPIO14,GPIO13,GPIO12
		};
		
reg [15:0]r_input_data;
always @( negedge gpio_clk )
		r_input_data <= w_input_data;

wire [15:0]o_data;
wire [9:0]w_wrusedw;
wire [9:0]w_rdusedw;
wire w_wrfull;
wire w_rdfull;
wire w_rdempty;

data_fifo fifo_inst(
	.data( r_input_data ),
	.wrreq( 1'b1 ),
	.wrfull( w_wrfull ),
	.wrusedw( w_wrusedw ),
	.wrclk( gpio_clk ),

	.rdclk( w_clk ),
	.rdreq( ~w_rdempty ),
	.q( o_data ),
	.rdfull( w_rdfull ),
	.rdempty( w_rdempty ),
	.rdusedw( w_rdusedw )
	);

reg [10:0]rd_fullness;
always @(posedge w_clk)
	rd_fullness <= { w_rdfull,w_rdusedw };
	
//request new data from RPI
assign GPIO2 = rd_fullness < 16'd768;

reg [15:0]r_data;
always @(posedge w_clk)
	r_data <= o_data;

assign LED = r_data[7:4];
assign GPIO_A[15:0] = r_data;

endmodule
