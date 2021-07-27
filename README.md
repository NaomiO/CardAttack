# EMV Credit Card Attack With Nfc 

![Emv Card Example](https://blog.payjunction.com/hubfs/BLOG/FEATURED%20IMAGES/emv-card-reader.svg)

This is an educational project as a part of my studies of computer sciences at the University of Ariel of the course: **Network Protection**.
This project aims to demonstrate the security failures on cret cards using **EMV technology**.
[**EMV**- Europay Mastercard Visa](https://en.wikipedia.org/wiki/EMV) -> Click on this link to read about the technology. 



##  Europay Mastercard Visa

EMV is a payment method based upon a technical standard for smart payment cards and for payment terminals and automated teller machines which can accept them. EMV originally stood for "Europay, Mastercard, and Visa", the three companies which created the standard.

EMV cards are smart cards, also called chip cards, integrated circuit cards, or IC cards which store their data on integrated circuit chips, in addition to magnetic stripes for backward compatibility. These include cards that must be physically inserted or "dipped" into a reader, as well as contactless cards that can be read over a short distance using near-field communication technology.

###  Type of the Attack

[**Man in the middle**](https://en.wikipedia.org/wiki/Man-in-the-middle_attack)

EMV is the international protocol standard for smartcard payment and is used in over 9 billion cards worldwide. Despite the standard’s advertised security, various issues have been previously uncovered, deriving from logical flaws that are hard to spot in EMV’s lengthy and complex specification, running over 2,000 pages.


### Explain Attack 
The purpose of this project is to read the banking information of the credit card: Card number, Expiration, Last payments
The principle is the following ->  The Emv card using contactless, uses the nfc.

By reading the card with an nfc reader we get a code in hexadecimal form.

> For example: 6F1A840E315041592E5359532E4444463031A5088801025F2D02656E'

As this code is part of the ISO standardization, we know its working principles (encapsulation) 

So we have to cut the hexadecimal code received and we get the information we want.
The principle is a bit the same as http where we have a header and a body.
 The header announces where the information is located.
 ![Nfc Record](https://images3.programmersought.com/680/4d/4da995268cfebfb0ba4f8cc91a93aa48.png)

###  ISO 
> [IEC 14443 -> Contactless cards](https://en.wikipedia.org/wiki/ISO/IEC_14443)

### Requirements 

 - [x] Linux (Ubuntu)
 - [x] g++ (C++ 20)
 - [x] (GCC) 5.2.0
 - [x] [PC/SC](https://fr.wikipedia.org/wiki/PC/SC)
 - [x] [Libnfc](https://github.com/nfc-tools/libnfc) (>1.5.0)


### Hardware
> ACS NFC ACR122U RFID Contactless Smart IC Card Reader13,56 MHz
> Buy this : **[Click here](https://www.amazon.com/ACS-ACR122U-Contactless-Smart-Reader/dp/B01KEGQFYY/ref=sr_1_3?dchild=1&keywords=ACS,%20ACR122U&qid=1627411981&sr=8-3)**


### What you need to run this project

[Decrypt Help](https://emvlab.org/main/) 
First you need to install the following tools :
> Card Driver (Step 1)

    $ sudo apt-get install pcsc-tools pcscd
	$ sudo apt-get install libpcsclite-dev
    $ sudo apt-get install libpcsclite1
    $ sudo apt-get install libusb-dev
    $ sudo apt-get install libnfc-bin
    $ sudo apt-get libnfc-dev 
    $ sudo apt-get libnfc5
    
> Fix Card Nfc (Step 2)

    $ sudo modprobe -r pn533
    $ sudo modprobe -r nfc
    $ sudo modprobe -r pn533_usb
    $ sudo service pcscd restart

> Compiler (Step 3)

    sudo apt-get install gcc
    sudo apt-get install g++
    sudo apt-get install make

#### :point_right: Run the project :point_left:

    $ git clone https://github.com/NaomiO/CardAttack
    $ cd ./CardAttack
    $ make all 
    $ ./hackCard

> To run and save output in file 

    ./hackCard >> OutputCard.txt 
> Clean after run 

    $ make Clean 

### Colaborators

- Naomi Oyer -- [*Github*](https://github.com/NaomiO)
- Shalhevet Gamliel -- [*Github*](https://github.com/ShalhevetGamliel) 

### License

- Based on a project from [Azet](https://github.com/azet) (in c)

### Documentation 

- [Doc 1 ](https://www.pcisecuritystandards.org/documents/Contactless_Payments_on_COTS-Security_and_Test_Requirements-v1.0.pdf)
- [Doc 2](https://www.emvco.com/wp-content/uploads/2017/05/C-1_Kernel_1_v2.6_20160512101416661.pdf)
- [Doc 3](https://www.emvco.com/wp-content/uploads/2017/05/Book_A_Architecture_and_General_Rqmts_v2_6_Final_20160422011856105.pdf)
- [Doc 4](https://www.emvco.com/terms-of-use/?u=/wp-content/uploads/documents/EMV_v4.3_Book_1_ICC_to_Terminal_Interface_2012060705394541.pdf)
- [Doc 5](https://www.emvco.com/wp-content/plugins/pmpro-customizations/oy-getfile.php?u=/wp-content/uploads/documents/EMV_v4.3_Book_2_Security_and_Key_Management_20120607061923900.pdf)
