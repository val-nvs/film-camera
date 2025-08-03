# DIY Film Camera

This repository documents the build of a homemade motion picture film camera. Mostly 3D printed and using off the shelf parts.

![image](files/v12/v12.gif)

## Features

- 35mm film 
<sub>120 film mod possible for more area</sub>
- Sony E mount for interchangeable lenses
- Manual variable framerate <sub>for extended filming time</sub>
- Individual motors for each function <sub>simplified, no gears</sub>
- NEMA 8 stepper for shutter, NEMA 8/11/14/17 options for film advance
- 3x CR123A batteries with USB-C charging

## How it Works

Uses separate motors for each function instead of complex gears. One motor spins the shutter disc, another advances the film.

## Status

- [x] Camera body designed in 3D  
- [ ] Components sourcing  
- [ ] Electrical diagram created  
- [ ] Battery and BMS integration  
- [ ] Test roll

## Current Version: v12 

- Replaced cheaper Nema 17 motors for smaller and less powerful motors
- 2 CR123A rechargeable batteries and holder https://www.takachi-enclosure.com/assets/attachments/images/CR123A-PC.pdf

### v11

![image](media/v11.gif)

## Key Components

- 3D printed camera body and internal mechanisms
- NEMA 8/11 stepper motors for precision control
- Arduino or similar microcontroller for motor timing
- Custom PCB for power management and motor drivers
- Standard 35mm film cartridges

## Files

- `/media` - Photos and videos of the camera
- `/files` - 3D models, schematics, code

## Notes

This is an ongoing project. The design prioritizes simplicity and compactness. The 3D printed parts can be modified as needed.

## Inspiration

- Joshua Bird: https://joshuabird.com/blog/post/3d-printed-film-video-camera
- Blaž Semprimožnik: https://www.youtube.com/@BlazSemprimoznik
- Michael Reeves





# etc

38exp = 1444mm
100ft = 30480mm
24.9 width
16:9 ratio
0.05 gaps between
@ 24 fps

@ 2 columns (12.42x6.989) (86.8mm²) (7.039mm pitch)
 38 exp: 17 sec
 100 ft: 6    min
 400 ft: 24   min
1000 ft: 60.1 min

@ 3 columns (8.26x4.65) (38.4mm²) (4.700mm pitch)
 38 exp: 38.4 sec
 100 ft: 13.5  min
 400 ft: 54    min
1000 ft: 135.1 min

@ 4 columns (6.187x3.48) (21.5mm²) (3.530mm pitch)
 38 exp: 68.1 sec
 100 ft: 23.98 min
 400 ft: 95.92 min
1000 ft: 239.8 min

Super 16 @ 16:9 crop = 88.22 mm²
16mm     @ 16:9 crop = 59.24 mm²
Super 8  @ 16:9 crop = 18.87 mm²
8mm      @ 16:9 crop = 11.39 mm²

