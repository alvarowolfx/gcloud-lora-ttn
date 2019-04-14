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
