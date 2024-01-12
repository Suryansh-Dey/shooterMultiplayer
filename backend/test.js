let b = 1
setTimeout(()=>{
    console.log(b)
    b++
},1000)
setTimeout(()=>{
    console.log(b)
},2000)
