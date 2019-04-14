# The Things Network Integration with Google Cloud

Process uplink messages from TTN on Google Cloud, storing Realtime data on Firebase, historical data on BigQuery and visualize the data though a web app.

**THIS IS A WORK IN PROGRESS**

![Lora Node](./images/lora-node.jpg)

![The Things Network Dash](./images/ttn.png)

## BOM - Bill of Materials

### Gateway 
* Dragino LP01-S - Single Channel LoRa Gateway
  * Connected to The Things Network - [Connect to TTN - Wiki for Dragino Project](https://wiki.dragino.com/index.php?title=Connect_to_TTN)

### LoRa End Node

* Feather M0 LoRa 915mhz - [Link](https://www.adafruit.com/product/3178) 
* CCS811 Air Quality Sensor - [Link](https://learn.sparkfun.com/tutorials/ccs811-air-quality-breakout-hookup-guide?_ga=2.196632293.895409039.1555180411-1431402083.1549724810)
* 240mah battery

#### Upload firmware with PlatfomIO

I recommend installing the Visual Studio Code (VSCode) IDE and the PlatformIO plugin to get started using it. Just follow the step on the link below: 

https://platformio.org/platformio-ide

To deploy to the board, just open the `firmware` folder and you can use the “Build” and “Upload” buttons on PlatformIO Toolbar. All libraries and dependencies will be downloaded.

*TODO: Add wiring*

## The Things Network Setup

**Work in Progress**

## Google Cloud Setup

**Work in Progress**

## References
* https://learn.adafruit.com/using-lorawan-and-the-things-network-with-circuitpython?view=all
