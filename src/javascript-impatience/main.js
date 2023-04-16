await new Promise(resolve =>
  document.addEventListener('click', resolve, { once: true })
)
const port = await navigator.serial.requestPort()
await port.open({ baudRate: 115200 })

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
const lmao = document.getElementById('lmao')

for await (const entry of packets(port)) {
  lmao.textContent = [
    entry.leftBtn ? '✅' : '❌',
    entry.rightBtn ? '✅' : '❌'
  ].join(' ')
  wow.style.width = entry.sliderPos * 100 + '%'
  wow.style.opacity = entry.sliderPressed ? 1 : 0.5
}
