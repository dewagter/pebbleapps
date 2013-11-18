Pebble.addEventListener("ready",
    function(e) {
        console.log("Buienradar: Ready Event.");
        var req = new XMLHttpRequest();
        req.open('GET', "http://gps.buienradar.nl/getrr.php?lat=52.3982969&lon=4.6431016", true);
        req.onload = function(e) {
          if (req.readyState == 4) {
            if(req.status == 200 /* 200 - HTTP OK */) {
              console.log(req.responseText);
              response = req.responseText;
              Pebble.showSimpleNotificationOnPebble("BuienRadar", "Haarlem\n" + response);
            } else {
              console.log("Request returned error code " + req.status.toString());
            }
          }
          else
          {
            console.log("Request not state 4 " + req.readyState.toString());
          }
        }
        req.send(null);
    }
);
