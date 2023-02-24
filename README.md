# Solar-MPPT
***************************************************************
The basic functionality of the device is based on previous generation Solar MPPTs, but it is fully autonomous (alpha ver) using ESP32 as microcontroller.
The file named `MPPT_Port-Xtensa.ino` code is final port of MPPT on the ESP32. Also, `MPPT_Port-328P.ino` shows an overview of how MPPT will work with ATMega328P microcontroller (with reduced functionality).It is equipped with 20*4 Character LCD (for device & energy status), Function Buttons, ESP32 Microcontroller, Solar Inlet, 30A FUSE, Sleep Mode, Webserver, 2 x PT-100(for Indoor & Outdoor Temp.), RS-232 Interface(at 9600),Program Serial(at 115200), Internal DIP Switches (for system configuration), Relay driven Outputs(for automation purpose), I/O for interfacing external sensors and automation purpose. The live status is displayed on LCD, the Webserver(at 192.168.1.1) to freely receive updates on PC/smartphone as well as the data log can be viewed via both RS-232 (DB9) or USB A Serial ports. Refer README.md and attached datasheets for furthur details on MPPT.



 IR Remote Keypad Hex Map:
  
        +------------------+              
        |              DC  |
        |                  |
        |  92    93    CC  |
        |  8E    8F    C8  |
        |  8A    8B    C4  |
        |  CF    87    82  |
        |                  |
        |  80    9C    85  |
        |  81  8C  DD  86  |
        |                  |
        |        CA        |
        |  88 99 CE C1 C5  |
        |        D2        |
        |                  |
        |  DA          D6  |
        |      98  95      |
        |                  |
        |  CD    91    D0  |
        |  8D  97  C3  83  |
        |                  |
        |  89  84  D9  96  |
        |                  |
        |     hathw@y      |
        +------------------+
