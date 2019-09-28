# The Things Network Integration with Google Cloud

Process uplink messages from TTN on Google Cloud, storing Realtime data on Firebase, historical data on BigQuery and visualize the data through a web app.

Web App URL : [Access Here](https://ttn-gcp-lora.firebaseapp.com/)

[![Run on Google Cloud](https://storage.googleapis.com/cloudrun/button.svg)](https://console.cloud.google.com/cloudshell/editor?shellonly=true&cloudshell_image=gcr.io/cloudrun/button&cloudshell_git_repo=https://github.com/alvarowolfx/gcloud-lora-ttn&cloudshell_working_dir=functions)

️ ⚠️️️️**THIS IS A WORK IN PROGRESS** ⚠️

![Architecture](./images/flow.png)

LoRa End Node:
![Lora Node](./images/lora-node.jpg)

Data arriving on The Things Network:
![The Things Network Dash](./images/ttn.png)

Realtime Data on Firebase:
![Data Firebase](./images/fb-data.png)

Query Data on Bigquery:
![Data on Bigquery](./images/query-data.png)

Monitor data on a WebApp :
![Web App](./images/ttn-web-app.png)

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

## Google Cloud Setup

* Install gcloud CLI - [Link](https://cloud.google.com/sdk/install)
* Authenticate with Google Cloud:
    * `gcloud auth login`
* Create cloud project — choose your unique project name:
    * `gcloud projects create YOUR_PROJECT_NAME`
* Set current project
    * `gcloud config set project YOUR_PROJECT_NAME`
* Associate with a Firebase Project - [Link](https://console.firebase.google.com/)
* Create a Table on BigQuery - [Link](https://console.cloud.google.com/bigquery)
  * Create a Dataset with name `ttn_dataset` 
  * Create a table with name `raw_data` and fields `deviceId (string)`, `data(string)` and `time (Timestamp)`

![Bigquery Table](./images/bq-table.png)

## Deploy Backend

We have two options here, using Cloud Run and Google Cloud Functions.

#### Deploy Using Cloud Run

Just follow the steps and fill the `GCP_PROJECT` variable with your Google Cloud Platform project ID.

[![Run on Google Cloud](https://storage.googleapis.com/cloudrun/button.svg)](https://console.cloud.google.com/cloudshell/editor?shellonly=true&cloudshell_image=gcr.io/cloudrun/button&cloudshell_git_repo=https://github.com/alvarowolfx/gcloud-lora-ttn&cloudshell_working_dir=functions)

After deploying the server on Cloud Run, you can get it's url on GCP Console ([link](https://console.cloud.google.com/run)) and select the service `ttn-gcp-lora` that we just deployed. Them copy the URL will look like `https://{project-id}-{some-random-hash-string}.a.run.app`. The endpoint that handles TTN requests it `/uplink`, we are going to use this to setup TTN integration.

#### Deploy Cloud Function

This step will deploy a HTTP Cloud Function made with Golang. I made a script `deploy.sh` that is on the `functions` folder, that you can use to do that.

* Run on the command line:
```
cd functions
./deploy.sh
```

After deploying the cloud function, you can get it's url that will look like `https://{project-region}-{project-id}.cloudfunctions.net/HandleTTNUplink` that handles TTN data and another one that will look like `https://{project-region}-{project-id}.cloudfunctions.net/HandleDeviceHistoryQuery` that returns device history to be used by the UI.

## The Things Network Setup

* Create a new application
* Create a new integration of type HTTP
  * Use the url of the server on Cloud Run or from Cloud Function created on the previous step.

![TTN Integration 1](./images/ttn-integration1.png)
![TTN Integration 2](./images/ttn-integration2.png)
![TTN Integration 3](./images/ttn-integration3.png)

## Deploy Web UI on Firebase

* Enable Firebase Hosting on the web project
  * `cd web && firebase init` and choose your Firebase project
* Change file `src/config.js` with you Firebase Web SDK Config.
* Deploy UI
  * `npm run deploy`

## References
* https://learn.adafruit.com/using-lorawan-and-the-things-network-with-circuitpython?view=all
* https://www.thethingsnetwork.org/docs/applications/http/#uplink
* https://cloud.google.com/bigquery/streaming-data-into-bigquery#bigquery_table_insert_rows-go
