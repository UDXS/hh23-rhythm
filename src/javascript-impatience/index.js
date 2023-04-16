// node.exe index.js COM7

import { SerialPort } from 'serialport'

const [, , portPath] = process.argv
const port = new SerialPort({
  path: portPath,
  baudRate: 115200
})
port.on('open', () => {
  console.log('open')
})
port.on('readable', () => {
  console.log(port.read())
})
port.on('data', data => {
  console.log(data)
})
port.on('close', () => {
  console.log('close')
})
