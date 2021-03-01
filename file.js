const net = require("net")
let client = new net.Socket()

const Mode = {
    OFF: 0,
    SOLID: 1,
    SOLID_RARE: 2,
    TO_SLEEP: 3,
    TIME_CURSOR: 4,

    SECTION_SOLID: 5,

    EASE_ANIM: 10,

    FADE_ANIM: 12,
}

const HOST = "192.168.4.1"

let data = Buffer.from([Mode.SOLID_RARE, 0, 0, 0, 0]) //# все диоды не активны
 //let data = Buffer.from([Mode.FADE_ANIM, 0, 0, 128, 0]) //# мерцание синим цветом 
//let data = Buffer.from([Mode.SOLID_RARE, 0, 20, 243, 70]) //# 20 70 243 - мятный - горит
//let data = Buffer.from([Mode.SOLID_RARE, 0, 20, 114, 243]) //#1472f3 20 114 243
// let data = Buffer.from([Mode.SOLID_RARE, 0, 255, 255, 255]) //full bright
//let data = Buffer.from([Mode.SOLID_RARE, 10, 0, 25, 0])
 //let data = Buffer.from([Mode.SOLID_RARE, 6, 0, 0, 0])
//let data = Buffer.from([Mode.SOLID_RARE, 1, 255, 0])
// let data = Buffer.from([Mode.FADE_ANIM, 15,34,70])
 //let data = Buffer.concat([Buffer.from([Mode.TIME_CURSOR, 0, 0, 255]), Buffer.from('10 ', 'utf8')])
// let data = Buffer.concat([
//     Buffer.from([Mode.SECTION_SOLID, 0, 255, 0]),
//     Buffer.from('0 5 ', 'utf8')
// ])
 //let data = Buffer.concat([
 //    Buffer.from([Mode.SECTION_SOLID, 20, 243, 70]),
 //    Buffer.from('10 20 ', 'utf8')
// ])

function send(data) {
    client.connect({port: 8888, host: HOST}, function () {
        console.log("TCP connection established with the server.")
        client.write(data)
    })
}
send(data)

// FADE_ANIM - мерцание // режими передаются константными цифрами, можно увидеть выше  
//TIME_CURSOR - пробег по кругу каждого светодиода
// SOLID_RARE просто светит
// 0, 0, 0, 0]) - первый параметр лучше не трогать, можно передавать просто 3, отвечают за цвет
// rgb просто байты. Когда переключаются режимы, лучше выполнять сначала ([Mode.SOLID_RARE, 0, 0, 0, 0]) //# все диоды не активны
// т.к. ведет себя немного неадекватно без этого
// 