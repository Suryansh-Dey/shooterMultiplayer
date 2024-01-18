const express = require("express")
const Games = require("./games.js")

let app = express()
let games = new Games

app.use((req,res,next)=>{
    console.log("Current state: ",games.games)
    next()
})
app.get("/update", (req, res) => {
    games.update(req.query.id, req.query.state)
    res.send(games.fetch(req.query.id))
})
app.get("/available", (req,res)=>{
    res.send(games.isAvailable(req.query.id))
})
app.get("/join", (req, res) => {
    if (req.query.id == "random")
    res.send(games.joinRandom())
else res.send(games.joinByCode(req.query.id))
})
app.get("/code",(req,res)=>{
    res.send(games.generateCode())
})
app.get("/quit", (req, res) => {
    games.quit(req.query.id)
    res.send("Bye")
})
app.get("/", (req, res) => {
    res.send("Welcome to the shooter multiplayer server by suryansh dey")
})
app.get("/CLOSESERVER", (req, res) => {
    res.send("Server closed!")
    server.close()
})

let server = app.listen(3000, () => {
    console.log("Server started on port 3000")
})