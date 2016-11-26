//подключение библиотек
var mqtt = require('mqtt'), 
	express = require('express'),
	path = require("path"),
	conf = require('./config.json'),
	fs = require('fs'),
	ParkingData = require('./libs/mongoose').ParkingData,
	app = express();

app.use (express.static(__dirname + '/public'));
app.set('view engine', 'jade');
// опции для подключения к MQTT Broker Bluemix
var options = {
		clientId:'a:' + conf.BLUEMIXAPPID + ':' + conf.appID,
		username:conf.username,
		password:conf.password
	}, 
	MQTTmessage="no message", // буфер для хранения сообщения
	statuses = ["free","blocked","reserved"],
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
  var data = JSON.parse(MQTTmessage), flag = false;
  for (var i=0; i<3;i++){
  	if (data.status == statuses[i]){
  		flag = true;
  	}
  }
  if (flag){
  	var dataToInsert = new ParkingData ({
  			lotID: data.lotID,
  			status: data.status
  		});
  	ParkingData.findOneAndUpdate({'lotID':data.lotID},dataToInsert,function(err, lot){
  		if (lot == null){
  			console.log("Was inserted")
  			dataToInsert.save(function(err){
	
  			})
  		}else{
  		  	console.log("Was updated");
  		}

  	})

  }else{
  	console.log("Wrong status");
  }  
  
  
})

app.get('/', function (req, res) {

	return res.sendFile(path.join(__dirname+'/parking.html'))
});

app.get('/show', function (req, res) {
	return res.send(MQTTmessage);
});


app.get('/sendCommand', function (req, res) {
	
	client.publish(publishPath,req.query.command);  

	return res.send("Command was sent");
	
});

app.get('/update', function (req, res) {
	ParkingData.find(function(err,docs){
			var result = [];
		docs.forEach(function(item,i){
			var elem = [2];
			elem[0] = item.lotID;
			elem[1] = item.status;
        	result.push(elem); 

        });
        

        return res.send(JSON.stringify(result));
	})
	
});
var test = {
	commandName:"blockPlace",
	lotID:"1_3"
}

app.get('/start', function (req, res) {
	timerID = setInterval(function() {
  						        	client.publish(publishPath,JSON.stringify(test));  
				              },5000);
	return res.send("Sending started");
	
});
app.get('/stop', function (req, res) {
	clearInterval(timerID);
	return res.send("Sending stopped");
});
//process.env.VCAP_APP_PORT for Bluemix instead of 3000
app.listen(3000,function(){
	console.log("Start on 3000");
});