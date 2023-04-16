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
  // this is for the type completion
  obstacles: [{ type: 'left', time: 0, element: document.body }]
}
const { obstacles, audio: audioUrl } = await fetch('./level.json').then(r =>
  r.json()
)
level.obstacles = structuredClone(obstacles)
const audio = new Audio(audioUrl)
window.audio = audio

async function reset () {
  for (const obstacle of level.obstacles) {
    obstacle.element?.remove()
  }

  audio.currentTime = 0
  level.obstacles = structuredClone(obstacles)
  for (const obstacle of level.obstacles) {
    if (obstacle.type === 'left' || obstacle.type === 'right') {
      obstacle.element = Object.assign(document.createElement('div'), {
        className: 'square obstacle'
      })
      if (obstacle.type === 'left') {
        leftRow.append(obstacle.element)
      } else {
        rightRow.append(obstacle.element)
      }
    } else {
      obstacle.element = Object.assign(document.createElement('div'), {
        className: `qs ${obstacle.type}`
      })
      wow.append(obstacle.element)
    }
  }
}
document.getElementById('reset').addEventListener('click', reset)

const wow = document.getElementById('wow')
const leftRow = document.getElementById('left-row')
const rightRow = document.getElementById('right-row')
const left = document.getElementById('left')
const right = document.getElementById('right')
const scoreDisplay = document.getElementById('score')

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
window.animationId = null
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
  window.animationId = window.requestAnimationFrame(paint)
}

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
  let slideDown = null
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
    const currTime = audio.currentTime
    if (entry.sliderPressed) {
      if (slideDown === null) {
        slideDown = {
          time: currTime,
          pos: entry.sliderPos
        }
      }
    } else if (slideDown !== null) {
      const diff = entry.sliderPos - slideDown.pos
      const TIME_THRESHOLD = 0.3
      if (Math.abs(diff) > 0.5 && currTime - slideDown.time < 0.2) {
        const dir = diff > 0 ? 'right' : 'left'
        console.log(dir)
        const midpt = (slideDown.time + currTime) / 2
        for (const obstacle of level.obstacles) {
          if (obstacle.type !== 'qs-' + dir) {
            continue
          }
          if (Math.abs(midpt - currTime) < TIME_THRESHOLD) {
            obstacle.element.remove()
            obstacle.type = 'gone'
            addScore(
              Math.exp(-(((midpt - currTime) / (TIME_THRESHOLD / 2)) ** 2)) *
                300
            )
            break
          }
        }
      }
      slideDown = null
    }
  }
}
document.addEventListener('click', connect, { once: true })

reset()
paint()
