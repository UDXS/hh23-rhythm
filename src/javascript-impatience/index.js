// node.exe index.js COM7

import { SerialPort } from 'serialport'

const [, , portPath] = process.argv
console.log(portPath)
const port = new SerialPort({
  path: portPath,
  baudRate: 115200
})
port.on('readable', () => {
  console.log(port.read())
})
port.on('close', () => {
  console.log('close')
})
