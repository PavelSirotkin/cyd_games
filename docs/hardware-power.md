# CYD Hardware: Power & Connections

This doc covers powering the ESP32-2432S028 ("CYD") via the rear **S1 / S3** solder pads as an alternative to the USB port — useful for a permanent install, an internal LiPo, or wiring into a project enclosure.

> **Verify before wiring.** Pad labels and polarity vary across CYD revisions. Probe `S1` against a known `GND` with a multimeter (continuity to the USB shield) before connecting a power source. Reverse polarity will destroy the board.

## Board orientation (rear view)

The diagram below is the **back** of the CYD — the side with the ESP32 module, the USB port at the bottom, and the three JST connectors. The display itself is on the opposite side.

```
       ┌──────────────────────────────────────────────────┐
       │                                                  │
       │       ┌────────────────────────────────┐         │
       │       │                                │         │
       │       │           ESP32-WROOM          │         │
       │       │            (shield)            │         │
       │       │                                │         │
       │       └────────────────────────────────┘         │
       │                                                  │
       │   ┌────┐                                ┌────┐   │
       │   │ P1 │   ┌───┐ ┌───┐ ┌───┐            │ P3 │   │
       │   │JST │   │S1 │ │S2 │ │S3 │            │JST │   │
       │   │    │   │ + │ │ • │ │ − │            │    │   │
       │   └────┘   └─┬─┘ └───┘ └─┬─┘            └────┘   │
       │              │           │                       │
       │              │           │                       │
       │           ┌──┴───────────┴──┐                    │
       │           │     CN1 JST     │                    │
       │           └─────────────────┘                    │
       │                                                  │
       │              ┌─────────────┐                     │
       │              │   USB-C /   │                     │
       │              │   micro-B   │                     │
       │              └─────────────┘                     │
       └──────────────────────────────────────────────────┘
                              ▲
                              │  (cable exits here)
```

## Power wiring (S1 / S3)

```
   external power               CYD rear pads
   ┌──────────────┐
   │              │
   │   +5V  ●─────┼────────────►  S1   (V+, ~5 V rail to USB)
   │              │
   │   GND  ●─────┼────────────►  S3   (GND)
   │              │
   └──────────────┘

   S2 is the middle pad (often unused or routed
   elsewhere depending on revision — leave open).
```

**Notes**

- `S1` ties into the same rail the USB port delivers, so a regulated **5 V** source (USB charger, buck converter, BMS output) is the safe target. The on-board AMS1117 then derives 3.3 V for the ESP32 and display.
- Do **not** feed 3.3 V into `S1` — the regulator's dropout means the ESP32 will brown out under load.
- Powering via `S1`/`S3` and the USB port simultaneously is generally fine (USB has back-feed protection on most CYD revs), but unplug the bench supply before connecting USB to a PC just to be safe.

## Other connectors (reference)

| Connector | Common pinout (verify per board) | Typical use |
|-----------|----------------------------------|-------------|
| P1 (JST 1.25mm 4-pin) | GND, GPIO22, GPIO27, 3V3 | I²C / extension |
| P3 (JST 1.25mm 4-pin) | GND, GPIO35, GPIO22, 3V3 | Sensor input |
| CN1 (JST 1.25mm 4-pin) | GND, TX, RX, 3V3 | UART / programming |
| USB | 5 V in, USB serial via CH340 | Flashing + power |

For the GPIO pins this firmware actually uses (display, touch, RGB LED, speaker, LDR), see [`include/config.h`](../include/config.h).
