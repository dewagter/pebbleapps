function fetchBuienRadar() {
    var req = new XMLHttpRequest();
    req.open('GET', "http://gps.buienradar.nl/getrr.php?lat=52.3982969&lon=4.6431016", true);
    req.onload = function(e) {
      if (req.readyState == 4) {
        if(req.status == 200 /* 200 - HTTP OK */) {
          console.log(req.responseText);
          response = req.responseText;
          // if (response.length > 40)

          Pebble.showSimpleNotificationOnPebble("BuienRadar", "Haarlem\n" + response);
        } else {
          console.log("Request returned error code " + req.status.toString());
          Pebble.sendAppMessage({"status": "JS Error Response"});
        }
      }
      else
      {
        console.log("Request not state 4 " + req.readyState.toString());
        Pebble.sendAppMessage({"status": "JS Error State"});
      }
    };
    req.onerror = function (e) {
       Pebble.sendAppMessage({"status": "No internet"});
    };
    req.send(null);
    Pebble.sendAppMessage({"status": "JS waits for http"});
}



Pebble.addEventListener("ready",
    function(e) {
        console.log("Buienradar: ReadyEvent=" + e.ready + "Type=" + e.type);
        Pebble.sendAppMessage({"status": "JS ready"});
    }
);

// Set callback for appmessage events
Pebble.addEventListener("appmessage",
    function(e) {
      console.log("message");
      if (e.payload.fetch) {
        //localStorage.setItem("symbol", symbol);
        console.log("fetch");
        fetchBuienRadar();
      }
    }
);




