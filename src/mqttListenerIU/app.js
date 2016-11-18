//подключение библиотек
var mqtt = require('mqtt'), 
	express = require('express'),
	conf = require('./config.json'),
	fs = require('fs'),
	SensorData = require('./libs/mongoose').SensorData,
	app = express();

app.use (express.static(__dirname + '/public'));
// опции для подключения к MQTT Broker Bluemix
var options = {
		clientId:'a:' + conf.BLUEMIXAPPID + ':' + conf.appID,
		username:conf.username,
		password:conf.password
	}, 
	MQTTmessage="no message", // буфер для хранения сообщения
	timerID;	

var brokerPath = 'tcp://' + conf.BLUEMIXAPPID + '.messaging.internetofthings.ibmcloud.com', // путь к брокеру
	subscribePath = 'iot-2/type/' + conf.deviceType + '/id/' + conf.deviceID + '/evt/' + conf.eventID + '/fmt/json', // путь канала для подписки
	publishPath = 'iot-2/type/MQTTDevice/id/aabbccddee12/cmd/getSmth/fmt/json',
	client = mqtt.connect(brokerPath,options); // создание клиента MQTT


client.on('connect', function () {
 
  client.subscribe(subscribePath); // подписка на канал

})

client.on('message', function (topic, message) {
  // message is Buffer 
  // сигнал, который задействуется при поступлении сообщения в канал от устройства
  MQTTmessage = message.toString();
  var data = JSON.parse(MQTTmessage);
  var dataToInsert = new SensorData ({
  	sensorType: data.sensorType,
  	sensorVal: data.sensorVal
  });
  dataToInsert.save(function(err){

	 		});

  console.log(data);
})

app.get('/', function (req, res) {
	return res.send('App is working');
});

app.get('/show', function (req, res) {
	return res.send(MQTTmessage);
});

app.get('/showChart', function (req, res) {
	 fs.readFile('dataChart.html', 'utf-8', function (err, data) {

        var chartData = [];
        SensorData.find({'sensorType':'temperature'},function(err,docs){
        	var docsLength = docs.length;
        	if (docsLength > 10){
        		for (var i = docsLength-10; i<docsLength;i++){
        			chartData.push(docs[i].sensorVal);
        		}
        	} else{
        		docs.forEach(function(item,i){
        			chartData.push(item.sensorVal);        		
        		});
        	}
        	
        	var result = data.replace('{{chartData}}', JSON.stringify(chartData));
			return res.send(result);
        });
        
	})
});

app.get('/updateChart', function (req, res) {
		var chartData = [];
		SensorData.find({'sensorType':'temperature'},function(err,docs){
	        	var docsLength = docs.length;
        		if (docsLength > 10){
        			for (var i = docsLength-10; i<docsLength;i++){
        				chartData.push(docs[i].sensorVal);
        			}
        		} else{
        			docs.forEach(function(item,i){
        				chartData.push(item.sensorVal);        		
        			});
        		}
	        	var result = {
	        		"chartData":chartData,
	        	}
				return res.send(JSON.stringify(result));
	        });
});

app.get('/sendCommand', function (req, res) {
	var command = {
		commandName: req.query.commandName
	}
	console.log(JSON.stringify(command));
	client.publish(publishPath,JSON.stringify(command));  

	return res.send("Command was sent");
	
});

app.get('/start', function (req, res) {
	timerID = setInterval(function() {
  						        	client.publish(publishPath,'Some command');  
				              },5000);
	return res.send("Sending started");
	
});
app.get('/stop', function (req, res) {
	clearInterval(timerID);
	return res.send("Sending stopped");
});
//process.env.VCAP_APP_PORT for Bluemix instead of 3000
app.listen(process.env.VCAP_APP_PORT,function(){
	console.log("Start on 3000");
});