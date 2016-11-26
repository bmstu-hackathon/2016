var mongoose    = require('mongoose'),
	Schema = mongoose.Schema,		
	conf = require('../config.json'),
	autoIncrement = require('mongoose-auto-increment');

var connection = mongoose.createConnection(conf.mongodb_uri);
 
autoIncrement.initialize(connection);
 



var db = mongoose.connection;

db.once('open',function callback(){
	console.log("Connected to DB");
});


var sensorDataSchema= new Schema({
    
    sensorType: { type: String, required: true },
    sensorVal: { type: String }
});

sensorDataSchema.plugin(autoIncrement.plugin, 'sensorData');
var SensorData = connection.model('SensorData',sensorDataSchema);

module.exports.SensorData = SensorData;