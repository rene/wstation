function showHidePass(elem)
{
	var x = document.getElementById(elem);
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
			list.innerHTML = "<option value=''>            </option>";
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

function loadPage()
{
	var tzone  = document.getElementById("tzone");
	var cbox   = document.getElementById("timezone");
	var dlight = document.getElementById("dlight");
	var ckbox  = document.getElementById("daylight");
	var tempsc = document.getElementById("tempsc");
	var tscale = document.getElementById("tempscale");

	cbox.value   = tzone.value;
	tscale.value = tempsc.value;

	if (dlight.value != 0) {
		ckbox.checked = true;
	} else {
		ckbox.checked = false;
	}

	setTimeout(populateWifi, 1500);
}

function resetForm()
{
	var frm = document.getElementById("frmconf");
	var wpass = document.getElementById("wpass");
	var upass = document.getElementById("userpass");
	wpass.type = "password";
	upass.type = "password";
	frm.reset();
	loadPage();
}

function logout()
{
	if (confirm("Proceed to logout?")) {
		var xhttp = new XMLHttpRequest();
		xhttp.open("GET", "/logout", true);
		xhttp.send();
		xhttp.onload = function() {
			if (xhttp.status != 401) {
				alert("Unknown error!");
			}
			window.location = "/login";
		};
	}
}

function chooseSSID()
{
	var sel = document.getElementById("wifilist");
	var inp = document.getElementById("frmSSID");
	inp.value = sel.value;
}

function sleep(ms)
{
  return new Promise(resolve => setTimeout(resolve, ms));
}

function updateFirmware()
{
	var contents = document.getElementById("contents");
	var elem     = document.getElementById("pbar");
	var msgbox   = document.getElementById("msgbox");
	var msg      = document.getElementById("pmsg");
	var frmf     = document.getElementById("frmFirmware");
	var xhttp    = new XMLHttpRequest();
	var file     = document.getElementById("firmfile").files[0];
	var frmData  = new FormData();

	if(!file)
		return;

	// Form data
	frmData.append("firmfile", file);

	// Callbacks
	xhttp.onerror = function(e) {
		msg.innerHTML = "";
		msgbox.style.display   = "none";
		contents.style.opacity = 1.0;
		window.alert("Firmware uploading error!");
		location.reload();
	};
	xhttp.onreadystatechange = async function(e) {
		if(xhttp.readyState === XMLHttpRequest.DONE) {
			var status = xhttp.status;
			var i;

			if (status == 200) {
				elem.innerHTML = "";
				for (i = 10; i >= 0; i--) {
					msg.innerHTML = "Done. Waiting for device reset... " + i + "s";
					elem.style.width = ((10-i) * 10) + "%";
					await sleep(1000);
				}
			} else if (status == 408) {
				window.alert("Firmware update failed!");
			} else {
				window.alert("Unknown error!");
			}

			msg.innerHTML = "";
			msgbox.style.display   = "none";
			contents.style.opacity = 1.0;
			frmf.reset();

			location.reload();
		}
	};
	xhttp.upload.onprogress = function(e) {
		var p = Math.round(100 / e.total * e.loaded);
		elem.style.width = p + "%";
		elem.innerHTML = p + "%";
	};
	xhttp.onabort = function(e) {
		msg.innerHTML = "";
		msgbox.style.display   = "none";
		contents.style.opacity = 1.0;
		window.alert("Firmware uploading aborted!");
		location.reload();
	};

	// Progress bar
	contents.style.opacity = 0.2;
	msgbox.style.display   = "block";
	msg.innerHTML = "UPDATING DEVICE FIRMWARE.... DO NOT TURN OFF!<br/>Please, wait...";

	// Submit data
	xhttp.open("POST", "/updateFirmware");
	xhttp.send(frmData);
}

function requestReset(request)
{
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", request, true);
	xhttp.send();
}

async function waitWindow()
{
	var contents = document.getElementById("contents");
	var elem     = document.getElementById("pbar");
	var msgbox   = document.getElementById("msgbox");
	var msg      = document.getElementById("pmsg");
	var width    = 1;
	var i;
	
	contents.style.opacity = 0.2;
	msgbox.style.display   = "block";
	msg.innerHTML = "Please, wait...";

	for (i = 0; i < 100; i++) {
		elem.style.width = width + "%";
		elem.innerHTML = width + "%";
		width++;
		await sleep(140);
	}

	msg.innerHTML = "";
	msgbox.style.display   = "none";
	contents.style.opacity = 1.0;
	window.alert("Done!");
	location.reload();
}

function resetDevice()
{
	if (confirm("Are you sure you want to reset the device?")) {
		requestReset("/resetDevice");
		waitWindow();
	}
}

function resetToFactory()
{
	if (confirm("Reset to factory settings? This action cannot be undone!")) {
		requestReset("/resetToFactory");
		waitWindow();
	}
}
