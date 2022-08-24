# Sprinklers 

This is an irrigation system created with Sequent Microsystems [Eight RELAYS 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/collections/industrial-automation/products/8-relays-stackable-card-for-raspberry-pi)

## Installing the software

* Install NodeRed folloing the instructions from [here](https://help.ubidots.com/en/articles/1958375-how-to-install-node-red-in-raspberry-pi).
* Enable I2C communication running ```sudo raspi-config```
* Install the node for Eight RELAYS hat by open NodeRED interface in a browser: Go to Menu > Manage pallete > Install and search for node-red-contrib-sm-8relind, install the node and restart the NodeRED
* Import the Sprinkler controller flow: Go to Menu > Import > select a file to import. Load the provided ```flows.json``` file and click Import button.
