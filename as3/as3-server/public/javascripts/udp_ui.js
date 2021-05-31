"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
var timeoutError;
var timeoutUDP;
$(document).ready(function() {

	setInterval(checkSecond, 1000);

	$('#modeNone').click(function(){
		sendCommand("beat stop");
	});
	
	$('#modeRock1').click(function(){
		sendCommand("beat rock");
	});
	
	$('#modeCustom1').click(function(){
		sendCommand("beat custom1");
	});
	
	$('#modeCustom2').click(function(){
		sendCommand("beat custom2");
	});
	$('#tempoUp').click(function(){
		sendCommand("tempo increase");
	});
	$('#tempoDown').click(function(){
		sendCommand("tempo decrease");
	});
	$('#volumeUp').click(function(){
		sendCommand("volume increase");
	});
	$('#volumeDown').click(function(){
		sendCommand("volume decrease");
	});
	$('#base').click(function(){
		sendCommand("play base");
	});
	$('#snare').click(function(){
		sendCommand("play snare");
	});
	$('#hihat').click(function(){
		sendCommand("play hihat");
	});
	
	socket.on('commandReply', function(result) {
		var commasep = result.split(',');
		if (commasep[0] === "bbgdata") {
			clearTimeout(timeoutUDP);
			updateStats(commasep);
		}
	});
	
});



function showError(reason) {
	clearTimeout(timeoutError);
	if (reason === "node") {
		var ebox = document.getElementById("error-box");
		ebox.style.display = "block";
		ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>";
		timeoutError = setTimeout(function () {
			ebox.innerHTML = "";
			ebox.style.display = "none";
		}, 10000);
	}
	else {
		var ebox = document.getElementById("error-box");
		ebox.style.display = "block";
		if (ebox.innerHTML === "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>") {
			ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>";
		}
		else {
			ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach Beatbox C program.</h2>";
		}
		timeoutError = setTimeout(function () {
			ebox.innerHTML = "";
			ebox.style.display = "none";
		}, 10000);
	}
}

function checkSecond() {
	sendCommand("check");
	$.ajax({
		url: "/nodeCheck",
		type: 'GET',
		dataType: 'json',
		error : function(err) {
		  console.log('Error!', err);
		  showError("node");
		},
		success: function(data) {
		  console.log('Node Server still going!');
		}
	  });
	  timeoutUDP = setTimeout(function () {
		showError("udp");
	}, 1000);
}

function updateStats(commasep) {
	var volume = document.getElementById("volumeid");
	volume.value = commasep[2];
	var tempo = document.getElementById("tempoid");
	tempo.value = commasep[3];
	//seconds to timestamp based off of
	//https://stackoverflow.com/questions/1322732/convert-seconds-to-hh-mm-ss-with-javascript
	var uptime = new Date(commasep[1] * 1000).toISOString().substr(11, 8);
	//console.log(uptime);
	var status = document.getElementById("status");
	status.innerHTML = uptime;
	var beat = document.getElementById("modeid");
	console.log("cs4" + commasep[4]);
	if (commasep[4].trim() == '0') {
		beat.innerHTML = "STOP";
	}
	else if (commasep[4].trim() == '1') {
		beat.innerHTML = "ROCK";
	}
	else if (commasep[4].trim() == '2') {
		beat.innerHTML = "CUSTOM1";
	}
	else if (commasep[4].trim() == '3') {
		beat.innerHTML = "CUSTOM2";
	}
}

function sendCommand(message) {
	socket.emit('prime', message);
};