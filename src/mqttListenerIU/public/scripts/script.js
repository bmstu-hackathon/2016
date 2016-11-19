$('a').click( function(e) {
	e.preventDefault(); 
	if ($("#commandInput").val()==""){
		alert("Input command!");
	}else{
		sendFunc(); 
	}
	
	return false; 
} );
function sendFunc(){
	$.ajax({
		type: "GET",
		url: "http://" + location.host + "/sendCommand",
		data: {commandName:$("#commandInput").val()},
		dataType: "text",
		success: function(res) {
			console.log(res);
		}
	});
}