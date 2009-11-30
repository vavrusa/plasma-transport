var Service = {

   name: 'Vlak.cz',
   method: 'POST',
   url: 'http://www.vlak.cz/ConnForm.asp?tt=a&p=CD&ConnAlg=1&link=9EB3',
   qmap: { 'date' : 'ConnDate',
           'time' : 'ConnTime',
           'from' : 'FromStn',
           'to'   : 'ToStn'
   },
   result: null,

   parse: function(html) {

      // Clear result
      this.result = new Array();
      var i = html.indexOf("<tr valign=\"top\">");
      while(i != -1) {
         // Find block
         var end = html.indexOf("</tr>", i + 1);
         var str = html.substr(i, end - i);

         // Find matches
         var arr = null;
         var date  = str.match(/\d{2}\.\d{2}\./);
         arr = str.match(/\d+\:\d+/g);
         var arrivs = arr.slice(0, arr.length / 2).reverse();
         var departs = arr.slice(arr.length / 2, arr.length).reverse();
         arr = str.match(/([^\<\>]+)(?=\<\/a\>)/g);
         var stations = arr.slice(0, arr.length / 2 + 1).reverse();
         var trains = arr.slice(arr.length / 2 + 1, arr.length).reverse();

         // Fill transits
         // 1. first has no arrival information
         // 2. transit points
         // 3. last has no departure information
         var transits = [];

         // Station, Train, Arrival, Departure
         transits.push([stations.pop(), trains.pop(),
                        "", departs.pop()]);

         while(departs.length > 0) {
            transits.push([stations.pop(), trains.pop(),
                           arrivs.pop(), departs.pop()]);
         }

         transits.push([stations.pop(), "",
                        arrivs.pop(), ""]);

         // Push new transit
         this.result.push({ "codepage" : "Windows-1250",
                            "date"     : date,
                            "transits" : transits
                          });


         // Next block
         i = html.indexOf("<tr valign=\"top\">", end)
      }

      // Dump array
      print(this.result);
   }
}
