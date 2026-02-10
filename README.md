# 3D Printed Clawless 35mm Film Camera

![image](media/v20-1.jpg)![image](media/v20-2.jpg)

This repository documents the build of a 3D printed film camera that is designed to use standard 35mm 36exp film cartridges. With the goal of shooting small clips on a budget. By **[Valdir Neves](https://www.instagram.com/val.nevs/)** & **[Charlie Chaves](https://www.instagram.com/charliechavess/)**.

[{{{ **View in 3D on onshape** }}}](https://cad.onshape.com/documents/5c2b9c47264034ac5c1a5ddc/w/cf712b211e6f3dfb560adac5/e/ec23c9e4d3ee5216bc6ef26c?renderMode=0&uiState=696ad2c3b048071179547c61)

## Acknowledgements

Thanks to **[Hans Boden](https://github.com/hans-boden)** for guidance and support throughout this project, including lending a 3D printer.

Thanks to [MILL - Makers In Little Lisbon](https://www.instagram.com/lxmill/) for providing a platform for connecting with other makers.

Thanks to [Elly T.](https://www.instagram.com/forfutureelly/) and [Daniel @photonbit](https://www.instagram.com/photonbit/) for design advice.

## Inspiration

- Joshua Bird https://joshuabird.com/blog/post/3d-printed-film-video-camera
	[![YouTube](https://img.shields.io/badge/-a?style=plastic&logo=youtube&color=f00)](https://www.youtube.com/@joshuayjbird)
	[![Github](https://img.shields.io/badge/-a?style=plastic&logo=github&color=purple)](https://github.com/jyjblrd/3D-Printed-Film-Video-Camera)
- Blaž Semprimožnik https://blazsemprimoznik.com/okto35-mk3
	[![YouTube](https://img.shields.io/badge/-a?style=plastic&logo=youtube&color=f00)](https://www.youtube.com/@BlazSemprimoznik)
	[![Instagram](https://img.shields.io/badge/-a?style=plastic&logo=instagram&color=orange)](https://www.instagram.com/blazsemprimoznik/)
- Brian's Lucky Box Camera
	[![YouTube](https://img.shields.io/badge/luckybox_camera-a?style=plastic&logo=&color=175AAA)](https://www.instagram.com/p/DKC4CXsJ28i/)
	[![Instagram](https://img.shields.io/badge/-a?style=plastic&logo=instagram&color=orange)](https://www.instagram.com/luckybox_camera/)
- henry kidman
	[![YouTube](https://img.shields.io/badge/3D%20printed%20MOVIE%20CAMERA%20from%20scratch-a?style=plastic&logo=youtube&color=175AAA)](https://www.youtube.com/watch?v=ZAtYJYfV2nA)
	[![YouTube](https://img.shields.io/badge/-a?style=plastic&logo=youtube&color=f00)](https://www.youtube.com/@henry_kidman)
	[![Instagram](https://img.shields.io/badge/-a?style=plastic&logo=instagram&color=orange)](https://www.instagram.com/henry_kidman/)

## About:

3D printed film camera, designed to use standard 35mm 36exp film. Capable of taking photos and making 1 to 4 short clips per roll.

Goals:

- To be a low cost-of-entry into analog motion picture with easily accesible film and labs. ~11€ film + ~15€ developing. (Super 8 starts at 23€+ B&W, color at 50€+, and it's harder to find somewhere to develop).

- To be low cost when bulk loading (when someone rolls their own 5ft rolls from a 100ft or 400ft spool). I'm using Lucky SHD50 B&W bulk loaded- 1.9€ per roll. Fomapan would be around 2.7€ per roll when bulk loaded.

Highlights:

- 2 Motors: Instead of a single motor with a system of gears to connect the shutter and pulldown mechanism, the mechanisms are independent of each other by using a motor for each.

- Clawless: Instead of claws to pulldown the film by the perforations, it simply winds the film into the camera with sticky tape. Possible due to the short total lenght of the film strip (150cm instead of 30m or 120m). Pros: simpler; frame can be any height/aspect ratio. Downside: almost impossible to project videos- must be scanned digitally.

- Multitrack video-recording: Possible to record up to 4 separate discrete tracks of video (not simultaneously) by shifting the internal assembly (and rewinding) after finishing each track. Comes with 4 options of gates to record between 1 to 4 tracks, with more options for taller aspect ratios like 1:1.

- Maximum FPS for video: ???

- Records 1600 frames per canister with the smallest gate (divided into 4 passes) (2min@13fps). Or 820 frames using a "Super 16" sized gate (divided into 2 passes), at 24fps it would be 34s of total recording time- 45s@18fps 1min@14fps.

Features:

- E-mount used, easy to adapt to many lenses And it's easier to print compared to C-mount- with it's fine threads.

- With the smallest "Super 8" gate you can film up to 1200 frames per cartridge (divided into 4 passes).

Future plans:

- OLED screen tracks how much film you used.
- Unique manual variable FPS feature designed to [save on film] or [add slow motion] mid-shoot. (Each frame will be logged in time making it easy to sync frames and sound).


## 3D model
[{{{ **View in 3D on onshape** }}}](https://cad.onshape.com/documents/5c2b9c47264034ac5c1a5ddc/w/cf712b211e6f3dfb560adac5/e/ec23c9e4d3ee5216bc6ef26c?renderMode=0&uiState=696ad2c3b048071179547c61)

## Code
[Wokwi](https://wokwi.com/projects/455248968650449921)

## Setup

- Onshape CAD software on Chrome
- UltiMaker Cura 5.11.0 Slicer
- Anycubic i3 Mega S Printer / 0.4mm nozzle / SMARTFIL 1.75mm Black PLA

## Parts

Motors:

- 1x Nema 8 Stepper Motor for the shutter ![Static Badge](https://img.shields.io/badge/15%E2%82%AC-icon?color=purple)
- 1x Nema 14 Stepper Motor to pull film ![Static Badge](https://img.shields.io/badge/5%E2%82%AC-icon?color=purple)

Other parts:

- 2x Stepper Driver DRV8825 ![Static Badge](https://img.shields.io/badge/3pcs-4.5%E2%82%AC-icon?color=purple)
- Arduino and breadboard ~15€
- 2x Tripod threaded insert (1/4") length 6mm ![Static Badge](https://img.shields.io/badge/10pcs-2%E2%82%AC-icon?color=purple)
- Optional: 8x 5x3mm round magnets ![Static Badge](https://img.shields.io/badge/50pcs-3.4%E2%82%AC-icon?color=purple)
- 4x Buttons: ![Static Badge](https://img.shields.io/badge/4pcs-5.7%E2%82%AC-icon?color=purple)

Viewfinder:

- Acrylic lens 24.8mm diameter ![Static Badge](https://img.shields.io/badge/2.28%E2%82%AC-icon?color=purple)
- Front surface mirror 30x30x2mm ![Static Badge](https://img.shields.io/badge/1.66%E2%82%AC-icon?color=purple)
- Plastic Fresnel lens f=28mm 30x30mm ![Static Badge](https://img.shields.io/badge/2.92%E2%82%AC-icon?color=purple)

Lens:

- Lens mount adaptor C-NEX (to support C-mount lenses) ![Static Badge](https://img.shields.io/badge/2.74%E2%82%AC-icon?color=purple)
- Lens: 3.6-10mm f/1.4 C-Mount [1/1.8"] "8MP IR CCTV" (effectively f/2) ![Static Badge](https://img.shields.io/badge/19%E2%82%AC-icon?color=purple)
- Optional: Imaging lens cover ![Static Badge](https://img.shields.io/badge/9.7%E2%82%AC-icon?color=purple)

## 3D Printed parts (v21)

- Body
- Core
- Left and right wall
- Shutter disc
- Rewinder pin
- Optional: 2 gears and 2 locking pins

## History

Feb 10 - test failed, not enough force to pull film consistently

Feb 9 - test failed, solder fail

![image](media/v20.gif)


v21 - TBA
```
- smaller body, eletronics outside
- integrated viewfinder into body
```

v20b - Jan 19, 2026
```
- shave off parts of gate and oled lid
- added lock pin
- supressed NPF wiring ducts
- fix shutter piece
- fix roller size
- rotate mount slightly
- widen button holes
```
v20a - Jan 9, 2026
```
- added support for roller on core
```
v20 - Jan 1, 2026
```
- FDM focused redesign
- better gears
- add 4 magnets on each wall
```

v19 - Nov 25, 2025
```
v19a: added switches - Nov 26, 2025
```
![image](media/v19.gif)

v18
```
- Not symmetric
- Custom buttons
```

v17

![image](media/gear.gif)

```
- Gear to move core
- 2x RCR123A
- OLED
```

v16
```
- Protuding shutter design thing
```

v15
```
- Fixed wrong flange value!
- Core completely encased to prevent light leaks
- Core on bottom
- Nema 8 for film
- Nema 14 for shutter
- Cold Shoe
```

v14
```
- Nema 8 for film
- 0530 6V Solenoid for shutter
- 2x RCR123A
```

v13
```
- Tests with solenoid for shutter and shifted reel motor
```

v12 - PRINTED @JLC3DP - July 31, 2025

![image](media/v12-1.jpg)
![image](media/v12-2.jpg)
![image](media/v12-3.jpg)
![image](media/v12-4.jpg)
```
- Wrong flange value!
- Nema 14 for film 
- Nema 8 for shutter
- 71x72x67mm
- 2x RCR123A batteries and holder https://www.takachi-enclosure.com/assets/attachments/images/CR123A-PC.pdf
```
![image](media/v12.gif)

v11
```
- 4x RCR123A batteries
```

v10
```
?
```

v9

![image](media/v9.gif)
```
- Uses separate motors for each function instead of complex gears. One motor spins the shutter disc, another advances the film.
- Nema 17 for film and shutter
- 4x AAAA batteries
```

# Film scanner
![image](media/scan1.jpg)
![image](media/scan2.jpg)
![image](media/scan3.jpg)

to photo scam 35mm 8 perforations film
using 7artisans 35mm f/.95 and a small video light (NEEWER RGB61 iirc)

- v3 printed but film gets jammed
- v6 works great, 2 identical pieces that connect each-other for the base, and one tube piece to hold the lens. easy to adapt other lenses with .step file
- v7 will add phone holder to use the flashlight to see dust particles on top of the film like the laser dyson vacuum

# etc

```
38exp = 1444mm
100ft = 30480mm
24.9 width
16:9 ratio
0.05 gaps between
@ 24 fps
```

```
@ 2 columns (12.42x6.989) (86.8mm²) (7.039mm pitch)
 38 exp: 17 sec
 100 ft: 6    min
 400 ft: 24   min
1000 ft: 60.1 min
```

```
@ 3 columns (8.26x4.65) (38.4mm²) (4.700mm pitch)
- ` 38 exp`: 38.4 sec
- ` 100 ft`: 13.5  min
- ` 400 ft`: 54    min
- `1000 ft`: 135.1 min
```

```
@ 4 columns (6.187x3.48) (21.5mm²) (3.530mm pitch)
-  38 exp: 68.1 sec
-  100 ft: 23.98 min
-  400 ft: 95.92 min
- 1000 ft: 239.8 min
```

```
- Super 16 @ 16:9 crop = 88.22 mm²
- 16mm     @ 16:9 crop = 59.24 mm²
- Super 8  @ 16:9 crop = 18.87 mm²
- 8mm      @ 16:9 crop = 11.39 mm²
```

# etc
Tripod threaded insert (1/4") length 6mm

![image](media/tripod.jpg)

100ft of film makes 20 "36exp" cartridges

![image](media/shd50.jpg)

6.3-10mm f/1.4 C-mount lens

![image](media/lens.jpg)
