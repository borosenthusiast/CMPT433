var router = require('express').Router();
var path = require('path');
var file_path = "/../public/";

router.get('/', function(req, res) {
	res.sendFile(path.join(__dirname + file_path + "index.html"));
});

router.get('/style.css', function(req, res) {
	res.sendFile(path.join(__dirname + file_path + "stylesheets/style.css"));
});

router.get('/udp_ui.js', function(req, res) {
	res.sendFile(path.join(__dirname + file_path + "javascripts/udp_ui.js"));
});

module.exports = router;