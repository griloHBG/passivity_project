##### NMT commands

Message format: 

|CAN id| DATA0 |   DATA1   | DATA2 | DATA3 | DATA4 | DATA5 | DATA6 | DATA7 |
|------|-------|-----------|-------|-------|-------|-------|-------|-------|
| 0x00 | state | node addr | ----- | ----- | ----- | ----- | ----- | ----- |

#### State:

|     State         | Value |
|-------------------|-------|
|Operational        | 0x01  |
|Pre-Operational    | 0x80  |
|Stopped            | 0x02  |
|Reset Application  | 0x81  |
|Reset Communication| 0x82  |


##### Rx and TX PDO listing

|Summary |COB-ID	   |Binary (11 bits)|(A/S)ync|Valid  |Obj 0 (num of bytes), Obj 1 (num of bytes), ...                               |
|--------|-------------|----------------|---------|-------|------------------------------------------------------------------------------|
| RxPDO1 |0x00000201   |010 0000 0001   |Async    |YES    |PositionMode Setting Value (4), CurrentMode Setting Value (2), ControlWord (2)|
| RxPDO2 |0x80000301   |011 0000 0001   |Async    |NO     |VelocityMode Setting Value (4)                                                |
| RxPDO3 |0x00000401   |100 0000 0001   |Async    |YES    |ControlWord (2), Target Position (4)                                          |
| RxPDO4 |0x00000501   |105 0000 0001   |Async    |YES    |ControlWord (2), Target Velocity (4)                                          |
|        |             |                |         |       |                                                                              |
| TxPDO1 |0x00000181   |001 1000 0001   | Sync    |YES    |Position Actual Value (4), Current Actual Value (2), StatusWord (2)           |
| TxPDO2 |0x80000281   |010 1000 0001   | Sync    |NO     |Velocity Actual Value (4)                                                     |
| TxPDO3 |0x00000381   |011 1000 0001   |Async    |YES    |StatusWord (2), Position Actual Value (4)                                     |
| TxPDO4 |0x00000481   |100 1000 0001   |Async    |YES    |StatusWord (2), Velocity Actual Value (4)                                     |

Directly representation of each PDO (remember that each property is sent in LESS significatn BYTE order):

|Summary |COB-ID	   | Byte0| Byte1| Byte2| Byte3| Byte4| Byte5| Byte6| Byte7|
|--------|-------------|------|------|------|------|------|------|------|------|
| RxPDO1 |0x201   | PMSV | PMSV | PMSV | PMSV | CMSV | CMSV |  CW  |  CW  |
| RxPDO2 |0x301   | VMSV | VMSV | VMSV | VMSV |      |      |      |      |
| RxPDO3 |0x401   |  CW  |  CW  |  TP  |  TP  |      |      |      |      |
| RxPDO4 |0x501   |  CW  |  CW  |  TV  |  TV  |      |      |      |      |
|        |        |      |      |      |      |      |      |      |      |
| TxPDO1 |0x181   |  PAV |  PAV |  PAV |  PAV |  CAV |  CAV |  SW  |  SW  |
| TxPDO2 |0x281   |  VAV |  VAV |      |      |      |      |      |      |
| TxPDO3 |0x381   |  SW  |  SW  |  PAV |  PAV |  PAV |  PAV |      |      |
| TxPDO4 |0x481   |  SW  |  SW  |  VAV |  VAV |  VAV |  VAV |      |      |

Meaning of each acronym above:

    PositionMode Setting Value (4): PMSV

     CurrentMode Setting Value (2): CMSV

                   ControlWord (2): CW

    VelocityMode Setting Value (4): VMSV

               Target Position (4): TP

               Target Velocity (4): TV

         Position Actual Value (4): PAV

          Current Actual Value (2): CAV

                    StatusWord (2): SW

         Velocity Actual Value (4): VAV

|CAN Bit rate (_index 0x2001 Sub-Index 0x00) |Bit rate   |
|-------------------------------------------|-----------|
|                0                          |  1 Mbit/s |
|                1                          |800 kBit/s |
|                2                          |500 kBit/s |
|                3                          |250 kBit/s |
|                4                          |125 kBit/s |
|                5                          | 50 kBit/s |
|                6                          | 20 kBit/s |

|Operation Mode |Description            |
|---------------|-----------------------|
|	 6		    |Homing Mode            |
|	 3		    |Profile Velocity Mode  |
|	 1		    |Profile Position Mode  |
|	-1		    |Position Mode          |
|	-2		    |Velocity Mode          |
|	-3		    |Current Mode           |
|	-4		    |Diagnostic Mode        |
|	-5		    |MasterEncoder Mode     |
|	-6		    |Step/Direction Mode    |


### SDO COB-ID Server -> Client (tx)	0x0580 + Node-ID
### SDO COB-ID Server -> Client (tx)	0x0600 + Node-ID

## Important command specifier Byte 0 (Bits 7 ... 5)

### Reading
|Length|Sending Data [Byte 0]   |Receiving Data [Byte 0]|
|------|------------------------|-----------------------|
|1 Byte|0x40                    |0x4F                   |
|2 Byte|0x40                    |0x4B                   |
|4 Byte|0x40                    |0x43                   |

### Writing
|Length		|Sending Data [Byte 0]  |Receiving Data [Byte 0]|
|-----------|-----------------------|-----------------------|
|1 Byte		|0x2F (or 0x22)		    |0x60                   |
|2 Byte		|0x2B (or 0x22)			|0x60                   |
|4 Byte		|0x23 (or 0x22)			|0x60                   |
|Not defined|0x22   				|0x60                   |

**PPM**: Profile Position Mode

**PVM**: Profile Velocity Mode

**HMM**: Homing Mode?




### Fault Reset
Object		User Value(??)
0x6040-00	0x0080

### Controlword bits
|Index  |Subindex   |Type       |
|-------|-----------|-----------|
|0x6040 |0x00       | UNSIGNED16|

|Bit	|Description			|PPM					|PVM		|HMM					|
|-------|-----------------------|-----------------------|-----------|-----------------------|
|15-11	|not used				|						|			|						|
|10,9	|reserved				|						|			|						|
|8		|Operation mode specific|Halt 					|Halt		|Halt					|
|7		|Fault reset 			|						|			|						|
|6		|Operation mode specific|Abs / rel				|reserved	|reserved				|
|5		|Operation mode specific|Change set immediately |reserved 	|reserved 				|
|4		|Operation mode specific|New set-point 			|reserved 	|Homing operation start	|
|3		|Enable operation		| 						|   		|   					|
|2		|Quick stop				| 						|   		|   					|
|1		|Enable voltage			| 						|   		|   					|
|0		|Switch on				| 						|   		|   					|


### Statusword bits
|Index  |Subindex   |Type       |
|-------|-----------|-----------|
|0x6040 |0x00       |UNSIGNED16 |

|Bit|Description							|PPM			|PVM			|HMM            |
|---|---------------------------------------|---------------|---------------|---------------|
|15 |Position referenced to home position	|               |               |               |
|14 |Refresh cycle of power stage			|               |               |               |
|13 |Operation mode specific				|Following error|Not used		|Homing error   |
|12 |Operation mode specific				|Set-point ack 	|Speed			|Homing attained|
|11	|Internal limit active			        |               |               |               |
|10	|Operation mode specific				|Target reached	|Target reached |Target reached |
|9	|Remote (NMT Slave State Operational)	|               |               |               |
|8	|Offset current measured			    |               |               |               |
|7	|not used (Warning)			            |               |               |               |
|6	|Switch on disable			            |               |               |               |
|5	|Quick stop			                    |               |               |               |
|4	|Voltage enabled (power stage on)		|               |               |               |
|3	|Fault			                        |               |               |               |
|2	|Operation enable			            |               |               |               |
|1	|Switched on			                |               |               |               |
|0	|Ready to switch on			            |               |               |               |

### Receive and Transmit PDO mapping Objects
Set the value from an object.					
					
|Object Index|Object Sub-Index in bit   |Object Length Value|Object Name                        |Object     |
|------------|--------------------------|-------------------|-----------------------------------|-----------|
|0x6040		 |0x00                      |0x10 (16)          |Controlword						|0x60400010 |
|0x6060		 |0x00                      |0x08 (08)          |Modes of Operation				    |0x60600008 |
|0x6065		 |0x00                      |0x20 (32)          |Max Following Error				|0x60650020 |
|0x607A		 |0x00                      |0x20 (32)          |Target Position					|0x607A0020 |
|0x607C		 |0x00                      |0x20 (32)          |Home Offset						|0x607C0020 |
|0x6081		 |0x00                      |0x20 (32)          |Profile Velocity				    |0x60810020 |
|0x6083		 |0x00                      |0x20 (32)          |Profile Acceleration			    |0x60830020 |
|0x6084		 |0x00                      |0x20 (32)          |Profile Deceleration			    |0x60840020 |
|0x6085		 |0x00                      |0x20 (32)          |Quick Stop Deceleration			|0x60850020 |
|0x6086		 |0x00                      |0x10 (16)          |Motion Profile Type				|0x60860010 |
|0x6098		 |0x00                      |0x10 (16)          |Homing Method					    |0x60980010 |
|0x6099		 |0x01                      |0x20 (32)          |Speed for Switch Search			|0x60990120 |
|0x6099		 |0x02                      |0x20 (32)          |Speed for Zero Search			    |0x60990220 |
|0x609A		 |0x00                      |0x20 (32)          |Homing Acceleration				|0x609A0020 |
|0x60F6		 |0x01                      |0x10 (16)          |Current Regulator P-Gain		    |0x60F60110 |
|0x60F6		 |0x02                      |0x10 (16)          |Current Regulator I-Gain		    |0x60F60210 |
|0x60F9		 |0x01                      |0x10 (16)          |Speed Regulator P-Gain			    |0x60F90110 |
|0x60F9		 |0x02                      |0x10 (16)          |Speed Regulator I-Gain			    |0x60F90210 |
|0x60FB		 |0x01                      |0x10 (16)          |Position Regulator P-Gain		    |0x60FB0110 |
|0x60FB		 |0x02                      |0x10 (16)          |Position Regulator I-Gain		    |0x60FB0210 |
|0x60FB		 |0x03                      |0x10 (16)          |Position Regulator D-Gain		    |0x60FB0310 |
|0x60FB		 |0x04                      |0x10 (16)          |Velocity Feedforward Factor		|0x60FB0410 |
|0x60FB		 |0x05                      |0x10 (16)          |Acceleration Feedforward Factor	|0x60FB0510 |
|0x60FF		 |0x00                      |0x20 (32)          |Target Velocity					|0x60FF0020 |
|0x6410		 |0x01                      |0x10 (16)          |Continuous Current Limit		    |0x64100110 |
|0x6410		 |0x02                      |0x10 (16)          |Output Current Limit			    |0x64100210 |
|0x6410		 |0x04                      |0x10 (16)          |Maximal Speed in Current Mode	    |0x64100410 |
|0x2030		 |0x00                      |0x10 (16)          |CurrentMode Setting Value		    |0x20300010 |
|0x2062		 |0x00                      |0x20 (32)          |PositionMode Setting Value		    |0x20620020 |
|0x206B		 |0x00                      |0x20 (32)          |VelocityMode Setting Value		    |0x206B0020 |
|0x2078		 |0x01                      |0x10 (16)          |Digital Output Functionality State |0x20780110 |
|0x2080		 |0x00                      |0x10 (16)          |Current Threshold for Homing Mode  |0x20800010 |
|0x2081		 |0x00                      |0x20 (32)          |Home Position					    |0x20810020 |
					
### Only Transmit PDO mapping Objects
				
|Object Index   |Object Sub-Index in bit    |Object Length Value|Object Name                        |Object     |
|---------------|---------------------------|-------------------|-----------------------------------|-----------|
|0x6041         |0x00                       |0x10 (16)          |Statusword							|0x60410010 |
|0x6061         |0x00                       |0x08 (08)          |Modes of Operation Display			|0x60610008 |
|0x6062         |0x00                       |0x20 (32)          |Position Demand Value				|0x60620020 |
|0x6064         |0x00                       |0x20 (32)          |Position Actual Value				|0x60640020 |
|0x6069         |0x00                       |0x20 (32)          |Velocity Sensor Actual Value		|0x60690020 |
|0x606B         |0x00                       |0x20 (32)          |Velocity Demand Value				|0x606B0020 |
|0x606C         |0x00                       |0x20 (32)          |Velocity Actual Value				|0x606C0020 |
|0x6078         |0x00                       |0x10 (16)          |Current Actual Value				|0x60780010 |
|0x2020         |0x00                       |0x10 (16)          |Encoder Counter					|0x20200010 |
|0x2021         |0x00                       |0x10 (16)          |Encoder Counter at Index Pulse		|0x20210010 |
|0x2022         |0x00                       |0x10 (16)          |Hall Sensor Pattern				|0x20220010 |
|0x2027         |0x00                       |0x10 (16)          |Current Actual Value Averaged		|0x20270010 |
|0x2028         |0x00                       |0x10 (16)          |Velocity Actual Value Averaged		|0x20280010 |
|0x2071         |0x01                       |0x10 (16)          |Digital Input Functionalities State|0x20710110 |
|0x2074         |0x01                       |0x20 (32)          |Position Marker Captured Position	|0x20740120 |
|0x207C         |0x01                       |0x10 (16)          |Analogue Input 1					|0x207C0110 |
|0x207C         |0x02                       |0x10 (16)          |Analogue Input 2					|0x207C0210 |
|0x20F4         |0x00                       |0x10 (16)          |Following Error Actual Value		|0x20F40010 |

Motor: [https://www.maxongroup.com/maxon/view/product/motor/ecmotor/ecmax/ecmax30/272763](https://www.maxongroup.com/maxon/view/product/motor/ecmotor/ecmax/ecmax30/272763)

Encoder: [https://www.maxongroup.com/maxon/view/product/sensor/encoder/Magnetische-Encoder/ENCODERMR/ENCODER-MR-TYPML-128-1000IMP-3KANAL/225778](https://www.maxongroup.com/maxon/view/product/sensor/encoder/Magnetische-Encoder/ENCODERMR/ENCODER-MR-TYPML-128-1000IMP-3KANAL/225778)
Assembly number: 301782
