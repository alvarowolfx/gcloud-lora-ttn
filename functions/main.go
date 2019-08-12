package ttn

import (
	"bytes"
	"context"
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"time"

	"cloud.google.com/go/bigquery"
	"firebase.google.com/go"
	"firebase.google.com/go/db"
	"google.golang.org/api/iterator"
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

// HistoryResultItem history query result
type HistoryResultItem struct {
	DeviceID string    `json:"deviceId"`
	Time     time.Time `json:"time"`
	Data     string    `json:"data"`
}

// HandleDeviceHistoryQuery returns last 7 days of data for a device
func HandleDeviceHistoryQuery(w http.ResponseWriter, r *http.Request) {
	finished := handleCORSRequest(w, r)
	if finished {
		return
	}

	p := r.URL.Query()
	deviceID := p.Get("deviceId")
	if deviceID == "" {
		sendErrorResponse(w, "Missing deviceId query param.")
		return
	}

	table := "`" + projectID + "." + datasetID + "." + tableID + "`"
	q := bqClient.Query(`
		SELECT d.deviceId,
			d.time,
			JSON_EXTRACT(d.data, '$.data') as data
		FROM ` + table + ` d
		WHERE d.time between timestamp_sub(current_timestamp, INTERVAL 7 DAY) and current_timestamp()    
		and d.deviceId = "` + deviceID + `"
    order by d.time
	`)
	ctx := context.Background()
	it, err := q.Read(ctx)

	if err != nil {
		sendErrorResponse(w, err.Error())
		return
	}

	var results []interface{}

	for {
		var i HistoryResultItem
		err := it.Next(&i)
		if err == iterator.Done {
			break
		}

		if err != nil {
			sendErrorResponse(w, err.Error())
			return
		}

		results = append(results, i)
	}

	sendSuccessResponseWithData(w, results)
	return
}

// HandleTTNUplink process uplink msg sent by TTN
func HandleTTNUplink(w http.ResponseWriter, r *http.Request) {
	bodyBytes, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Printf("error reading body: %v\n", err.Error())
		sendErrorResponse(w, err.Error())
		return
	}
	log.Printf("received body %v\n", string(bodyBytes))

	// Restore the io.ReadCloser to its original state
	r.Body = ioutil.NopCloser(bytes.NewBuffer(bodyBytes))

	var msg UplinkMessage
	err = json.NewDecoder(r.Body).Decode(&msg)

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
