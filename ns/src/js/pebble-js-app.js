function parse_ns_xml(nodelist)
{
  var msg = ""; 
  for ( i=0; (i< nodelist.length) && (i<10); i++)
  {
    msg += nodelist[i].getElementsByTagName('VertrekSpoor')[0].firstChild.nodeValue + ".";
    if (nodelist[i].getElementsByTagName('VertrekVertragingTekst').length > 0) {
      msg += nodelist[i].getElementsByTagName('VertrekVertragingTekst')[0].firstChild.nodeValue.replace(" min", "") + " ";
    }
    tijd = nodelist[i].getElementsByTagName('VertrekTijd')[0].firstChild.nodeValue.split("T")[1].split(":");
    msg += tijd[0] + ":" + tijd[1] + " ";
    dest = nodelist[i].getElementsByTagName('EindBestemming')[0].firstChild.nodeValue;
    if (dest.length > 8)
      dest = dest.substring(0,8);
    msg += dest;
    msg += "\n";
  }
  console.log(msg);
  return msg;
}

function fetchNSInfo(station) {
    var req = new XMLHttpRequest();
    req.open('GET', "http://webservices.ns.nl/ns-api-avt?station=" + station, true);
    req.setRequestHeader("Authorization", "Basic " + btoa("username:password"))
    req.onload = function(e) {
      if (req.readyState == 4) {
        if(req.status == 200 /* 200 - HTTP OK */) {
          //console.log(req.responseText);
          nodelist = req.responseXML.getElementsByTagName("VertrekkendeTrein");
          response = parse_ns_xml(nodelist);
          Pebble.showSimpleNotificationOnPebble(station, response);
        } else {
          console.log("Request returned error code " + req.status.toString());
          Pebble.sendAppMessage({"status": "JS Error Login"});
        }
      }
      else
      {
        console.log("Request not state 4 " + req.readyState.toString());
        Pebble.sendAppMessage({"status": "JS Error State"});
      }
    }
    req.onerror = function (e) {
       Pebble.sendAppMessage({"status": "No internet"});
    }
    req.send(null);
    Pebble.sendAppMessage({"status": "JS waits for http"});
  }

Pebble.addEventListener("ready",
    function(e) {
        console.log("NS-JS: ReadyEvent=" + e.ready + "Type=" + e.type);
        Pebble.sendAppMessage({"status": "JS ready"});
    }
);

// Set callback for appmessage events
Pebble.addEventListener("appmessage",
    function(e) {
      console.log("message");
      if (e.payload.station) {
        station = e.payload.station;
        //localStorage.setItem("symbol", symbol);
        fetchNSInfo(station);
      }
    }
);


