!function() {
  "use strict";

  var socket = new WebSocket('ws://' + window.location.host + '/data');
  socket.onmessage = function(e) {
    var item = JSON.parse(e.data);

    switch (item.topic) {
      case 'temperature':
        processTemp(item.data);
        break;
      case 'angle':
        processAngle(item.data);
        break;
      case 'predicted':
        processPredicted(item.data);
        break;
    }
  };

  /*
   * Temperature.
   */
  var $graph = document.querySelector('#graph');

  var actual = [{x: new Date}];
  var predicted = [];

  var chart = new CanvasJS.Chart($graph, {
    axisX: {title: "Timeline"},
    axisY: {title: "Temperature"},
    data: [{
      type: "spline",
      dataPoints: actual
    }, {
      type: "line",
      dataPoints: predicted,
      lineThickness: 1,
      lineDashType: 'dashDot'
    }]
  });

  chart.render();

  function processTemp(temp) {
    var now = new Date;

    if (+now - actual[0].x > 2 * 60 * 1000)
      actual.shift();

    var point = {x: now, y: temp};

    actual.push(point);
    predicted[0] = point;

    chart.render();
  }

  function processPredicted(temp) {
    predicted[1] = {
      x: new Date(Date.now() + 60 * 1000),
      y: temp
    };

    chart.render();
  }

  /*
   * Angle.
   */
  var $angle = document.querySelector('#angle');

  function processAngle(angle) {
    $angle.innerHTML = angle;
  }

  /*
   * Button.
   */
  var $button = document.querySelector('#turn-btn');
  $button.onclick = function() {
    socket.send(true);
  };

}();
