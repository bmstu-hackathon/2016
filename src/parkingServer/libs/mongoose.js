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


var parkingDataSchema= new Schema({
    
    lotID: { type: String, required: true },
    status: { type: String }
});

parkingDataSchema.plugin(autoIncrement.plugin, 'parkingData');
var ParkingData = connection.model('ParkingData',parkingDataSchema);

module.exports.ParkingData = ParkingData;