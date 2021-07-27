
struct Command{
     
      const unsigned char HEADER[6] = {
                0x40,0x01,
                0x00,0xA4,0x04,0x00 // CLA - INS - P1 - P2
      }; 

      const unsigned char PPSE[22] = {
          0x40,0x01,
          0x00,0xA4,0x04,0x00, // CLA - INS - P1 - P2
          0x0e, // size
          0x32,0x50,0x41,0x59,0x2e,0x53,0x59,0x53,0x2e,0x44,0x44,0x46,0x30,0x31, // 2PAY.SYS.DDF01
          0x00
      };


      const unsigned char RECORD[7] = {0x40,0x01,
                0x00,0xB2, // RECORD
                0x00,0x00, // SFI,  edited
                0x00
      };

      const unsigned char LOG[7] = {0x40,0x01,
                  0x80,0xCA, // DATA
                  0x9F,0x4F, // log format
                  0x00
      };
    const nfc_modulation modulation[2] = {
                                            { NMT_ISO14443A, NBR_106 },
                                            { NMT_ISO14443B, NBR_106 },
       };
     const unsigned char TEMPLATE_EMV =0x61;  
     const unsigned char ID_EMV = 0x4F;  
     const unsigned char NextEntry = 0x61;  
     const unsigned char INDICATOR_P = 0x87;  
     const unsigned char EMVLABEL = 0x50;  
     const unsigned char LANGUAGEPREF = 0x2D;  
     const unsigned char LANGUAGEPREF2 = 0x5F;  
     const unsigned char  PDOL_ = 0x38;  
     const unsigned char  FILEcONTROLE_ = 0xBF;  
     const unsigned char  FILEcONTROLE_2 = 0x0C;  
     const unsigned char  COMMANDENTRY_2 = 0x4D;  
     const unsigned char TRACK2 = 0x57;  
     const unsigned char NAMECARD_2 = 0x20;  

     const unsigned char TRACK_1_ = 0x1F;  
     const unsigned char DATE = 0x9A;  
     const unsigned char NEXT = 0x9C;  
     const unsigned char LOG2 = 0x9F;  
     const unsigned char TIME_2 = 0x21;  
     const unsigned char CURRENCY = 0x5F;  
     const unsigned char CURRENCY_2 = 0x2A;  
   
     const unsigned char AMOUNT_2 = 0x02;  
     const unsigned char MERCHANT = 0x4E;  
     const unsigned char COUNTER = 0x36;  
     const unsigned char TERMINAL = 0x1A;  
     const unsigned char CRYPTO = 0x27;  

 };