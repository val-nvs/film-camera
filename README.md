# DIY Film Camera

This repository documents the build of a homemade motion picture film camera. Mostly 3D printed and using off the shelf parts.

![image](files/v12/v12.gif)

## Features

- Pulls 35mm film from the end, ignoring perfs
- Records multiple tracks in a single film strip (2 to 4)
- Interchangeable gate (~Super16 to Super8 resolution)
- Choose any aspect ratio without wasting film
- Independent variable shutter
- Sony E mount for interchangeable lenses (18mm flange)
- Manual variable framerate (real time by logging time)
- Stills photo mode; long exposure mode

## Parts

- NEMA 8 stepper motor + driver
- 0530 6V Solenoid
- Seeed XIAO
- 2x RCR123A batteries with USB-C + BMS
- Body 3D print (lens mount & tripod thread)
- Core 3D print (houses the film & film motor)
- Shutter 3D print (lever/disc)
- Gate (ideally a thin material)
- OLED text display
- 3 buttons & 1 knob

## Status

- [x] Camera body designed in 3D  
- [x] Printed
- [x] Tested electornics
- [ ] Fix BMS, stepper needs higher voltage
- [ ] Assembled 
- [ ] Battery BMS
- [ ] Test roll

## Versions

### v14
- Nema 8 for film
- 0530 6V Solenoid for shutter
- Core completely encased to prevent light leaks
- 2x RCR123A

### v12 - printed
- Nema 14 for film 
- Nema 8 for shutter
- 71x72x67mm
- 2x RCR123A rechargeable batteries and holder https://www.takachi-enclosure.com/assets/attachments/images/CR123A-PC.pdf

### v11
- Nema 17 for film and shutter
- Uses separate motors for each function instead of complex gears. One motor spins the shutter disc, another advances the film.
![image](media/v11.gif)

## Files

- `/media` - Photos and videos of the camera
- `/files` - 3D models, schematics, code

## Notes

This is an ongoing project. The design prioritizes simplicity and compactness.

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

### @ 2 columns (12.42x6.989) (86.8mm²) (7.039mm pitch)
 38 exp: 17 sec
 100 ft: 6    min
 400 ft: 24   min
1000 ft: 60.1 min

### @ 3 columns (8.26x4.65) (38.4mm²) (4.700mm pitch)
- ` 38 exp`: 38.4 sec
- ` 100 ft`: 13.5  min
- ` 400 ft`: 54    min
- `1000 ft`: 135.1 min

### @ 4 columns (6.187x3.48) (21.5mm²) (3.530mm pitch)
-  38 exp: 68.1 sec
-  100 ft: 23.98 min
-  400 ft: 95.92 min
- 1000 ft: 239.8 min

- Super 16 @ 16:9 crop = 88.22 mm²
- 16mm     @ 16:9 crop = 59.24 mm²
- Super 8  @ 16:9 crop = 18.87 mm²
- 8mm      @ 16:9 crop = 11.39 mm²

