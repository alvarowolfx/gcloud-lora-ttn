package ttn

import (
	"context"
	"encoding/json"
	"log"
	"net/http"
	"os"

	"cloud.google.com/go/bigquery"
	"firebase.google.com/go"
	"firebase.google.com/go/db"
)

var app *firebase.App
var database *db.Client
var bqClient *bigquery.Client
var projectID = os.Getenv("GCLOUD_PROJECT")
var datasetID = "ttn_dataset"
var tableID = "raw_data"

func init() {
	ctx := context.Background()

	conf := &firebase.Config{
		DatabaseURL: "https://" + projectID + ".firebaseio.com",
	}

	var err error
	app, err = firebase.NewApp(ctx, conf)
	if err != nil {
		log.Fatalf("error initializing app: %v\n", err)
	}

	database, err = app.Database(ctx)
	if err != nil {
		log.Fatalf("error initializing db: %v\n", err)
	}

	bqClient, err = bigquery.NewClient(ctx, projectID)
	if err != nil {
		log.Fatalf("error initializing bigquery client: %v\n", err)
	}
}

// HandleTTNUplink process uplink msg sent by TTN
func HandleTTNUplink(w http.ResponseWriter, r *http.Request) {
	var msg UplinkMessage
	err := json.NewDecoder(r.Body).Decode(&msg)

	if err != nil {
		log.Printf("error decoding body: %v\n", err.Error())
		sendErrorResponse(w, err.Error())
		return
	}

	log.Printf("Function received valid json %v\n", msg)

	deviceData := GetDeviceUpdate(msg)

	log.Printf("Sending update to firebase  %v\n", deviceData)

	ctx := context.Background()

	devicesRef := database.NewRef("devices")
	err = devicesRef.Child(msg.DevID).Update(ctx, deviceData)
	if err != nil {
		log.Printf("error updating firebase: %v\n", err.Error())
		sendErrorResponse(w, err.Error())
		return
	}

	log.Printf("Data updated on firebase\n")

	u := bqClient.Dataset(datasetID).Table(tableID).Uploader()
	rows := []*DeviceData{
		{DeviceID: msg.DevID, Data: deviceData, Timestamp: msg.Metadata.Time},
	}

	err = u.Put(ctx, rows)
	if err != nil {
		log.Printf("error inserting on bigquery: %v", err.Error())
		sendErrorResponse(w, err.Error())
		return
	}

	log.Printf("Data Inserted on BigQuery\n")

	sendSuccessResponse(w)
	return
}
