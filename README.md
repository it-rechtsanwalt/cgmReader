# cgmReader

Software to read the CBG values from a Medtronic MiniMed 640G pump.


**DISCLAIMER: I am not affiliated with Medtronic or any other company. This is just a private project. I do not guarantee that the shown values are correct. I do not guarantee that you break the pump or the reader! DO NOT TRUST THE SHOWN VALUES!!**

**READ THE KNOWN ISSUES BEFORE YOU START**


## Overview

### what is this

This software is intendet to use with a Medronic MiniMed 640G pump. If you do not know what this is, you do not need this software!

With this little peace of software you are able the read the CGM status from a 640g pump on any linux machine (especially on a Raspberry Pi with raspbian). The values are shown in a console. In a next step the values will be readable via an JSON object.

The software is just testet in linux (it will not run in windows, because the lib-usb library needs to be changed...)

**It runs under Raspbian**

### DOES IS WORK?

yes, but the software is in a alpha state. I use the system right now!

## Known Issues / missing features

- no commandline interpretation
- no startup script for linux
- random crashes after hours of perfect work 
- BUGS!
- too many "fixme" comments in the software

## HowTo:

Prerequisites:

Hardware:

- Linux Machine 
- Ascensia Contour Next Link 2.4 Stick (see: https://www.diabetes.ascensia.at/produkte/contour-next-link-2-4/) 
- MiniMed 640G Insulin Pump (see: https://www.medtronic-diabetes.at/minimed-produkte/minimed-640g-insulinpumpe)


Software:

- The Contour Next Link 2.4 and the MiniMed pump need to be connected (linked) with each other. 

on the linux side:

1. install raspbian
2. install dependencies:
```
sudo aptitude install libcrypto++-dev  libhidapi-libusb0 libhidapi-dev libusb-1.0-0-dev liblzo2-dev libusb-dev
```

3. get the software

** TODO **

4. compile 

** TODO ** 

5. create startup script

** TODO **

6. grant permission (so no root is required):

```
sudo nano /etc/udev/rules.d/50-usb-bayer.rules
```

add:

```
SUBSYSTEM=="usb", ATTRS{idVendor}=="1a79", ATTR{idProduct}=="6210", MODE="0666"
```
no reboot needed

### home assistant?

yes: it works! 

add this to your sensors section (im my case: sensors.yaml) - don't forget to replace the ip an port with your cgmReader ip and port:


```
  - platform: rest
    name: cgmreader
    hidden: true
    resource: http://192.168.12.170:8888/ID=1234
    scan_interval: 20 
    json_attributes:
      - id
      - sensorBGL
      - readStatus
      - activeInsulin
      - currentBasalRate
      - batteryLevelPercentage
      - insulinUnitsRemaining
      - sensorBattery
      - timeToCalibrate
      - trend
      - message

  - platform: template
    sensors:
      cgm_stickserial:
        value_template: '{{ states.sensor.cgmreader.attributes.id }}' 
        friendly_name: 'Stick serial number'
        icon_template: mdi:identifier
      cgm_sensor_bgl:
        value_template: '{{ states.sensor.cgmreader.attributes.sensorBGL  | int }}' 
        friendly_name: 'Sensor BGL'
        unit_of_measurement: 'mg/dl'
        icon_template: mdi:home-heart
      cgm_trend:
        value_template: '{{ states.sensor.cgmreader.attributes.trend  }}' 
        friendly_name: 'Trend'
        icon_template: mdi:trending-up
      cgm_readstatus:
        value_template: '{{ states.sensor.cgmreader.attributes.readStatus  | int }}' 
        friendly_name: 'Read status'
      cgm_current_basal_rate:
        value_template: '{{ states.sensor.cgmreader.attributes.currentBasalRate  }}' 
        friendly_name: 'Current basal rate'
      cgm_active_insulin:
        value_template: '{{ states.sensor.cgmreader.attributes.activeInsulin | round(2) }}' 
        friendly_name: 'Active insulin'
        unit_of_measurement: 'U'
        icon_template: mdi:trending-down
      cgm_pump_battery_level:
        value_template: '{{ states.sensor.cgmreader.attributes.batteryLevelPercentage  }}' 
        friendly_name: 'Pump battery level'
        unit_of_measurement: '%'
        icon_template: mdi:battery
      cgm_insulin_units_remaining:
        value_template: '{{ states.sensor.cgmreader.attributes.insulinUnitsRemaining  }}' 
        friendly_name: 'Insulin units remaining'
        unit_of_measurement: 'U'
        icon_template: mdi:history
      cgm_sensor_battery:
        value_template: '{{ states.sensor.cgmreader.attributes.sensorBattery  }}' 
        friendly_name: 'Sensor battery '
        unit_of_measurement: '%'
        icon_template: mdi:battery-charging-wireless-outline
      cgm_timetocalibrate:
        value_template: '{{ states.sensor.cgmreader.attributes.timeToCalibrate  }}' 
        friendly_name: 'Time to next calibration'
        unit_of_measurement: 'min'
        icon_template: mdi:clock-end
      cgm_message:
        value_template: '{{ states.sensor.cgmreader.attributes.message  }}' 
        friendly_name: 'Pump message'    
        icon_template: mdi:message
```

**HINT:**
"ID" is the id you defined in MainParameters.h as the "JsonServerKey". (don't forget: it is hardcoded!)

in the lovelace UI I use the following card:

```
entities:
  - entity: sensor.cgm_sensor_bgl
  - entity: sensor.cgm_trend
  - entity: sensor.cgm_active_insulin
  - entity: sensor.cgm_message
  - entity: sensor.cgm_timetocalibrate
  - entity: sensor.cgm_insulin_units_remaining
  - entity: sensor.cgm_pump_battery_level
  - entity: sensor.cgm_sensor_battery
show_header_toggle: false
title: Blutzucker
type: entities
```

No automation defined right now...

## Q&A: 

### Why am I doing this?

**Because I can!**

### Honestly: Why am I doing this?

Honestly: Because I can!

### for real?

Yes. AND: My wife got diabetes type I. If I want to check her values a night, I need to wake her up to grab the pump. Sooooo: I wanted a system to check the values "over the air". 

### Why c++? there are other implenetations with python

1. I do not speak python
2. I wanted a "fun" project in C++
3. I wanted a *simple* installation method (so i want to provide an dpkg-image for raspbian OR an debian package - i'm working on that)

### what is the goal of the project:

1. [x] read the bg-values over the air via an raspberry pi (thanks for this pretty little peace of hardware!) (yet working!)
2. [ ] show the bg-values on a display, attached on the raspberry pi (not yet working correctly)
3. [x] read the bg-history over the air (working!)
4. [ ] show the bg-history on a display, attached on the raspberry pi (not yet working correctly)
5. [x] provide the bg-values and the status to a [home-assistant instance](https://www.home-assistant.io/hassio/)
  - (switch the light on and wake me up, when there is an critical situation) - (not working right now)
  - remind me to  
    - change the insulin - (workin)
    - change the catheter - (not working right now)
    - change the battery - (workin)
    - change the sensor - (not working right now)
    - ...

6. [ ] provide the history to an [home-assistant](https://www.home-assistant.io/hassio/) (maybe not possible)
7. [ ] provide a dpkg-package.

### do i need help?

YES. as you can see

1. my english is BAD.
2. my C++ is BAD.

so help is welcome!

## known issues

### what if the stick "hangs"?

The sticks are crap and obviously not build to do a permanent reading. so: sometimes they hang. first of all: no panic. if the stick does not respond (i had this issue in two years only two times, when the software got stuck while reading): disconnect the stick from the computer. wait a few minutes! press (and keep pressed) the "on/off" button and the lower two buttons for several seconds. if it is not working in the first place, try it again (trust me: is works).

### what if i get permanent crc-errors?

I testest the software with four different sticks. the oldest stick had an strange behavour: i got very often a crc-error an incorrect values. i could not find out why - maybe the stick "got old"? my solution: i bought a new one...


## THANKS TO

- @pazaan for his grate work - see: (https://github.com/pazaan/decoding-contour-next-link)
- @jkirk - you where very supportive!
- @zerok - you where also very supportive!
