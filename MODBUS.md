[![8relind-rpi](readmeres/sequent.jpg)](https://sequentmicrosystems.com)

# Modbus

The Sequent Microsystems [Eight Relays 4A/120V 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/products/eight-relays-stackable-card-for-raspberry-pi) V 7.0 or higher can be accessed through the Modbus RTU protocol over the RS-485 port.
You can set up the RS-485 port with the **8relind** command.

Example:
```bash
~$ 8relind 0 cfg485wr 1 9600 1 0 1
```
Set Modbus RTU, Baudrate: 9600bps, 1 Stop Bit,  parity: None, slave address base: 1
```bash
~$ 8relind -h cfg485wr
```
Display the full set of options

## Slave Address
The three address jumpers have a dual purpose. 
When the card is accessed from Raspberry Pi, the jumpers define the offset of the I2C address, permitting stacking of up to 8 cards.
When the card is accessed through MODBUS protocol over the RS485 port, the jumpers define the offset of the RS485 base address set by the software.

## Modbus object types
All Modbus RTU object types with standard addresses are implemented: Coils, Discrete Inputs, Input Registers, Holding Registers.

### Coils

Access level: Read/Write, Size: 1 bit

| Device function | Register Address | Modbus Address | Description |
| --- | --- | --- | --- |
|   RELAY1 | 0001 | 0x00 | Relay #1 control |
|   RELAY2 | 0002 | 0x00 | Relay #1 control |
|   RELAY3 | 0003 | 0x00 | Relay #1 control |
|   RELAY4 | 0004 | 0x00 | Relay #1 control |
|   RELAY5 | 0005 | 0x00 | Relay #1 control |
|   RELAY6 | 0006 | 0x00 | Relay #1 control |
|   RELAY7 | 0007 | 0x00 | Relay #1 control |
|   RELAY8 | 0008 | 0x00 | Relay #1 control |


## Function codes implemented

* Read Coils (0x01)
* Read Discrete Inputs (0x02)
* Read Holding Registers (0x03)
* Read Input Registers (0x04)
* Write Single Coil (0x05)
* Write Single Register (0x06)
* Write Multiple Coils (0x0f)
* Write Multiple Registers (0x10)
