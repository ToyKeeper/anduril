# Anduril Whezzel Firmware Build & Flash Guide

## Build Firmware
Be in the firmware directory (`~/repos/anduril`):

```
make clean
make whezzel
## Optional (specific firmware)
make whezzel blf-gt
```

## Flash Firmware
```
./bin/flash-<MCU>.sh ./hex/<HEX_FILE>.hex
```
(Replace `<MCU>` and `<HEX_FILE>` as needed, e.g., `./bin/flash-attiny25.sh ./hex/anduril.whezzel-blf-gt.hex`)

## Flashlight Specifications
(Sorted by brand; MCU noted where known. All variants listed on separate lines.)

|   Brand   |        Model         |         Driver          |    MCU     |          LED          |                  Firmware File                  |      Notes      |
|-----------|----------------------|-------------------------|------------|-----------------------|-------------------------------------------------|-----------------|
| BLF       | GT                   | Stock                   | attiny85   | XHP35 HI 5700K        | anduril.whezzel-blf-gt.hex                      |                 |
| BLF       | LT1                  | Stock                   | attiny85   | LH351D 2700K/5000K    | anduril.whezzel-blf-lt1.hex                     |                 |
| Convoy    | S2+                  | MTN-17DDm               | attiny25   | -                     | -                                               | Current: Bistro |
| Emisar    | D1k                  | Linear                  | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-emisar-d1k-linear-nofet.hex     | FET disabled    |
| Emisar    | D3aa                 | "TheFreeman" 9v6a Boost | avr32dd20  | 519A 4000k de-domed   | anduril.whezzel-emisar-d3aa.hex                 |                 |
| Emisar    | D3aa Brass           | "TheFreeman" 9v6a Boost | avr32dd20  | 519A 4500k de-domed   | anduril.whezzel-emisar-d3aa.hex                 |                 |
| Emisar    | D3aa Copper          | "TheFreeman" 9v6a Boost | avr32dd20  | 519A 4500k de-domed   | anduril.whezzel-emisar-d3aa.hex                 |                 |
| Emisar    | D4K                  | Linear                  | attiny1634 | 5w UV 365nm           | anduril.whezzel-emisar-d4k-linear-nofet.hex     | FET disabled    |
| Emisar    | D4K Copper           | Lume X1                 | avr32dd20  | 519A 4000k de-domed   | anduril.whezzel-emisar-d4k-lume-x1.hex          |                 |
| Emisar    | D4sv2                | DD + FET                | attiny1634 | SST-20 660nm Deep Red | anduril.whezzel-emisar-d4sv2-low.hex            |                 |
| Emisar    | D4sv2                | DD + FET                | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-emisar-d4sv2.hex                |                 |
| Emisar    | D4v2 Copper          | DM11 12v Boost          | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-emisar-d4v2-boost.hex           |                 |
| Emisar    | D4v2 Ti              | DM11 12v Boost          | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-emisar-d4v2-boost.hex           |                 |
| Emisar    | D4v2 Brass           | DD + FET                | attiny1634 | ?                     | anduril.whezzel-emisar-d4v2.hex                 |                 |
| Emisar    | D18                  | DD + FET                | attiny85   | 519A 3500k de-domed   | anduril.whezzel-emisar-d18.hex                  |                 |
| Emisar    | DW4                  | DM11 12v Boost          | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-emisar-dw4-boost.hex            |                 |
| Lumintop  | FW3A                 | Lume1                   | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-lumintop-fw3x-lume1-rgbswap.hex |                 |
| Lumintop  | FW3A Brass           | Lume1                   | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-lumintop-fw3x-lume1-rgbswap.hex |                 |
| Lumintop  | FW3A Copper          | Lume1                   | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-lumintop-fw3x-lume1-rgbswap.hex |                 |
| Lumintop  | FW3A Stainless Steel | FET + DD                | attiny1634 | 519A 3500k de-domed   | anduril.whezzel-lumintop-fw3x-lume1-rgbswap.hex |                 |
| Lumintop  | FWAA                 | Stock                   | attiny85   | 519A 3500k de-domed   | anduril.whezzel-lumintop-fwaa.hex               |                 |
| Lumintop  | FWAA Copper          | Stock                   | attiny85   | 519A 3500k de-domed   | anduril.whezzel-lumintop-fwaa.hex               |                 |
| Lumintop  | GT Micro             | Stock                   | attiny85   | Osram KW CSLNM1.1     | anduril.whezzel-lumintop-gt-micro.hex           |                 |
| Noctigon  | KR4 Copper           | Lume X1                 | avr32dd20  | 519A 4000k de-domed   | anduril.whezzel-noctigon-kr4-lume-x1.hex        |                 |
| Wildtrail | WT90                 | Stock                   | attiny85   | SBT90.2 5700k         | anduril.whezzel-wildtrail-wt90.hex              |                 |
