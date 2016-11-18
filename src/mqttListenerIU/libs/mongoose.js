var mongoose    = require('mongoose'),
	Schema = mongoose.Schema,
	autoIncrement = require('mongoose-auto-increment');

var connection = mongoose.createConnection("mongodb://admin:XOLEFJURAVGNLPJF@sl-us-dal-9-portal.4.dblayer.com:16416/admin?ssl=true");
 
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