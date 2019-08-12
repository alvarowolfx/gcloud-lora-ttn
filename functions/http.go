package ttn

import (
	"encoding/json"
	"net/http"
)

func sendSuccessResponse(w http.ResponseWriter) {
	w.Header().Set("Content-Type", "application/json; charset=UTF-8")
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(struct {
		Status  int    `json:"status"`
		Message string `json:"message"`
	}{
		Status:  http.StatusOK,
		Message: "OK",
	})
}

func sendSuccessResponseWithData(w http.ResponseWriter, data []interface{}) {
	w.Header().Set("Content-Type", "application/json; charset=UTF-8")
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(struct {
		Status  int           `json:"status"`
		Message string        `json:"message"`
		Results []interface{} `json:"results"`
	}{
		Status:  http.StatusOK,
		Message: "OK",
		Results: data,
	})
}

func handleCORSRequest(w http.ResponseWriter, r *http.Request) bool {
	// Set CORS headers for the preflight request
	if r.Method == http.MethodOptions {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "POST")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
		w.Header().Set("Access-Control-Max-Age", "3600")
		w.WriteHeader(http.StatusNoContent)
		return true
	}
	// Set CORS headers for the main request.
	w.Header().Set("Access-Control-Allow-Origin", "*")
	return false
}

func sendErrorResponse(w http.ResponseWriter, cause string) {
	w.Header().Set("Content-Type", "application/json; charset=UTF-8")
	w.WriteHeader(http.StatusInternalServerError)
	json.NewEncoder(w).Encode(struct {
		Status  int    `json:"status"`
		Message string `json:"message"`
	}{
		Status:  http.StatusInternalServerError,
		Message: cause,
	})
}
