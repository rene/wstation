function showHidePass()
{
	var x = document.getElementById("wpass");
	if (x.type == "password") {
		x.type = "text";
	} else {
		x.type = "password";
	}
}
		
function populateWifi()
{
	var xhttp = new XMLHttpRequest();

	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			var list = document.getElementById("wifilist");
			var networks = JSON.parse(this.responseText);
			list.innerHTML = "";
			for (i = 0; i < networks.length; i++) {
				var percent = (0.92076*networks[i].rssi) + 104.50727;
				var netstr  = networks[i].ssid + " (" + Math.round(percent) + "%)";
				list.innerHTML += "<option value='" + networks[i].ssid + 
					"'>" + netstr + "</option>";
			}
		}
	};

	xhttp.open("GET", "/scan", true);
	xhttp.send();
}

function chooseSSID()
{
	var sel = document.getElementById("wifilist");
	var inp = document.getElementById("frmSSID");
	inp.value = sel.value;
}

function resetDevice()
{
	if (confirm("Are you sure you want to reset the device?")) {
		// TODO implement
		window.alert("Device reseted!");
	}
}

function resetToFactory()
{
	if (confirm("Reset to factory settings? This action cannot be undone!")) {
		// TODO implement
		window.alert("Reset to factory!");
	}
}
