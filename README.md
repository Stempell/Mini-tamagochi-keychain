# Mini-tamagochi-keychain
Im designing a mini tamagochi keychain as a gift for my friend. It features a 0.96" OLED display and 3 buttons. The whole thing runs on a XIAO ESP32-C3 and is powered by a rechargable lipo cell.
Here's an initial drawing of it:

<img width="943" height="376" alt="tamagochi" src="https://github.com/user-attachments/assets/0f14ef24-b92e-4414-a56e-e1df522437b0" />


<img width="754" height="512" alt="Zrzut ekranu 2026-10-22 194450" src="https://github.com/user-attachments/assets/7f4ff7b4-50b8-4df3-9c52-2c355cff48d7" />
<img width="801" height="503" alt="Zrzut ekranu 2026-10-22 194512" src="https://github.com/user-attachments/assets/cf554e23-08a1-4ae0-b460-d0f4f81848c4" />
(more cad screenshots can be found in the "PHOTOS" folder)

##Firmware
The three buttons function as:
Button 1 - feed pet
Button 2 - wake up
BUtton 3 - play
The device sits mostly in deep sleep, to save battery, until the power button is pressed. upon startup, the oled screen displays the pet's face, it's hunger, energy and happines stat and animations (passive or while interacting).The user can then replenish hunger or happines by clicking the respective buttons (playing will be less effective if energy or hunger is low). Energy can be replenished by charging the device via usb.
Beware, that if hunger runs out, the pet dies and can be only respawned by manually resetting the device's flash.
Battery capacity is Battery 200mAh. should last a while thanks to the tamagochi being in deep sleep most of the time. I'll share the exact data once im able to test it.

## BOM
| # | Component | Price in $) | Link |
|---:|---|---:|---|
| 1 | ESP32-C3 | $6.28  | [AliExpress](https://pl.aliexpress.com/item/1005013335912855) |
| 2 | Buttons | $2.73  | [AliExpress](https://pl.aliexpress.com/item/1005012637969465) |
| 3 | SSD1315 OLED display | $2.38  | [AliExpress](https://pl.aliexpress.com/item/4001028369082) |
| 4 | Battery 200mAh | $8.45 | [AliExpress](https://pl.aliexpress.com/item/1005013092520018.html?spm=a2g0o.productlist.main.26.33681aa5y0bQKN&aem_p4p_detail=2026101807565439518473057620004515941&algo_pvid=91238635-de95-4b51-82c5-e0c7e30d7bda&algo_exp_id=91238635-de95-4b51-82c5-e0c7e30d7bda-23&pdp_ext_f=%7B%22order%22%3A%2249%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%218.28%218.28%21%21%2155.12%2155.11%21%400b0fe0e117923354144838788e0ce0%2112000060251468284%21sea%21PL%216850984236%21X%211%210%21n_tag%3A-29919%3Bd%3A9d0c4368%3Bm03_new_user%3A-29895&curPageLogUid=fz5Jf5ADed6p&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005013092520018%7C_p_origin_prod%3A&search_p4p_id=2026101807565439518473057620004515941_6) |

Total cost - $11.39
