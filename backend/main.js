const express = require("express")
let app = express()
let router = express.Router()
router.get('/',(req,res)=>{
    res.send("Welcome to router")
})
router.get('/about',(req,res)=>
{
    res.send("I am a router what else")
})
app.use("/router",router)
app.get("/",(req,res)=>{
    res.send("Hello world")
})
app.get("/quit",(req,res)=>{
    res.send("Server closed!")
    server.close()
})
app.get("/:print",(req,res)=>{
    res.send(req.params.print)
})
let server = app.listen(3000,()=>{
    console.log("Server started on port 3000")
})