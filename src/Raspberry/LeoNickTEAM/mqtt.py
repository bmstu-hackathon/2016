import json

import ibmiotf.device
from uti import *

PUBLISH_TOPIC = 'iot-2/evt/<event-id>/fmt/<format>'


def connect(config):
    options = ibmiotf.device.ParseConfigFile(config)
    client = ibmiotf.device.Client(options)
    client.connect()
    client.commandCallback = on_message
    return client


def on_message(cmd):

    inpstr = cmd.command;
    parsed_string = json.loads(inpstr)
    fullID = parsed_string["lotID"]
    a = fullID[0];
    b = fullID[2];
    ind = int(a) * 4 + int(b)
    update_status(ind, 1)


def send_data(ident, status):

    if (status == 1):
        st = 'blocked'
    else:
        st = 'free'
    data = '{"lotID": ' + ident + ',"status": "' + st + '"}'
    client.publishEvent(PUBLISH_TOPIC, 'json', data)


client = connect('device.cfg')


def main():
    while True:
        for i in range(1, 2):
            send_data(i, get_status(i))


if __name__ == '__main__':
    main()
