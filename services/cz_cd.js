function stripTags(html) {
   html = html.replace(/<\/?[^>]+>/g, '');
   html = html.replace(/\s$/, '');
   html = html.replace(/^\s/, '');
   return html;
}

var Service = {

   name: 'cd.cz/spojeni',
   method: 'POST',

   /* TODO: Check if __VIEWSTATE varies (mandatory).
    */
   url: 'http://www.cd.cz/spojeni/conn.aspx?form-odjezd=true&cmdSearch=Vyhledat&__VIEWSTATE=/wEPDwUJOTY5MDI2NDQ1DxYCHglQQUdFU0NFTkELKWJDSEFQUy5WU3BvakNELldlYi5QQUdFU0NFTkEsIFZTcG9qQ0QsIFZlcnNpb249MS4wLjM2MzcuMTk5MDgsIEN1bHR1cmU9bmV1dHJhbCwgUHVibGljS2V5VG9rZW49bnVsbABkZA==',
   qmap: { 'date-format' : 'd.M.yyyy',
           'date' : 'form-datum',
           'time' : 'form-cas',
           'from' : 'FROM_0t',
           'to'   : 'TO_0t'
   },
   codepage: 'UTF-8',
   result: null,

   parse: function(html) {

      this.result = new Array();
      var i = html.indexOf('<table class="jointissue conntbl">');
      while(i != -1) {
         // Find block
         var end = html.indexOf('</table>', i + 1);
         var str = html.substr(i, end - i);

         // Find matches
         var arr = null;
         var stations = [];
         var times    = [];
         var trains   = [];
         var date     = str.match(/\d{1,2}\.\d{1,2}\./);

         // Stations
         arr = str.match(/<td(| class="highlight")>(|<strong>)([^<]+)/g);
         for(var k = 0; k < arr.length; ++k) {
            arr[k] = stripTags(arr[k]);
            if(arr[k].length > 1)
               stations.unshift(arr[k]);
         }

         // Arrivals, departures
         arr = str.match(/<td class="timec (|highlight)">(|<strong>)[^<]+/g);
         for(var k = 0; k < arr.length; ++k)
            times.unshift(stripTags(arr[k]));

         // Trains
         arr = str.match(/<a href=\"Route[^>]+>[^<]+/g);
         for(var k = 0; k < arr.length; ++k)
            trains.unshift(stripTags(arr[k]));

         // Fill transits
         // 1. first has no arrival information
         // 2. transit points
         // 3. last has no departure information
         var transits = [];

         // Station, Train, Arrival, Departure
         transits.push([stations.pop(), trains.pop(), "", times.pop()]);
         while(stations.length > 1)
            transits.push([stations.pop(), trains.pop(), times.pop(), times.pop()]);
         transits.push([stations.pop(), "", times.pop(), ""]);

         // Push new transit
         this.result.push({ 'date'     : date,
                            'transits' : transits
                          });


         // Next block
         i = html.indexOf('<table class="jointissue conntbl">', end)
      }
   }
}
