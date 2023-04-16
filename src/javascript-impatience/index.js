// node index.js COM7

import { SerialPort } from 'serialport'

const [, portPath] = process.argv

const port = new SerialPort({
  path: portPath,
  baudRate: 115200
})
port.on('readable', function () {
  console.log(port.read())
})
