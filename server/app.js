
const BEDROOM_SHELF_TEMPERATURE_TOPIC = "bedroom/shelf/temperature"
const BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC = "bedroom/shelf/relative_humidity"

// setup mqtt
var mqtt = require('mqtt')
var client = mqtt.connect('mqtt://mosquitto') // should connect to the mosquitto docker container

// setup influx db
const {InfluxDB} = require('@influxdata/influxdb-client')

// You can generate a Token from the "Tokens Tab" in the UI
const token = 'octopusoctopus'
const org = 'seabed'
const bucket = 'ocean'

const influxClient = new InfluxDB({url: 'http://influxdb:8086', token: token})

const {Point} = require('@influxdata/influxdb-client')
const writeApi = influxClient.getWriteApi(org, bucket)
writeApi.useDefaultTags({host: 'host1', location:'bedroom_shelf'})




client.on('connect', () => {
    console.log("connected to mqtt bridge")
    
    client.subscribe(BEDROOM_SHELF_TEMPERATURE_TOPIC)
    client.subscribe(BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC)
})

client.on('message', (topic, message) => {

    let point = new Point('indoor-weather')
    
    if (topic == BEDROOM_SHELF_TEMPERATURE_TOPIC) {
        console.log("temperature: " + message)

        point = point.floatField("temperature", parseFloat(message))
        
    }
    else if (topic == BEDROOM_SHELF_RELATIVE_HUMIDITY_TOPIC) {
        console.log("humidity: " + message)
        point = point.floatField("relative_humidity", parseFloat(message))
    }

    writeApi.writePoint(point)
    

})