## ECE153B Project Page


### Group Members:
- Willy Tu
- Rick Franc

### Project Idea:
The idea of the project is to connected a XboxOne controller to LPC4088 micro-controller and have it perform some task. The full project proposal can be viewed in [here](https://github.com/wltu/ECE153B-Project/blob/master/ECE153B_ProjectPropoal.pdf "Project Proposal").

#### Week 1 (2/17 - 2/23):
The information on the Configuration and Interface descriptor for the XboxOne controller was looked into. The information was modified into the code in attempts to connect the controller to the micro-controller. The 8x8 LED matrix is also purchased. 

#### Week 2 (2/24 - 3/02):
After many days of research and testing, we have concluded that having communication between the micro-controller and the Xbox One Controller would be unfeasable. It seems that Xbox does not follow the HID standards for control transfer protocols as seen [here]( https://xboxdevwiki.net/Xbox_Input_Devices). The boot protocol would cause a STALL status in the USB communcation. Apparently this means that the endpoint cannot accept data according to [this site](https://www.silabs.com/community/mcu/8-bit/knowledge-base.entry.html/2017/06/18/the_role_of_stallha-pQTe). While we learned a lot about how write/read requests were made over USB, the data passed in a request for Xbox Input Device (XID) protocols from the first site were contradicting the general form of USB setup packets from this [explanation](https://www.beyondlogic.org/usbnutshell/usb6.shtml#SetupPacket). Using the protocols from the Xbox Dev Wiki would also give us a STALL status. We were unable to find any other helpful information about how to properly send XID requests and what information would be returned so we decided to use USB mouse input instead for the project.

Fortunately, USB mouse input went a lot smoother. We modified the protocol to the mouse's and changed the way the reports from the mouse were read to get button, X, and Y data. After some testing, we found the button data to be (in binary) 001 when the left button was pressed, 010 for the right button, and 100 for the middle button. When multiple buttons were pressed, these values would be ORed together.

#### Week 3 (3/03 - 3/09):
This week we were successful in setting up the 8x8 LED screen using GPIO pins. The LED screen's datasheet can be found [here](https://cdn-shop.adafruit.com/datasheets/454datasheet.pdf) with information of the pin layout. The LPC GPIO pin to LED sceen pin connections as of right now can be found [here] (https://github.com/wltu/ECE153B-Project/blob/master/Pin.txt). It appears that the bottom few rows and one of the column have issues turning on on the hardware side of the part itself so we will be unable to use that part of the screen. To turn on the rest of the leds, we quickly rotate turning on each row and then turn on the respective column so that all the LEDs we want on are on and none of the LEDs we want off are on. The downside to this strategy is that the individual LEDs will appear less bright as they are only on for a fraction of the time but since the rate is so fast, all the lit LEDs appear to always be on to the human eye. A image of a smile to test the functionility of the LEDs is shown below. We have also merged this part of the project with the USB mouse host. At first, the idea is to use the LED screen to display the direction of the mouse movement, however, due to the communication between the mouse/UART/LPC halts the program and prevent the LED to scan across the rows. This result in noticeable each row turning on one at a time, instead of all on to the human eyes. Therefore, ewe scrap the directional arrow idea and just have the lit LED corresponds to the position of the mouse.

<p align="center">
  <img width="300" height="300" src="https://user-images.githubusercontent.com/27719918/54096806-a1b37d80-436a-11e9-88ae-c911a272d56b.PNG">
</p>

We have also began work to create a sound from the speaker when a mouse button is pressed. There is a sample project called periph_i2s that sends audio data from a computer through an audio cable to the LPC's speaker which we plan to mirror in order to create noise. One problem with this strategy is that we wish to create our own audio data rather than pulling it from a computer so we will have to understand how the data is formatted.  

#### Week 4 (3/10 - 3/16):
#### Final Week:
