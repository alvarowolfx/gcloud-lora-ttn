package ttn

import (
	"bytes"
	"encoding/json"
	"testing"
)

const validPacket = "{\"app_id\":\"teste_gcloud\",\"dev_id\":\"teste_gcloud\",\"hardware_serial\":\"0012243648607290\",\"port\":1,\"counter\":47,\"payload_raw\":\"AWcA5gJoegNzIyoEcwjKBWj+BmcAzwdzAAAIZwAACWcAlg==\",\"payload_fields\":{\"barometric_pressure_3\":900.2,\"barometric_pressure_4\":225,\"barometric_pressure_7\":0,\"relative_humidity_2\":61,\"relative_humidity_5\":127,\"temperature_1\":23,\"temperature_6\":20.7,\"temperature_8\":0,\"temperature_9\":15},\"metadata\":{\"time\":\"2019-04-19T21:29:46.1327739Z\",\"frequency\":913.3,\"modulation\":\"LORA\",\"data_rate\":\"SF10BW125\",\"coding_rate\":\"4/5\",\"gateways\":[{\"gtw_id\":\"eui-18fe34fffffa9c25\",\"timestamp\":825494655,\"time\":\"\",\"channel\":0,\"rssi\":-64,\"snr\":11,\"rf_chain\":0,\"latitude\":52,\"longitude\":5,\"altitude\":1}]},\"downlink_url\":\"https://integrations.thethingsnetwork.org/ttn-eu/api/v2/down/teste_gcloud/teste_gcloud?key=ttn-account-v2.o4z0c180zdesnHmGx8WqnE7wFS5cBXkOkaP-qrR_TbM\"}"

func TestDecodeTTNJson(t *testing.T) {
	buf := bytes.NewBufferString(validPacket)

	var msg UplinkMessage
	err := json.NewDecoder(buf).Decode(&msg)

	if err != nil {
		t.Fatalf("error: decoding uplink %v", err)
	}
}
