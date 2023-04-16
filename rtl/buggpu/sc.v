/*
Bug32 GPU
(C) 2023 Davit Margarian

Spritecache

Sprite properties CAM based on horizontal-position.
*/

`include sprite_defs.vh

module spritecache(
    input clk,
    input reset,

    input [9:0] hcursor,
    output reg [63:0] found_sprite,

    input stb, 
    output ack,
    input wr,
    input [63:0] wr_sprite,
    input [4:0] wr_idx
);

    reg [63:0] sprites[32];

    int i;

    always @(posedge clk) begin
        if(stb) begin
            if(wr)
                sprites[wr_idx] <= wr_sprite;
            else begin
                for(i=0; i < 31; i=i+1) begin
                    if(SPRITE_Hpos(sprites[i]) <= hcursor && hcursor <= SPRITE_Hpos(sprites[i]) + SPRITE_Width(sprites[i])) begin
                        found_sprite <= sprites[i];
                        ack <= 1;
                    end
                end
            end
        end else begin
            ack <= 0;
        end
    end

endmodule