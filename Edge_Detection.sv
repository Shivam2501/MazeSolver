//-------------------------------------------------------------------------
//    Color_Mapper.sv                                                    --
//    Stephen Kempf                                                      --
//    3-1-06                                                             --
//                                                                       --
//    Modified by David Kesler  07-16-2008                               --
//    Translated by Joe Meng    07-07-2013                               --
//                                                                       --
//    Fall 2014 Distribution                                             --
//                                                                       --
//    For use with ECE 385 Lab 7                                         --
//    University of Illinois ECE Department                              --
//-------------------------------------------------------------------------


module edge_detection (
    input        [9:0]  DrawX, DrawY,
    output        CE, LB, UB, OE, WE, 
    output        [19:0] SramAddr,
    input        [15:0] SramDQ,
    output logic [7:0]  Red, Green, Blue
);

 logic [7:0] gridx0, gridx1, gridx2, gridx3, gridx4, gridx5, gridx6, gridx7, gridx8;
 logic [7:0] gridy0, gridy1, gridy2, gridy3, gridy4, gridy5, gridy6, gridy7, gridy8;

 logic [13:0] x_edges, y_edges;

 logic [16:0] mix;

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
        gridx0 = -SramDQ[7:0];
        gridy0 = -SramDQ[7:0];

        SramAddr = base_addr - 20'b01010000000;
        gridx1 = 0;
        gridy1 = -(SramDQ[7:0] << 1);

        SramAddr = base_addr - 20'b01010000000 + 1'b1;
        gridx2 = SramDQ[7:0];
        gridy2 = -SramDQ[7:0];

        SramAddr = base_addr - 1'b1;
        gridx3 = -(SramDQ[7:0] << 1);
        gridy3 = 0;

        SramAddr = base_addr ;
        gridx4 = 0;
        gridy4 = 0;

        SramAddr = base_addr + 1'b1;
        gridx5 = SramDQ[7:0] << 1;
        gridy5 = 0;

        SramAddr = base_addr + 20'b01010000000 - 1'b1;
        gridx6 = -SramDQ[7:0];
        gridy6 = SramDQ[7:0];

        SramAddr = base_addr + 20'b01010000000;
        gridx7 = 0;
        gridy7 = SramDQ[7:0] << 1;

        SramAddr = base_addr + 20'b01010000000 + 1'b1;
        gridx8 = SramDQ[7:0];
        gridy8 = SramDQ[7:0];

        x_edges = gridx0 + gridx1 + gridx2 + gridx3 + gridx4 + gridx5 + gridx6 + gridx7 + gridx8;
        y_edges = gridy0 + gridy1 + gridy2 + gridy3 + gridy4 + gridy5 + gridy6 + gridy7 + gridy8;


    end 
    
endmodule
