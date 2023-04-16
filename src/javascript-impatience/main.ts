// deno run --allow-run=powershell.exe main.ts

import { writeAll } from 'https://deno.land/std@0.183.0/streams/write_all.ts'

const serialPort = 'COM7'

const process = Deno.run({
  cmd: ['powershell.exe'],
  stdin: 'piped',
  stdout: 'piped'
})

const encoder = new TextEncoder()
const decoder = new TextDecoder()

await writeAll(
  process.stdin,
  encoder.encode(
    `$port = new-Object System.IO.Ports.SerialPort ${serialPort},115200,None,8,one\n$port.Open();\n`
  )
)

while (true) {
  await writeAll(process.stdin, encoder.encode(`$port.ReadLine();\n`))
  const arr = new Uint8Array(80)
  await process.stdout.read(arr)
  console.log(decoder.decode(arr))
}
