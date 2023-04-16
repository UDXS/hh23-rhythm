// node index.js COM7

import { SerialPort } from 'serialport'

const decoder = new TextDecoder()

const [, , portPath] = process.argv

const port = new SerialPort({
  path: portPath,
  baudRate: 115200
})
port.on('readable', () => {
  console.log(decoder.decode(port.read(80)))
})
