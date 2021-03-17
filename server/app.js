
const BEDROOM_SHELF_TEMPERATURE_TOPIC = "bedroom/shelf/temperature"
const BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC = "bedroom/shelf/relative_humidity"

var mqtt = require('mqtt')

var client = mqtt.connect('mqtt://mosquitto') // should connect to the mosquitto docker container

client.on('connect', () => {
    console.log("connected to mqtt bridge")
    
    client.subscribe(BEDROOM_SHELF_TEMPERATURE_TOPIC)
    client.subscribe(BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC)
})

client.on('message', (topic, message) => {
    if (topic == BEDROOM_SHELF_TEMPERATURE_TOPIC) {
        console.log("temperature: " + message)
    }
    else if (topic == BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC) {
        console.log("humidity: " + message)
    }

})