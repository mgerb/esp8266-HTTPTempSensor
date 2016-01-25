const char page_index[] PROGMEM = R"=====(

<!DOCTYPE html>

<html>
<head>
<title>Configuration</title>
</head>

<body>
  
    <div class="container">
      
          <form action="config" method="post" class="form-signin">
            <h2 class="form-signin-heading center">Sensor Config</h2>
            <label for="inputSSID" class="sr-only">Access Point SSID</label>
            <input name="ssid" type="text" id="inputSSID" class="form-control" placeholder="SSID" required autofocus>
            <br>
            <label for="inputPassword" class="sr-only">Password</label>
            <input name="password" type="password" id="inputPassword" class="form-control" placeholder="Password" required>
            <br>
            <label for="inputName" class="sr-only">Sensor Name</label>
            <input name="name" type="text" id="inputName" class="form-control" placeholder="Sensor Name" required>
            <br>
            <label for"inputServerPassword" class="sr-only">Server Password</label>
            <input name="serverPassword" type="password" id="inputServerPassword" class="form-control" placeholder="Server Password" required>
            <br>
            <button class="btn btn-lg btn-primary btn-block" type="submit">Submit</button>
          </form>
      

    </div> <!-- /container -->


</body>


</html>

)=====";
