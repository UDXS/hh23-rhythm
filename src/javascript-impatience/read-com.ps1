# https://devblogs.microsoft.com/powershell/writing-and-reading-info-from-serial-ports/

$serialPort = $args[0]
$port = new-Object System.IO.Ports.SerialPort $serialPort,115200,None,8,one
$port.Open()
do {
  $line = $port.ReadLine()
  Write-Host $line
}
while ($port.IsOpen)
