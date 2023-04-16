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

const level = {
  audio: 'https://sheeptester-forks.github.io/cse110-lab-2/meeting.m4a',
  obstacles: [
    { type: 'left', time: 0.9 },
    { type: 'left', time: 1.8 },
    { type: 'left', time: 2.7 },
    { type: 'right', time: 3.1 },
    { type: 'left', time: 3.5 }
  ]
}

const wow = document.getElementById('wow')
const leftRow = document.getElementById('left-row')
const rightRow = document.getElementById('right-row')
const left = document.getElementById('left')
const right = document.getElementById('right')
const scoreDisplay = document.getElementById('score')

const audio = new Audio(level.audio)
window.audio = audio
for (const obstacle of level.obstacles) {
  obstacle.element = Object.assign(document.createElement('div'), {
    className: 'square obstacle'
  })
  if (obstacle.type === 'left') {
    leftRow.append(obstacle.element)
  } else {
    rightRow.append(obstacle.element)
  }
}

let tilt = 0
let targetTilt = 0
let score = 0

// Based on https://github.com/SheepTester/words-go-here/blob/master/misc/artificial-thonkulos/render-simulation.mjs
const SIM_STEP = 5 // ms per simulation
const MAX_DELAY = 500 // ms
let idealSimTime = 0
let simulatedTime = 0
function simulate () {
  tilt += (targetTilt - tilt) * 0.1
  if (Math.abs(tilt) < Number.EPSILON) {
    tilt = 0
  }
}

function addScore (diff) {
  score += diff
  scoreDisplay.textContent = Math.floor(score)
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
  const currTime = audio.currentTime
  for (const obstacle of level.obstacles) {
    if (currTime > obstacle.time + 0.3) {
      obstacle.type = 'gone'
      obstacle.element.remove()
      addScore(-30)
    } else if (obstacle.time >= currTime - 2) {
      obstacle.element.style.bottom = `${
        (obstacle.time - currTime) * 500 + 80
      }px`
      obstacle.element.style.display = null
    } else {
      obstacle.element.style.display = 'none'
    }
  }
  level.obstacles = level.obstacles.filter(a => a.type !== 'gone')
  window.requestAnimationFrame(paint)
}
paint()

const SQ_SIZE = 70
function didItHit (side) {
  const currTime = audio.currentTime
  for (const obstacle of level.obstacles) {
    if (obstacle.type !== side) {
      continue
    }
    const y = (obstacle.time - currTime) * 500
    if (y >= -SQ_SIZE && y < SQ_SIZE) {
      obstacle.element.remove()
      obstacle.type = 'gone'
      addScore(Math.exp(-((y / (SQ_SIZE / 2)) ** 2)) * 100)
      break
    }
  }
  addScore(-5)
}

async function connect () {
  const port = await navigator.serial.requestPort().catch(() => null)
  if (port === null) {
    return
  }
  document.getElementById('note').remove()
  await port.open({ baudRate: 115200 })
  audio.play()
  let wasLeft = false
  let wasRight = false
  for await (const entry of packets(port)) {
    if (entry.leftBtn) {
      left.classList.add('active')
      if (!wasLeft) {
        didItHit('left')
      }
    } else {
      left.classList.remove('active')
    }
    wasLeft = entry.leftBtn
    if (entry.rightBtn) {
      right.classList.add('active')
      if (!wasRight) {
        didItHit('right')
      }
    } else {
      right.classList.remove('active')
    }
    wasRight = entry.rightBtn
    targetTilt = entry.sliderPressed ? entry.sliderPos * 20 - 10 : 0
  }
}
document.addEventListener('click', connect, { once: true })
