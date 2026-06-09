# Emisar D3AA

This driver measures battery strength when power is connected, and uses that
info to set a hard limit on brightness when the battery is too weak to handle
full power.

The number of blinks at power-connect indicates the battery type and strength:

- 1 blink: Strong Li-ion cell, full power enabled

- 2 blinks: Strong AA cell, max AA power enabled
  (not used, strong AA uses mode 1 with full power enabled)

- 3 blinks: Weak battery, power severely limited

The 2-blink mode is not actually used.  So it should blink 1 time or 3 times,
depending on the cell strength.

