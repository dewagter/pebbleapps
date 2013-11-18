function parse_xml(ns_xml) {
  var res = "Res=";
  var tag = false;
  var cnt = 0;
  for (i = 0; i < ns_xml.length ; i++) {
    chr = ns_xml.charAt(i);
    if (chr == '<') {
      tag = true;
      cnt = 0;
    } else {
      if (chr == '>') {
        tag = false;
/*        switch (tagname)
        {
          case 'r':  // Ritnr
            res+= '>';
            break;
          case 'i':  // Tijd
            res+= '|';
            break;
          case 'e':  // BEstemming
            res+= '|';
            break;
          case 's':  // Ritnr
            res+= '|';
            break;
          case 'e':  // Ritnr
            res+= '|';
            break;
          case 'e':  // Ritnr
            res+= '>';
            break;
          default:  // Ritnr
            res+= '|';
            break;
        }
  */
      } else {
        if (!tag) {
          if (chr != ' ')
            if (chr != '\n')
              if (tagname != 's')
                res += chr;
        }
        else
        {
          cnt++;
          if (cnt == 9)
            tagname = chr;
          if (chr == '/')
            res+='|';
        }
      }
    }
  }
  return res;
}

Pebble.addEventListener("ready",
    function(e) {
        console.log("NS-JS: ReadyEvent=" + e.ready + "Type=" + e.type);
        var req = new XMLHttpRequest();
        req.open('GET', "http://webservices.ns.nl/ns-api-avt?station=DT", true);
        req.onload = function(e) {
          if (req.readyState == 4) {
            if(req.status == 200 /* 200 - HTTP OK */) {
              console.log(req.responseText);
              response = parse_xml(req.responseText);
              Pebble.showSimpleNotificationOnPebble("NS-trein", response);
            } else {
              console.log("Request returned error code " + req.status.toString());
              response = parse_xml(req.responseText);
              Pebble.showSimpleNotificationOnPebble("NS-trein", response);
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
