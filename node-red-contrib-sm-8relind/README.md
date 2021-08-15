# node-red-contrib-sm-8relind

This is the node-red node to control Sequent Microsystems [8-RELAYS Stackable Card for Raspberry Pi](https://sequentmicrosystems.com/collections/all-io-cards/products/raspberry-pi-relays-stackable-card/).

## Manual Install

Clone or update the repository, follow the instrutions fron the [first page.](https://github.com/SequentMicrosystems/8relind-rpi)

In your node-red user directory, tipicaly ~/.node-red,

```bash
~$ cd ~/.node-red
```

Run the following command:

```bash
~/.node-red$ npm install ~/8relind-rpi/node-red-contrib-sm-8relind
```

In order to see the node in the palette and use-it you need to restart node-red. If you run node-red as a service:
 ```bash
 ~$ node-red-stop
 ~& node-red-start
 ```

## Usage

After install and restart the node-red you will see on the node palete, under Sequent Microsystems category the "8relind" and "8relindrd" nodes.

### 8relind node
This node will turn on or off a relay or all relays as a 8 bits bitmap. The card stack level and relay number can be set in the dialog screen or dinamicaly thru ``` msg.stack``` and ``` msg.relay ```. The output of one relay or all 8 relays if you set the the relay number to 0, can be set dynamically using  ``` msg.payload ```.

### 8relindrd node
Thi node will read one relay state or all relays states as a 8 bits bitmap. The card stack level and relay number can be set in the dialog screen or dinamicaly thru ``` msg.stack``` and ``` msg.relay ``` and the state is output as  ``` msg.payload ``` .If you set the relay number to 0 the node will output the state of all relays.

## Important note

This node is using the I2C-bus package from @fivdi, you can visit his work on github [here](https://github.com/fivdi/i2c-bus). 
The inspiration for this node came from @nielsnl68 work with [node-red-contrib-i2c](https://github.com/nielsnl68/node-red-contrib-i2c).Thank both for the great job.
