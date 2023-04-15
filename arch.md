# HH23 Rhythm SoC/GPU "Bug32"

There are N elements to this SoC split into two complexes.

The first complex is the CPU complex. It consists of a PicoRV32 core, and two/three Wishbone SPI interfaces.

One SPI interface (SPI0) interacts with the SD card. This card contains three vital files: game software, rhythm map data, and music data.  In the future, this can be structured such that multiple levels can be selected loaded. The other SPI interface (SPI1) communicates with the Infineon PSoC board, running a serial communication firmware that provides finger position data and, optionally, muscle contraction data. 

Startup is a 2-stage process. At power-on, the FPGA is reset and the bitstream is loaded. This includes loading a small Boot ROM. This Boot ROM polls SPI1 until the SD card is detected successfully. It communicates with the card using 1nv1's ulibSD library and ELM-ChaN's FatFs library. This process will copy a 4 MB system image containing the game and Zephyr RTOS from the card to DRAM before closing the SD card interface and jumping to Zephyr's entrypoint.

The second complex is the GPU complex. It is connected to the CPU via the CPU's Wishbone open peripheral bus. The GPU has two parts: the graphics engine and the display engine. 

The graphics engine generates the requested pixel by requesting sprites from the Sprite Cache. The Sprite Cache is a 32-entry CAM that contains sprite entries with dimensions and texture pointer for sprites that are present on a given line. This is double-ported, with the sprite list for the next raster line being generated while the current line is displayed. This generation occurs by iterating the Sprite SRAM (2 KB). The GE will only select one candidate sprite from the CAM. The GE will then query its Texture SRAM (16 KB) for the necessary pixel index and then perform a lookup in the LUT RAM (64 entries split into 4 sub-LUTs), passing it up to the display engine.

The GE can also be put into the upload mode. This mode enables writes to the Texture SRAM and the Sprite SRAM. This should be done during V-sync. Textures are aligned to 4 bytes, with the internal sprite storage structure using a 16-bit address to address them. The bottom 2 bits of the texture address are thus instead repurposed as a LUT select, allowing the selection of subLUTs 0 to 3.

The graphics API will look like this:

```c

struct gpu_device; // Stores base address and SRAM allocator information.
typedef gpu_device_base_address volatile uint32_t*;
typedef image uint8_t;
typedef texture_ref uint32_t;
typedef lut_entry uint16_t;
typedef lut_id uint32_t;

void gpu_device_init(gpu_device* device, gpu_device_base_address* address);
void gpu_wait_vsync(gpu_device* device);

void gpu_clear_sprites(gpu_device* device);
void gpu_clear_textures(gpu_device* device);
void gpu_clear_luts(gpu_device* device);

void gpu_begin_upload(gpu_device* device);
texture_ref gpu_upload_LUT(gpu_device* device, lut_entry* lut);
texture_ref gpu_upload_texture(gpu_device* device, image* texture, lut_id lut);
void gpu_upload_sprite(gpu_device* device, texture_ref texture, uint32_t width, uint32_t height, uint32_t x, uint32_t y);
void gpu_end_upload(gpu_device* device);

```

