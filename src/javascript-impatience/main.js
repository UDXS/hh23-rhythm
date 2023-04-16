const PACKET_LENGTH = 12
const MAX_SLIDER = 300
const ZERO = '0'.codePointAt(0)
const ONE = '1'.codePointAt(0)
async function * packets (port) {
  const reader = port.readable.getReader()
  let buffer = new Uint8Array(PACKET_LENGTH)
  let i = 0
  while (true) {
    const { value, done } = await reader.read()
    if (done) {
      break
    }
    for (const byte of value) {
      if (byte === ZERO || byte === ONE) {
        buffer[PACKET_LENGTH - 1 - i] = byte === ONE
        i++
        if (i === PACKET_LENGTH) {
          const entry = {
            leftBtn: buffer[0] === 1,
            rightBtn: buffer[1] === 1,
            sliderPressed: buffer[2] === 1,
            sliderPos: 0
          }
          for (let i = 3; i < PACKET_LENGTH; i++) {
            entry.sliderPos |= buffer[i] << (i - 3)
          }
          entry.sliderPos /= MAX_SLIDER
          yield entry
        } else if (i > PACKET_LENGTH) {
          throw new Error('Too many bits??')
        }
      } else if (i !== 0) {
        if (i < PACKET_LENGTH) {
          throw new Error('Bits cut off unexpectedly')
        } else {
          i = 0
        }
      }
    }
  }
  reader.releaseLock()
}

const wow = document.getElementById('wow')
const left = document.getElementById('left')
const right = document.getElementById('right')

let tilt = 0
let targetTilt = 0

// Based on https://github.com/SheepTester/words-go-here/blob/master/misc/artificial-thonkulos/render-simulation.mjs
const SIM_STEP = 5 // ms per simulation
const MAX_DELAY = 500 // ms
let idealSimTime = 0
let simulatedTime = 0
function simulate () {
  tilt += (targetTilt - tilt) * 0.1
}

let lastTime = Date.now()
function paint () {
  const now = Date.now()
  const elapsed = now - lastTime
  if (elapsed < MAX_DELAY) {
    idealSimTime += elapsed
    while (simulatedTime < idealSimTime) {
      simulate()
      simulatedTime += SIM_STEP
    }
  }
  lastTime = now
  wow.style.setProperty('--tilt', `${tilt}deg`)
  window.requestAnimationFrame(paint)
}
paint()

async function connect () {
  document.getElementById('note').remove()
  const port = await navigator.serial.requestPort()
  await port.open({ baudRate: 115200 })
  for await (const entry of packets(port)) {
    if (entry.leftBtn) {
      left.classList.add('active')
    } else {
      left.classList.remove('active')
    }
    if (entry.rightBtn) {
      right.classList.add('active')
    } else {
      right.classList.remove('active')
    }
    targetTilt = entry.sliderPressed ? entry.sliderPos * 20 - 10 : 0
  }
}
document.addEventListener('click', connect, { once: true })
