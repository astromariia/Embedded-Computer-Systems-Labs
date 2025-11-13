if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      var bumper = e.data[1]
      var cliff = e.data[3];
      var drop = e.data[5];


      // finish the code to handle the bumper status
        if (bumper=="0")
          {
            document.getElementById("bumper").value = "OFF";
          }
        if (bumper=="1")
        {
          document.getElementById("bumper").value = "Right";
        }
        if (bumper=="2")
        {
          document.getElementById("bumper").value = "Forward";
        }
        if (bumper=="3")
        {
            document.getElementById("bumper").value = "Right Forward";
        }
        if (bumper == "4"){
          document.getElementById("bumper").value = "Left";
        }
        if (bumper == "6"){
          document.getElementById("bumper").value = "Left Forward";
        }
        
        // finish the code to handle the wheel drop status 
      if (drop=="0")
        {
          document.getElementById("wheeldrop").value = "OFF";

        }
      if (drop=="3")
        {
          document.getElementById("wheeldrop").value = "ON";

        }
      

      // finish the code to handle cliff status 
      if (cliff=="0")
        {
          document.getElementById("cliff").value = "OFF";
        }
      if (cliff=="1")
        {
          document.getElementById("cliff").value = "ON";
        }
      if (cliff=="2")
        {
          document.getElementById("cliff").value = "ON";
        }
      if (cliff=="4")
        {
          document.getElementById("cliff").value = "ON";
        }
      


    }
  }