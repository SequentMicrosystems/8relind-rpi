# Sprinklers 

This is an 64 stations irrigation system created with Sequent Microsystems [Eight RELAYS 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/collections/industrial-automation/products/8-relays-stackable-card-for-raspberry-pi)

## Installing the software

* Install NodeRed following the instructions from [HERE](https://help.ubidots.com/en/articles/1958375-how-to-install-node-red-in-raspberry-pi).

* Run ```sudo raspi-config``` and enable I2C communication.

* Connect up to 8 [Eight RELAYS Cards](https://sequentmicrosystems.com/collections/industrial-automation/products/8-relays-stackable-card-for-raspberry-pi) to your raspberry, each with different stack level jumper settings.  

* Install the node for [Eight RELAYS Card](https://sequentmicrosystems.com/collections/industrial-automation/products/8-relays-stackable-card-for-raspberry-pi) by opening the Node-RED interface in a browser: Go to Menu > Manage pallete > Install and search for node-red-contrib-sm-8relind, install the node and restart the Node-RED

* Import the Sprinkler controller flow: Go to Menu > Import > select a file to import. Load the provided [flows.json](https://github.com/SequentMicrosystems/8relind-rpi/blob/main/node-red/Sprinklers/flows.json) file and click the Import button.
![import](Pic/Import1.jpg)

* Click on the top rigt ```Deploy``` button.

* Go to the dashboard and select  the Settings menu
![Settings](Pic/Settings2.jpg)

* Setup the weather and operating mode:
![Settings1](Pic/Settings1_1.jpg)

* Enable each channel per day of the week from the Stations tab. The software detects how many cards are installed and creates a page for each card.
![Stations1-8](Pic/Stations1-8.jpg)


