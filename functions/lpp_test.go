package ttn

import (
	"bytes"
	"encoding/json"
	"github.com/TheThingsNetwork/go-cayenne-lib/cayennelpp"
	"testing"
)

func TestDecode(t *testing.T) {

	buf := []byte{
		1, cayennelpp.DigitalInput, 255,
		2, cayennelpp.DigitalOutput, 100,
		3, cayennelpp.AnalogInput, 21, 74,
		4, cayennelpp.AnalogOutput, 234, 182,
		5, cayennelpp.Luminosity, 1, 244,
		6, cayennelpp.Presence, 50,
		7, cayennelpp.Temperature, 255, 100,
		8, cayennelpp.RelativeHumidity, 160,
		9, cayennelpp.Accelerometer, 254, 88, 0, 15, 6, 130,
		10, cayennelpp.BarometricPressure, 41, 239,
		11, cayennelpp.Gyrometer, 1, 99, 2, 49, 254, 102,
		12, cayennelpp.GPS, 7, 253, 135, 0, 190, 245, 0, 8, 106,
	}
	decoder := cayennelpp.NewDecoder(bytes.NewBuffer(buf))
	target := NewUplinkTarget()

	err := decoder.DecodeUplink(target)

	if err != nil {
		t.Fatalf("error: decoding uplink %v", err)
	}

	data, err := json.Marshal(target.GetValues())
	if err != nil {
		t.Fatalf("error: encoding to json %v", err)
	}

	t.Log(string(data))
}
