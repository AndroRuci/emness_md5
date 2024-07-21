// md5 Core ZYNQ APSoC © 2024 by Filippo Borghetto, Alex Turiani, Andro Ruci is //licensed under CC BY-NC 4.0 

module dataInOut (
    //Generic signals
    input wire 	       clk,
    input wire 	       reset,
    //Input control signal
    input wire 	       start_encoding, 
    //Input data values
    input wire [31:0]  data_in_0,
    input wire [31:0]  data_in_1,
    input wire [31:0]  data_in_2,
    input wire [31:0]  data_in_3,
    input wire [31:0]  data_in_4,
    input wire [31:0]  data_in_5,
    input wire [31:0]  data_in_6,
    input wire [31:0]  data_in_7,
    input wire [31:0]  data_in_8,
    input wire [31:0]  data_in_9,
    input wire [31:0]  data_in_10,
    input wire [31:0]  data_in_11,
    input wire [31:0]  data_in_12,
    input wire [31:0]  data_in_13,
    input wire [31:0]  data_in_14,
    input wire [31:0]  data_in_15,
    //output data values
    output reg [127:0] data_out,
    //output status signal
    output reg         ready_output
);

   //Declaretion on registers 
   reg [127:0] data_register_0;
   reg [127:0] data_register_1;
   reg [127:0] data_register_2;
   reg [127:0] data_register_3;

   //Signals needed to the cryptocore
   reg 	        start_crypto; //new string to encode	       
   reg 	        inp_ready_crypto; //says to crypto to take the value		
   wire [127:0] crypt_message_out; //Hash generated 
   wire 	crypto_done;
   reg [127:0]  crypt_message_in; //input of the cryptocore
   
   //Signals needed to the Control Unit
   reg [3:0]   state, next_state;
   reg 	       next_start_crypto;
   reg 	       next_inp_ready_crypto;
   reg [1:0]   next_selector;
   reg [127:0] next_crypt_message_in;
   
   
   //Instantiation of the cryptocore
   md5 m1(clk, reset, inp_ready_crypto, crypto_done,
	  start_crypto, crypt_message_in, crypt_message_out);

   //Needed to map proprerly the cryptocore's output
   reg [4:0]   map[31:0];
   integer     i;
   initial 
     begin
	map[0]  = 6;
	map[1]  = 7;
	map[2]  = 4;
	map[3]  = 5;
	map[4]  = 2;
	map[5]  = 3;
	map[6]  = 0;
	map[7]  = 1;
	map[8]  = 14;
	map[9]  = 15;
	map[10] = 12;
	map[11] = 13;
	map[12] = 10;
	map[13] = 11;
	map[14] = 8;
	map[15] = 9;
	map[16] = 22;
	map[17] = 23;
	map[18] = 20;
	map[19] = 21;
	map[20] = 18;
	map[21] = 19;
	map[22] = 16;
	map[23] = 17;
	map[24] = 30;
	map[25] = 31;
	map[26] = 28;
	map[27] = 29;
	map[28] = 26;
	map[29] = 27;
	map[30] = 24;
	map[31] = 25;
     end // initial begin
   

   always @*
     begin
	ready_output = crypto_done;
     end
   
   //Concatenation of the slv_regs to the data_registers
   always @(posedge clk)
     begin	
	data_register_0[127:96] = data_in_0;
        data_register_0[95:64]  = data_in_1;
        data_register_0[63:32]  = data_in_2;
        data_register_0[31:0]   = data_in_3;
        
        data_register_1[127:96] = data_in_4;
        data_register_1[95:64]  = data_in_5;
        data_register_1[63:32]  = data_in_6;
        data_register_1[31:0]   = data_in_7;
        
        data_register_2[127:96] = data_in_8;
        data_register_2[95:64]  = data_in_9;
        data_register_2[63:32]  = data_in_10;
        data_register_2[31:0]   = data_in_11;
        
        data_register_3[127:96] = data_in_12;
        data_register_3[95:64]  = data_in_13;
        data_register_3[63:32]  = data_in_14;
        data_register_3[31:0]   = data_in_15;
     end // always @ (posedge clk)

   
   //Control unit to manage the cryptocore
   //  States:
   // 0 : Idle
   // 1 : Start_Cryp
   // 2 : Dato1
   // 3 : Wait_D1
   // 4 : Dato2
   // 5 : Wait_D2
   // 6 : Dato3
   // 7 : Wait_D3
   // 8 : Dato4
   // 9 : Wait_D4
   always @(posedge clk, negedge reset)
     begin	
	if(reset == 1'b0)
	  begin
	     //Reset occours	     
	     state = 4'b0000;
	     next_state = 4'b0000;
	     
	     start_crypto = 0;
	     next_start_crypto = 0;
	     
	     inp_ready_crypto = 1'b0;
	     next_inp_ready_crypto = 1'b0;
	     
	  end
	else
	  begin
	     state = next_state;
	     start_crypto = next_start_crypto;
	     inp_ready_crypto = next_inp_ready_crypto;
	     crypt_message_in = next_crypt_message_in;
	     case(state)

	       4'b0000: //Idle
		 begin
		    if(start_encoding)
		      begin
			 next_state = 4'b0001;
			 next_start_crypto = 1;
		      end
		    else
		      begin
			 next_state = 4'b0000;
		      end
		 end

	       4'b0001: //Start_Cryp
		 begin
		    next_state = 4'b0010;
		    next_start_crypto = 0;
		    next_inp_ready_crypto = 1;
		    next_crypt_message_in = data_register_0;
		 end
	       
	       4'b0010: //Dato1
		 begin
		    next_state = 4'b0011;
		    next_inp_ready_crypto = 0;
		 end

	       4'b0011: //Wait_D1
		 begin
		    next_state = 4'b0100;
		    next_inp_ready_crypto = 1;
		    next_crypt_message_in = data_register_1;
		 end

	       4'b0100: //Dato2
		 begin
		    next_state = 4'b0101;
		    next_inp_ready_crypto = 0;
		 end

	       4'b0101: //Wait_D2
		 begin
		    next_state = 4'b0110;
		    next_inp_ready_crypto = 1;
		    next_crypt_message_in = data_register_2;
		 end

	       4'b0110: //Dato3
		 begin
		    next_state = 4'b0111;
		    next_inp_ready_crypto = 0;
		 end

	       4'b0111: //Wait_D3
		 begin
		    next_state = 4'b1000;
		    next_inp_ready_crypto = 1;
		    next_crypt_message_in = data_register_3;
		 end

	        4'b1000: //Dato4
		 begin
		    next_state = 4'b1001;
		    next_inp_ready_crypto = 0;
		 end

	       4'b1001: //Wait_D4
		 begin
		    next_state = 4'b0000;
		 end

	     endcase // case (state)
	  end // else: !if(reset == 1'b0)
     end // always @ (posedge clk, negedge reset)

   
  /* //Process to display correctly the hash
   always @(posedge clk)
     begin
	     for (i = 0; i < 32; i = i + 1)
	       begin
		  data_out[i*4 +: 4] = crypt_message_out[map[i]*4 +: 4];
	       end
     end
*/
   always @(posedge clk)
     begin
	if(crypto_done) 
	  begin
	     for (i = 0; i < 32; i = i + 1)
	       begin
		  data_out[i*4 +: 4] = crypt_message_out[map[i]*4 +: 4];
	       end
	  end
     end
   
	       
endmodule // dataInOut



