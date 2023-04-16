// deno run --allow-run=cmd.exe main.ts

import { writeAll } from 'https://deno.land/std@0.183.0/streams/write_all.ts'

const serialPort = 'COM7'

const process = Deno.run({
  cmd: ['cmd.exe', '/c', 'type', serialPort],
  stdout: 'piped'
  // stderr: 'null'
})

const decoder = new TextDecoder()
for await (const chunk of process.stdout.readable) {
  console.log('!>', decoder.decode(chunk), '<!')
}
