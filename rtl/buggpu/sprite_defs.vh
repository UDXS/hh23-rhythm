/*
Bug32 GPU
(C) 2023 Davit Margarian

Sprite engine
Sprite = {16'Vpos, 16'Hpos, 8'Width, 8'Height, 16'Address}
*/

`ifndef SPRITE_DEFS

`define SPRITE_DEFS

`define SPRITE_Vpos(_S) (_S[63:48])
`define SPRITE_Hpos(_S) (_S[47:32])
`define SPRITE_Width(_S) (_S[31:24])
`define SPRITE_Height(_S) (_S[23:16])
`define SPRITE_Address(S) (_S[15:0])

`endif