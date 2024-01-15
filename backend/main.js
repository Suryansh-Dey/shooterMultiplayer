const express = require("express")
let app = express()
let nextPlayerId = 1;
app.get("/", (req, res) => {
    res.send("Hello world")
})
app.get("/start", (req, res) => {
    res.send(nextPlayerId.toString());
    console.log("A device connected: id = " + nextPlayerId)
    nextPlayerId++;
})
app.get("/state/:id", (req, res) => {
    res.send({
        movementAngle_degree: -1,
        shootingAngle_degree: -1,
        gunAngle_degree: 0,
        type: "snowBall"
    });
    console.log(req.params.id)
})
app.get("/end", (req, res) => {
    res.send("Bye")
})
app.get("/quit", (req, res) => {
    res.send("Server closed!")
    server.close()
})
let server = app.listen(3000, () => {
    console.log("Server started on port 3000")
})