import time
import paho.mqtt.client as mqtt
import json
from uti import *

publish_topic = "iot-2/evt/mySensor/fmt/json"
subscribe_topic = "iot-2/cmd/getSmth/fmt/json"
username = "use-token-auth"
password = "D5i2&9LAtE2OGc_9(a"  # auth-token
organization = "vln6hd"  # org_id
deviceType = "MQTTDevice"
macAddress = 'aabbccddee12'
# Creating the client connection
# Set clientID and broker
clientID = "d:" + organization + ":" + deviceType + ":" + macAddress
broker = organization + ".messaging.internetofthings.ibmcloud.com"
mqttc = mqtt.Client(clientID)


def on_connect(client, userdata, flags, rc):
    mqttc.subscribe(subscribe_topic)


def on_message(client, userdata, message):
    print("Received message '" + str(message.payload) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))
    inpstr = str(message.payload)[2:-1]
    parsed_string = json.loads(inpstr)
    fullID = parsed_string["lotID"]
    a = fullID[0]
    b = fullID[2]
    ind = (int(a)-1) * 4 + int(b) - 1
    status = parsed_string["commandName"]
    if status == "blockPlace":
        update_status(ind, 1)
        data = '{"lotID": ' + str(ind+1) + ',"status": "' + "reserved" + '"}'
        mqttc.publish(publish_topic, payload=data, qos=0, retain=False)

mqttc.on_message = on_message
mqttc.on_connect = on_connect
# Set authentication values, if connecting to registered service
if username is not "":
    mqttc.username_pw_set(username, password=password)

mqttc.connect(host=broker, port=1883, keepalive=60)

# Publishing to IBM Internet of Things Foundation
mqttc.loop_start()

while mqttc.loop() == 0:
    st = None
    i = 1
    status = get_status(i)
    if status == 0:
        st = 'free'
    elif status == 1:
        st = 'reserved'
    elif status == 2:
        st = 'blocked'
    if st is not None:
        data = '{"lotID": ' + str(i) + ',"status": "' + str(st) + '"}'
        mqttc.publish(publish_topic, payload=data, qos=0, retain=False)
        print("message published")
    time.sleep(5)
pass
