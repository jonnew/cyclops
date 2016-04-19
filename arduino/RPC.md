# Don't read (it's only for me)
# RPC

lossless media but packets might be lost due to overflows. Increase buffer size?
Packet Format: HEADER1 HEADER2 ARGS [DATA]
```
HEADER1 ...... .. seq channel
HEADER2 ... ..... flags command-id

HEADER ? .. ..... channel command-id
```

##for arduino

* Serial Object, binary reads, writes.
* Typical `Stream::ReadBytes(buff, count)` is highly blocking! Any timeout read is essentially blocking (`busy-wait`). Theses calls move data from the ring buffer to RAM.
* ISR blindly moves to ring buffer. Could be tinkered with... Serial is a critical interface for Cyclops.

\confirm ..... ... seq code
\request ..... ... seq request

##GUI sends

* Waveform Updates 1B + 4B!
	/phase /reset /pause /start /stop /resume /changePoint /updateData /switchWaveform
* `command` followed by `data`. This `data` stream should be interruptible, the GUI might create a simple command and it's implementation is clearly more important than the data transfer.
Since GUI interrupts data streams, 2 streams must not be interlaced.
**DMA cannot be used to move DATA if the stream is interruptible!**
```
HEADER . .. ..... sz
DATA
DATA
.
.
/Interrupt
/non-data-command
/non-data-command
/Resume
DATA
.
.
/end
\confirm
/any-command
.
.
```
Arduino keeps count of the stream being downloaded, makes sure it reads all points. It can send confirmation commands, helps in flow control and getting feedback.

> How would confirmations and ADC packets be interlaced?

## Waveform Object
## Task Object
## Process Queue
## Serial and RPC
## SPI interrupt and `Prepare DAC`