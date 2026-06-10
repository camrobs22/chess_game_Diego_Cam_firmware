# chess_firmware merge notes

This merged firmware uses Northwestern's registered-device Wi-Fi:

- SSID: Device-Northwestern (open network; access is authorized by MAC address)
- Server IP/URL: 18.221.166.110:3000

Main merged fixes:

- Keeps the patched invalid-move recovery logic so a rejected physical move no longer gets resent forever. The firmware waits for the physical board to be restored before continuing.
- Keeps the patched state-count fix so invalid captures do not permanently corrupt expected piece counts.
- Keeps capture, invalid-move, win, loss, and draw LED animations.
- Defers game-over animations until the right physical-board moment. If the opponent/engine sends a checkmating move, the loss animation is stored as pending and only plays after the human physically completes that highlighted opponent move on the board.
- Keeps serial output shorter by avoiding full JSON payload prints.

Build normally with PlatformIO from this folder.

## Checkmate animation timing follow-up

A second fix was added for the case where the server sends a `game:over` / `game:state` result before the matching opponent `move:accepted` message. In that case, the firmware now marks the game result as waiting for an opponent move and will not play the win/loss/draw animation until that opponent move has been physically completed on the board.

The handler also now tries to recover the final opponent move from `acceptedMove`, `lastMove`, or root `move` fields in a game-over message.
