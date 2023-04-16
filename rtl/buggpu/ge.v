/*
Bug32 GPU
(C) 2023 Davit Margarian

Graphics Engine
Primary sprite rendering orchestrator.
*/

module gpu_ge(
    input reset,
    input clk,

    input [9:0] wcursor_i;
    input [9:0] hcursor_i;
    // Strobed when the Display Engine has a new required pixel.
    input cursor_valid_i; 

    output [24:0] pixel_o;
    output reg pixel_ready_o;
    
    input stb_i;
    input cyc_i;
    input we_i;
    output reg ack_o;

    input [31:0] adr_i,
    input [31:0] dat_i
);

localparam STATE_RENDER_READY = 3'b000;
localparam STATE_PIXEL_REQUEST = 3'b001;
localparam STATE_PIXEL_WAIT_SPRITECACHE = 3'b010;
localparam STATE_PIXEL_WAIT_TEXTURE = 3'b011;
localparam STATE_UPLOAD_IDLE = 3'b100;
localparam STATE_UPLOAD_WAIT = 3'b101;

reg [2:0] state;
wire [15:0] dev_adr;
assign dev_adr = adr_i & 32'h000FFFFF;

always @(posedge clk) begin
    if(!reset) begin
        case(state) 
            STATE_RENDER_READY: begin
                if (stb && dev_adr == 20'h68086) begin // Prioritize host upload req.
                    STATE <= STATE_UPLOAD_IDLE;
                else 
                    if(cursor_valid_i) begin
                        state <= STATE_PIXEL_REQUEST;
                        sce_stb <= 1;
                        sce_vcursor <= vcursor_i;
                        sce_hcursor <= hcursor_i;
                    end
                end
            end
            STATE_PIXEL_REQUEST: begin
                sce_stb <= 0;
                if(sce_ready) begin 
                    state <= STATE_PIXEL_WAIT_TEXTURE;
                    sprite_data <= sce_out;
                    texture_stb <= 1;
                end
            end
            STATE_PIXEL_WAIT_TEXTURE: begin
                texture_stb <= 0;
                
                state <= STATE_PIXEL_WAIT_TEXTURE;
            end
    end else begin
        state <= STATE_RENDER_READY;
    end 
end


endmodule;