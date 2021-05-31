var router = require('express').Router();
var path = require('path');

router.get('/', function(req, res) {
    res.status(200).json({
        success: true,
        message: 'alive'
    });
});


module.exports = router;