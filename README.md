# Mini-tamagochi-keychain
Im designing a mini tamagochi keychain as a gift for my friend. It features a 0.96" OLED display and 3 buttons. The whole thing runs on a XIAO ESP32-C3 and is powered by a rechargable lipo cell.
Here's an initial drawing of it:

<img width="943" height="376" alt="tamagochi" src="https://github.com/user-attachments/assets/0f14ef24-b92e-4414-a56e-e1df522437b0" />


<img width="754" height="512" alt="Zrzut ekranu 2026-10-22 194450" src="https://github.com/user-attachments/assets/7f4ff7b4-50b8-4df3-9c52-2c355cff48d7" />
<img width="801" height="503" alt="Zrzut ekranu 2026-10-22 194512" src="https://github.com/user-attachments/assets/cf554e23-08a1-4ae0-b460-d0f4f81848c4" />

Link to onshape document - https://cad.onshape.com/documents/d71ea2f3e6966dbd3803b51b/w/8012156687731007183e445b/e/885404f0703f98faa2ecff5c?renderMode=0&uiState=6ada6cba8733704cbf182410

(more cad screenshots can be found in the "PHOTOS" folder)

## Firmware

The three buttons function as:

Button 1 - feed pet

Button 2 - wake up

Button 3 - play

The device sits mostly in deep sleep, to save battery, until the power button is pressed. upon startup, the oled screen displays the pet's face, it's hunger, energy and happines stat and animations (passive or while interacting).The user can then replenish hunger or happines by clicking the respective buttons (playing will be less effective if energy or hunger is low). Energy can be replenished by charging the device via usb.

Beware, that if hunger runs out, the pet dies and can be only respawned by manually resetting the device's flash.

Battery capacity is Battery 200mAh. should last a while thanks to the tamagochi being in deep sleep most of the time. I'll share the exact data once im able to test it.

## BOM
| # | Component | Price in $) | Link |
|---:|---|---:|---|
| 1 | ESP32-C3 | $6.41  | [AliExpress](https://pl.aliexpress.com/item/1005007367604089.html) |
| 2 | 3 Push buttons | $1.98 | [AliExpress](https://pl.aliexpress.com/item/1005009012038739.html) |
| 3 | SSD1315 OLED display | $2.92  | [AliExpress](https://pl.aliexpress.com/item/1005006262908701.html) |
| 4 | Battery 200mAh | $7.30 | [AliExpress](https://pl.aliexpress.com/item/1005011657290012.html) |

Total cost - $18.61
