## ECE153B Project Page


### Group Members:
- Willy Tu
- Rick Franc

### Project Idea:
The idea of the project is to connected a XboxOne controller to LPC4088 micro-controller and have it perform some task. The full project proposal can be viewed in [here](https://github.com/wltu/ECE153B-Project/blob/master/ECE153B_ProjectPropoal.pdf "Project Proposal").

#### Week 1 (2/17 - 2/23):
The information on the Configuration and Interface descriptor for the XboxOne controller was looked into. The information was modified into the code in attempts to connect the controller to the micro-controller. The 8x8 LED matrix is also purchased. 

#### Week 2 (2/24 - 3/02):
After many days of research and testing, we have concluded that having communication between the micro-controller and the Xbox One Controller would be unfeasable. It seems that Xbox does not follow the HID standards for control transfer protocols as seen [here]( https://xboxdevwiki.net/Xbox_Input_Devices). The boot protocol would cause a STALL status in the USB communcation. Apparently this means that the endpoint cannot accept data according to [this site](https://www.silabs.com/community/mcu/8-bit/knowledge-base.entry.html/2017/06/18/the_role_of_stallha-pQTe). While we learned a lot about how write/read requests were made over USB, the data passed in a request for Xbox Input Device (XID) protocols from the first site were contradicting the general form of USB setup packets from [this explanation](https://www.beyondlogic.org/usbnutshell/usb6.shtml#SetupPacket). Using the protocols from the Xbox Dev Wiki would also give us a STALL status. We were unable to find any other helpful information about how to properly send XID requests and what information would be returned so we decided to use USB mouse input instead for the project.

Fortunately, USB mouse input went a lot smoother. We modified the protocol to the mouse's and changed the way the reports from the mouse were read to get button, X, and Y data. After some testing, we found the button data to be (in binary) 001 when the left button was pressed, 010 for the right button, and 100 for the middle button. When multiple buttons were pressed, these values would be ORed together.

#### Week 3 (3/03 - 3/09):
#### Week 4 (3/10 - 3/16):
#### Final Week:
