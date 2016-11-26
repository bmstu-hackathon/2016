
function send_mqtt_data() {
	console.log(this.getAttribute('data'));
	var sendData = {
		command:JSON.stringify({
			commandName:"blockPlace",
			lotID:this.getAttribute('data')
		})
	};        

	$.ajax({
		type: "GET",
		url: "http://" + location.host + "/sendCommand",
		data: sendData,
		dataType: "text",
		success: function(res) {
			console.log(res);
		},
		error: function(err){
			console.log(err);
		} 
	});
        //// ТУТ ВОТ ОТПРАВИШЬ ПО MQTT
}

function update_parking_status(){
	$.ajax({
		type: "GET",
		url: "http://" + location.host + "/update",
		dataType: "json",
		success: function(res) {
			var i=0;
			console.log(res)
			res.forEach(function(item){
				if(item[1] == "free"){
					car_cell__inner[item[0]-1].style.background = "green";
				}else{
					if(item[1] == "blocked"){
						car_cell__inner[item[0]-1].style.background = "red";
					}else{
						car_cell__inner[item[0]-1].style.background = "yellow";
					}
				}
				i++;
			})
				 
		}
	});
}
setInterval(update_parking_status, 1000);


car_cells = document.getElementsByClassName("car-cell");
car_cell__inner = document.getElementsByClassName("car-cell__inner");

[].forEach.call(car_cells, function(e) {
	e.addEventListener('click', send_mqtt_data, false)
});
$('.car-cell__inner').each(function(i, obj) {
					obj.style.background = "green";
				});