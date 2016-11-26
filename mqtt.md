### Настройка MQTT

Пержде всего необходимо выполнить команду sudo pip install paho-mqtt

Исходный код находится в папке `2016/src/Raspberry/LeoNickTEAM/То что надо/`

База данных находится в places.db
Открыть можно при помощи sqlite3 коммандой:
`sqlite3 places.db`
Выйти из редактора можно с помощью команды `.exit`
в этой базе данных есть таблица place, в которой хранятся данные о парковке:
* id - номер парковочного места
* status - статус бронирования (например 0 - свободно, 1 - зарезервировано, 2 - занято)
Содержимое папки нужно скопировать в рабочую директорию

Код для редактирования находится в файле mqtt.py.
В нем нужно поменять следующие константы на свои:

* publish_topic = "iot-2/evt/mySensor/fmt/json" -топик по которому будут отправлятся сообщения в брокер
* subscribe_topic = "iot-2/cmd/getSmth/fmt/json" -топик по которому будут приниматься сообщения от брокера
* password = "D5i2&9LAtE2OGc_9(a"  # auth-token  -токен для подключения
* organization = "vln6hd"  - ID организации
* deviceType = "MQTTDevice" - тип девайса
* macAddress = 'aabbccddee12' - ID девайса

* отправка сообщения:
python
```
        data = '{"lotID": ' + str(i) + ',"status": "' + str(st) + '"}'
        
        mqttc.publish(publish_topic, payload=data, qos=0, retain=False)
```

* прием сообщения
python 
```
    inpstr = str(message.payload)[2:-1]
    
    parsed_string = json.loads(inpstr)
    
    fullID = parsed_string["lotID"]
    
    a = fullID[0] #строка
    
    b = fullID[2] #столбец
    
    ind = (int(a)-1) * 4 + int(b) - 
    
    status = parsed_string["commandName"]` # парсинг команды
```
