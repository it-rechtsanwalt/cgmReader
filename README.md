# cgmReader

Software to read the CBG values from a Medtronic MiniMed 640G pump.
--- DISCLAIMER: I am not affiliated with Medtronic or any other company. This is just a private project. I do not guarantee that the shown values are correct. DO NOT TRUST THE SHOWN VALUES ---
## Overview

### what is this

## HowTo:

## Q&A: 

### why am I doing this?

**Because I can!**

### honestly: why am I doing this?

honestly: because i can!

### for real?

yes. AND: my wife got diabetes type I. if i want to check her values a night, I need to wake her up to grab the pump. Sooo: I wanted a system to check the values "over the air". 

### why c++? there are other implenetations with python

1. I do not speak python
2. I wanted a "fun" project in C++
3. I wanted a *simple* installation method (so i want to provide an dpkg-image for raspbian OR an debian package - i'm working on that)

### what is the goal of the project:

1. [x] read the bg-values over the air via an raspberry pi (thanks for this pretty little peace of hardware!) (yet working!)
2. [ ] show the bg-values on a display, attached on the raspberry pi (not yet working correctly)
3. [x] read the bg-history over the air (working!)
4. [ ] show the bg-history on a display, attached on the raspberry pi (not yet working correctly)
5. [ ] provide the bg-values and the status to a [home-assistant instance](https://www.home-assistant.io/hassio/)
  - (switch the light on and wake me up, when there is an critical situation)
  - remind me to  
    - change the insulin
    - change the catheter
    - change the battery
    - change the sensor
    - ...

6. [ ] provide the history to an [home-assistant](https://www.home-assistant.io/hassio/)
7. [ ] provide a dpkg-package.

### do i need help?

YES. as you can see

1. my english is BAD.
2. my C++ is BAD.

so help is welcome!

## THANKS TO

- @pazaan for his grate work - see: (https://github.com/pazaan/decoding-contour-next-link)
- @jkirk - you where very supportive!
