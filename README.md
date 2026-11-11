# 8relind-rpi

[![relay8-rpi](readmeres/sequent.jpg)](https://www.sequentmicrosystems.com)

[![relay8-rpi](readmeres/8-RELIND.jpg)](https://sequentmicrosystems.com/products/eight-relays-stackable-card-for-raspberry-pi)

<h2><a href="https://sequentmicrosystems.com/products/eight-relays-stackable-card-for-raspberry-pi">8-RELAYS Stackable Card for Raspberry Pi</a> by <a href="https://www.sequentmicrosystems.com">Sequent Microsystems</a></h2>

The command line interface (CLI) included in this repository gives you full access to every feature of the card. It is the quickest way to get started and is all you need to operate the card.

If you would rather use a different platform, you can access the same functionality through dedicated, ready-to-use integrations:
- [Python Library](https://github.com/SequentMicrosystems/8relind-rpi/tree/master/python)
- [Node-RED example based on "exec" node](https://github.com/SequentMicrosystems/8relind-rpi/tree/master/node-red)
- [Node-RED "8relind" node](https://github.com/SequentMicrosystems/8relind-rpi/tree/master/node-red-contrib-sm-8relind)
- [Modbus RTU slave](https://github.com/SequentMicrosystems/8relind-rpi/blob/master/MODBUS.md)

For a firmware update, please follow the instructions in the link below:

- [Firmware update instructions](https://github.com/SequentMicrosystems/8relind-rpi/blob/main/update/README.md)
  
*Note: This repository contains the command line and Python functions to control Ver. 4 of the card. If you use Ver. 3.x, download from [here](https://github.com/SequentMicrosystems/8relay-rpi). If you have Ver. 1-2 with screw-type connectors, download from [here](https://github.com/SequentMicrosystems/relay8-rpi).*

---

## Prerequisites

<details>
<summary>1. Enable I2C communication</summary>

This only needs to be done once on each Raspberry Pi.

On Raspberry Pi OS, install `raspi-config` first:

```bash
sudo apt update
sudo apt install raspi-config
```

Then run:

```bash
sudo raspi-config
```

Follow the steps in the 3 screenshots below to enable I2C communication. After that, reboot the Raspberry Pi for the changes to take effect.

![Enable I2C](https://github.com/SequentMicrosystems/megaind-rpi/blob/master/res/i2c/i2c_step1.png)
![Enable I2C](https://github.com/SequentMicrosystems/megaind-rpi/blob/master/res/i2c/i2c_step2.png)
![Enable I2C](https://github.com/SequentMicrosystems/megaind-rpi/blob/master/res/i2c/i2c_step3.png)

Reboot the Raspberry Pi:

```bash
sudo reboot
```

</details>

<details>
<summary>2. Install git and build-essential packages</summary>

```bash
sudo apt update
sudo apt install git build-essential
```

</details>

## Installation

Install the CLI with:

```bash
cd
git clone https://github.com/SequentMicrosystems/8relind-rpi.git
cd 8relind-rpi/
sudo make install
```

To update to the latest version:

```bash
cd ~/8relind-rpi/  
git pull
sudo make install
```  

## Usage

You can now access all the functions of the [Eight Relays 4A/120V 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/products/eight-relays-stackable-card-for-raspberry-pi) through the "8relind" command. Use the -h option for help:

```bash
8relind -h
```

For more detailed help on a specific function, use the -h option after the function name. For example:

```bash
8relind -h read
```

Example output:

```txt
8relind -h read
```
Outputs:

```txt
read:        Read relays status
Usage:       8relind <id> read <channel>
Usage:       8relind <id> read
Example:     8relind 0 read 2; Read Status of Relay #2 on Board #0
```
