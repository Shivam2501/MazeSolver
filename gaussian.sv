
module gaussian (
    input        [9:0]  DrawX, DrawY,
    output        CE, LB, UB, OE, WE, 
    output        [19:0] SramAddr,
    input        [15:0] SramDQ,
    output logic [7:0]  Red, Green, Blue
);

 logic [7:0] gridx0, gridx1, gridx2, gridx3, gridx4, gridx5, gridx6, gridx7, gridx8;

 logic [13:0] gradient;


 assign x_pos = {10'b0,DrawX};
 assign y_pos = {10'b0,DrawY};
 assign base_addr = (x_pos * 20'b01010000000) + y_pos;

    always_comb
    begin
        
        WE = 1;
        CE = 0;
        LB = 0;
        UB = 0;
        OE = 0;

        SramAddr = base_addr - 20'b01010000000 - 1'b1;
        gridx0 = SramDQ[7:0];

        SramAddr = base_addr - 20'b01010000000;
        gridx1 = SramDQ[7:0] << 1;

        SramAddr = base_addr - 20'b01010000000 + 1'b1;
        gridx2 = SramDQ[7:0];

        SramAddr = base_addr - 1'b1;
        gridx3 = SramDQ[7:0] << 1;

        SramAddr = base_addr ;
        gridx4 = SramDQ[7:0] << 2;

        SramAddr = base_addr + 1'b1;
        gridx5 = SramDQ[7:0] << 1;

        SramAddr = base_addr + 20'b01010000000 - 1'b1;
        gridx6 = SramDQ[7:0];

        SramAddr = base_addr + 20'b01010000000;
        gridx7 = SramDQ[7:0] << 1;

        SramAddr = base_addr + 20'b01010000000 + 1'b1;
        gridx8 = SramDQ[7:0];

        gradient = gridx0 + gridx1 + gridx2 + gridx3 + gridx4 + gridx5 + gridx6 + gridx7 + gridx8;


    end 
    
endmodule
