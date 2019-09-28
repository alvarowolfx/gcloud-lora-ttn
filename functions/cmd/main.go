package main

import (
	"fmt"
	"github.com/alvarowolfx/ttn-lora-gcp"
	"log"
	"net/http"
	"os"
)

func main() {
	log.Print("GCP TTN Integration started.")

	http.HandleFunc("/uplink", ttn.HandleTTNUplink)
	http.HandleFunc("/history", ttn.HandleDeviceHistoryQuery)

	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}

	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%s", port), nil))
}
