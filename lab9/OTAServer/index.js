const express = require('express');
const { networkInterfaces } = require('os');
const path = require('path');
 
const app = express();
const nets = networkInterfaces();
 
// Server port
const PORT = 3000;
 
app.get('/', (request, response) => response.send('Hello'));
 
let downloadCounter = 1;
app.get('/firmware/9.2.ino.bin', (request, response) => {
    response.download(path.join(__dirname, 'firmware/9.2.ino.bin'), '9.2.ino.bin', (err)=>{
        if (err) {
            console.error("Problem on download firmware: ", err)
        }else{
            downloadCounter++;
        }
    });
    console.log('Your file has been downloaded '+downloadCounter+' times!')
})

app.listen(PORT, () => {
    const results = {};
 
    for (const name of Object.keys(nets)) {
        for (const net of nets[name]) {
            // Skip over non-IPv4 and internal (i.e. 127.0.0.1) addresses
            if (net.family === 'IPv4' && !net.internal) {
                if (!results[name]) {
                    results[name] = [];
                }
                results[name].push(net.address);
            }
        }
    }
 
    console.log('Listening on port '+PORT+'\n', results)
});