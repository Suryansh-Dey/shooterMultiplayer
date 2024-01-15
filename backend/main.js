const express = require("express")
let app = express()
let nextPlayerId = 1;
app.get("/",(req,res)=>{
    res.send("Hello world")
})
app.get("/start",(req,res)=>{
    res.send("You are connected")
})
app.get("/generateCode",(req,res)=>{
    res.send(nextPlayerId.toString());
    nextPlayerId++;
})
app.get("/actions/:id",(req,res)=>{
    res.send({movementAngle_degree : -1,
    shootingAngle_degree : -1,
    gunAngle_degree : 0,
    type: "snowBall"});
    nextPlayerId++;
})
app.get("/quit",(req,res)=>{
    res.send("Server closed!")
    server.close()
})
let server = app.listen(3000,()=>{
    console.log("Server started on port 3000")
})