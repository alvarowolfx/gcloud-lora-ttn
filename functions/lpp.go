package ttn

import (
	"strconv"
)

type UplinkTarget struct {
	values map[string]interface{}
}

func NewUplinkTarget() *UplinkTarget {
	return &UplinkTarget{
		values: map[string]interface{}{},
	}
}

func (t *UplinkTarget) GetValues() map[string]interface{} {
	return t.values
}

func (t *UplinkTarget) putValue(channel uint8, key string, value interface{}) {
	channelKey := strconv.Itoa(int(channel))
	val, ok := t.values[channelKey]
	if !ok {
		val = map[string]interface{}{}
		t.values[channelKey] = val
	}
	obj := val.(map[string]interface{})
	obj[key] = value
}

func (t *UplinkTarget) Port(channel uint8, value float32) {
	t.putValue(channel, "port", value)
}

func (t *UplinkTarget) DigitalInput(channel, value uint8) {
	t.putValue(channel, "digitalInput", value)
}

func (t *UplinkTarget) DigitalOutput(channel, value uint8) {
	t.putValue(channel, "digitalOutput", value)
}

func (t *UplinkTarget) AnalogInput(channel uint8, value float32) {
	t.putValue(channel, "analogInput", value)
}

func (t *UplinkTarget) AnalogOutput(channel uint8, value float32) {
	t.putValue(channel, "analogOutput", value)
}

func (t *UplinkTarget) Luminosity(channel uint8, value uint16) {
	t.putValue(channel, "luminosity", value)
}

func (t *UplinkTarget) Presence(channel, value uint8) {
	t.putValue(channel, "presence", value)
}

func (t *UplinkTarget) Temperature(channel uint8, celcius float32) {
	t.putValue(channel, "temperature", celcius)
}

func (t *UplinkTarget) RelativeHumidity(channel uint8, rh float32) {
	t.putValue(channel, "relativeHumidity", rh)
}

func (t *UplinkTarget) Accelerometer(channel uint8, x, y, z float32) {
	value := map[string]float32{
		"x": x,
		"y": y,
		"z": z,
	}
	t.putValue(channel, "accelerometer", value)
}

func (t *UplinkTarget) BarometricPressure(channel uint8, hpa float32) {
	t.putValue(channel, "barometricPressure", hpa)
}

func (t *UplinkTarget) Gyrometer(channel uint8, x, y, z float32) {
	value := map[string]float32{
		"x": x,
		"y": y,
		"z": z,
	}
	t.putValue(channel, "gyrometer", value)
}

func (t *UplinkTarget) GPS(channel uint8, latitude, longitude, altitude float32) {
	value := map[string]float32{
		"latitude":  latitude,
		"longitude": longitude,
		"altitude":  altitude,
	}
	t.putValue(channel, "gps", value)
}
