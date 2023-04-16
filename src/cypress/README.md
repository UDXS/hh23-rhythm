To load the sensing program on the Infineon board,

1. Use [Eclipse IDE for ModusToolbox](https://www.infineon.com/modustoolbox).
2. Bottom left panel, Start > New Application.
3. Wait for the window to load.
4. PSoC™️ 6 BSPs > CY8CPROTO-062-4343W. Click "Next >."
5. Wait for the list of templates to load.
6. Sensing > CAPSENSE Buttons and Slider. You can call it whatever you want; it'll create a folder with that name in the Application(s) Root Path specified at the top of the Select Application window. By default, the name is `CAPSENSE_Buttons_and_Slider`.
7. Click "Create."
8. This copies the [example project](https://github.com/Infineon/mtb-example-psoc6-capsense-buttons-slider/) this project is based on.
9. Replace the `deps` and `source` folders with the `deps` and `src` folders in this repository.
10. Select the name (e.g. `CAPSENSE_Buttons_and_Slider`) in the Project Explorer in the left panel.
11. Ensure that the Infineon board is plugged into your computer.
12. In the bottom left panel, click on Launches > CAPSENSE_Buttons_and_Slider Program (KitProg3_MiniProg4).
13. The red LED should turn on on the board.

To see the terminal,

1. Open Device Manager and look in Ports (COM & LPT).
2. Find KitProg3 USB-UART (COM*n*). Note the COM number (eg COM7).
3. Use [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).
4. Select Connection type: Serial.
5. Set Serial line to the COM number.
6. Set Speed to 115200.
7. Click "Open."

The board partakes as a SPI employee. It offers 32-bit data packets:

```sv
data[11:3] // slider_pos (9 bits), position of finger between 0 (left) and 300 (right)
data[2] // slider_touch_status, whether there's a finger on the slider
data[1] // button1_status, whether the right button is pressed
data[0] // button0_status, whether the left button is pressed
```

I'm not sure if it works though.
