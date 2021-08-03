#include <map>
#include <list>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cstring>

extern "C" {
#include <nfc/nfc.h>
#include <nfc/nfc-types.h>
# define PN32X_TRANSCEIVE
  int pn53x_transceive(struct nfc_device *current_pnd, const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t data_loadLen, int timeout);
}

#include "../struct/emv.hpp"
#include "../struct/dataUnit.hpp"
#include "../Command/command.hpp"

using namespace std;
#define PRINT(c) hex << uppercase << setw(2) << setfill('0') << (unsigned int)c << dec

extern nfc_device* current_pnd;

Command _Command;

class EmvClass {
 private:
      EMV _EMV;
      char CardUserName[56]; 

      DataUnit TAG_9F38; 
      DataUnit Discretionaire; 
      DataUnit Tag57;
      DataUnit test;


      unsigned char _logTag88;
      unsigned char _logCount;

      DataUnit LogCard; 
      DataUnit Log_Input[0x20];
     
  public:

      inline static unsigned char abtRx[300];
      inline static int data_load;
    
      inline static bool isTrailer(){
          if (data_load < 2) return true;
          if (abtRx[data_load - 2] == 0x90 && abtRx[data_load - 1] == 0) return false;
          return true;
      }

      inline static list<EMV> getPPSE(){
        list<EMV> emv_data;
        DataUnit getPPSE = runCommand("SELECT PPSE",
                                      _Command.PPSE,
                                       sizeof(_Command.PPSE)
                                        );

        if (getPPSE.len == 0) return emv_data;

        for (unsigned int i = 0; i < data_load; ++i) {
          if (abtRx[i] == _Command.TEMPLATE_EMV) { // template
            EMV app;
            ++i;
            while (i < data_load && abtRx[i] != _Command.NEXT) { 
              if (abtRx[i] == _Command.ID_EMV) { // EMV ID
                unsigned char len = abtRx[++i];
                i++;
                memcpy(app.IdentifierApp, &abtRx[i], len);
                i += len - 1;
              }

              if (abtRx[i] == _Command.INDICATOR_P) {
                i += 2;
                app.priority = abtRx[i];
              }
              ++i;
              if (abtRx[i] == _Command.EMVLABEL) { 
                unsigned char len = abtRx[++i];
                i++;
                memcpy(app.name, &abtRx[i], len);
                app.name[len] = 0;
                i += len - 1;
              }
            }
            emv_data.push_back(app);
            --i;
          }
        }
       return emv_data;
      }

      inline static DataUnit IsPriority( unsigned char priority,list<EMV> const& list){

        EMV app;

        for (EMV a : list) {
          if (a.priority == priority) {
            app = a;
            break;
          }
        }
        
        unsigned char current_app[256];
        unsigned char size = 0;

        bzero(current_app, 256);
        
        // Header
        memcpy(current_app, _Command.HEADER, sizeof(_Command.HEADER));
        size = sizeof(_Command.HEADER);
        
        // Len
        current_app[size] = sizeof(app.IdentifierApp);
        size += 1;
        
        // IdentifierApp
        memcpy(current_app + size, app.IdentifierApp, sizeof(app.IdentifierApp));
        size += sizeof(app.IdentifierApp);
     
        size += 1;
        return runCommand( "SELECT APP",current_app, size);
      }

      inline static DataUnit runCommand( char const* name,unsigned char const* command, unsigned int size){
          data_load = pn53x_transceive(current_pnd,command, size, abtRx, sizeof(abtRx), 0);

          if (isTrailer() || data_load < 0 ) {
            if (data_load < 0) nfc_perror(current_pnd, name);
            return {0, {0}};
          }
            
          DataUnit _response;
          _response.len = data_load - 1;
          memcpy(_response.value, abtRx + 1, data_load - 1);
          return _response;
      }
      
      EmvClass(): 
                TAG_9F38({0, {0}}),
                Discretionaire({0, {0}}),
                Tag57({0, {0}}),
                test({0, {0}}),
                _logTag88(0),
                _logCount(0),
                LogCard({0, {0}}),
                Log_Input({{0, {0}}})
              {
                  bzero(CardUserName, sizeof(CardUserName));
              }

      int extractAppResponse(EMV const& app, DataUnit const& app_response) {        
          _EMV = app;

          unsigned char const* buffer = app_response.value;
          unsigned int size = app_response.len;

          for (unsigned int i = 0; i < size; ++i) {
            if (i + 1 < size && buffer[i] == _Command.LANGUAGEPREF2 && buffer[i + 1] == _Command.LANGUAGEPREF) {
              i += 2;
              i += buffer[i++] - 1;
            }
            else if (i + 1 < size &&
              buffer[i] == _Command.LOG2 && buffer[i + 1] == _Command.PDOL_) { 
              i += 2;
              TAG_9F38.len = buffer[i++];;
              memcpy(TAG_9F38.value, &buffer[i], TAG_9F38.len);
              i += TAG_9F38.len - 1;
            } else if (i + 1 < size &&  buffer[i] == _Command.FILEcONTROLE_ && buffer[i + 1] == _Command.FILEcONTROLE_2 ) { 
              i += 2;
              unsigned char len = buffer[i++];;
              for (unsigned int j = 0; j < len; ++j) {
                  if (j + 1 < len && buffer[i + j] == _Command.LOG2 && buffer[i + j + 1] == _Command.COMMANDENTRY_2) {
                      j += 3;
                      _logTag88 = buffer[i + j++];
                      _logCount = buffer[i + j];
                  }
              }
              i += len - 1;
            }
          }
      }

      int LogExtract() {

        LogCard = EmvClass::runCommand(
                                          "GET DATA LOG FORMAT",
                                            _Command.LOG,
                                            sizeof(_Command.LOG)
                                            );
        
        if (LogCard.len == 0)   return 1;
        
        unsigned char readRecord[sizeof(_Command.RECORD)];
        memcpy(readRecord, _Command.RECORD, sizeof(readRecord));

        readRecord[5] = (1 << 2) | (_logTag88 << 3) ;

        for (unsigned int i = 0; i < _logCount; ++i) {
          readRecord[4] = i + 1; 
          Log_Input[i] = EmvClass::runCommand(
                                            "READ RECORD: LOGFILE",
                                            readRecord,
                                            sizeof(readRecord)
                                          );

          if (Log_Input[i].len == 0) return 1;
        }

        return 0;
      }
      
      int RecordExtract() {
        DataUnit restult;
        DataUnit readRecord;
        
        readRecord.len = sizeof(_Command.RECORD);
        memcpy(readRecord.value, _Command.RECORD, sizeof( _Command.RECORD));
        
        for (unsigned int Tag88 = 1; Tag88 <= 2; ++Tag88) {

          readRecord.value[5] =  (1 << 2) | (Tag88 << 3) ;

          for (unsigned int record = 1; record <= 2; ++record) {

            readRecord.value[4] = record; 

            restult = EmvClass::runCommand(
                                            "READ RECORD BASE",
                                            readRecord.value,
                                            readRecord.len
                                            );
            
            if (restult.len == 0) continue;

            unsigned char const* buffer = restult.value;
            
            for (unsigned int i = 0; i < restult.len; ++i) {
              if (buffer[i] == _Command.TRACK2 && Tag57.len == 0) {// Track2
                  i++;
                  Tag57.len = buffer[i++];
                  memcpy(Tag57.value, &buffer[i], Tag57.len);
                  i += Tag57.len - 1;      
              }else if (i + 1 < restult.len  && buffer[i + 1] == _Command.NAMECARD_2 && buffer[i] == _Command.TRACK2) {// Name card
                  i += 2;
                  if (buffer[i++] > 2) 
                      memcpy(CardUserName, &buffer[i], buffer[i++]);
                      i += buffer[i++] - 1;
              }else if (buffer[i] == _Command.LOG2 && i + 1 < restult.len && Discretionaire.len == 0  && buffer[i + 1] == _Command.TRACK_1_) { // Track1 
                      i += 2;
                      Discretionaire.len = buffer[i++];
                      memcpy(Discretionaire.value, &buffer[i], Discretionaire.len);
                      i += Discretionaire.len - 1;
              }    
            }
          }
        }
        return 0;
      }
     
      void GetDetails() const {
              cout << "Name Card's" << ": " << CardUserName << endl;
              unsigned char const* test1 = test.value;

              cout << "Type of card1" << ": " << test.value << endl;
              
              unsigned char const* buffer = Tag57.value;

              unsigned int i;
              for (i = 0; i < 8; ++i)  cout << PRINT(test1[i]) << (!(i & 1) ? "" : "-");

              cout << "Number Card: ";
              for (i = 0; i < 8; ++i)  cout << PRINT(buffer[i]) << (!(i & 1) ? "" : "-");
              
              cout << endl;

              unsigned char year = buffer[i++] << 4;
              year |= buffer[i] >> 4;
              unsigned char month = buffer[i++] << 4;
              month |= buffer[i] >> 4;

              cout << "Expiry : " << PRINT(month) << "/20" << PRINT(year) << endl;
           
              cout << endl << "Orders Logs"  <<endl;

              unsigned char const* format = LogCard.value;
              unsigned int index = 0;

              for (DataUnit this_input : Log_Input) {
                 if (this_input.len == 0)  break;
                  index++;
                  cout << endl <<index<< " => ";
                  unsigned int e = 0;
                  for (unsigned int i = 0; i < LogCard.len; ++i) {
                    if (format[i] == _Command.DATE) { // Date
                        i++;
                        cout <<endl<<"  Date => ";
                        for (unsigned int j = 0; j < format[i]; ++j)  
                            cout <<(j == 0 ? "" : "/") << (j == 0 ? "20" : "") << PRINT(this_input.value[e++]);
                    }else if (format[i] == _Command.NEXT) { //  
                      i++;
                    }else if (i + 1 < LogCard.len) {
                        if (format[i] == _Command.LOG2 && format[i + 1] == _Command.TIME_2) { // Time
                            i += 2;
                        }else if (format[i] == _Command.CURRENCY && format[i + 1] == _Command.CURRENCY_2) { // Currency
                          i += 2;
                          cout <<endl<<"  Currency => ";
                          unsigned short value = this_input.value[e+1] | this_input.value[e] << 8 ;
                        
                          for (unsigned int j = 0; j < format[i]; ++j) cout << PRINT(this_input.value[e++]);	  
                        
                      }else if (format[i] == _Command.LOG2 && format[i + 1] == _Command.AMOUNT_2) { // Amount
                        i += 2;
                        cout <<endl<<"  Amount => ";
                        int flag = 0;
                        for (unsigned int j = 0; j < format[i]; ++j) {
                            if (flag==0 && j < 4 && this_input.value[e] == 0) { 
                                e++;
                                continue;
                            }else {
                                flag = 1;
                            }
                              cout << PRINT(this_input.value[e++]);
                              if (j == 4) cout << ",";
                            }
                      } else if (format[i] == _Command.LOG2 && format[i + 1] == _Command.MERCHANT) { // Merchant
                        i += 2;
                      }else if (format[i] == _Command.LOG2 && format[i + 1] == _Command.COUNTER) { // Counter
                        i += 2;
                      } else if (format[i] == _Command.LOG2 && format[i + 1] == _Command.TERMINAL) { // Terminal country code
                        i += 2;
                      } else if (format[i] == _Command.LOG2 && format[i + 1] == _Command.CRYPTO) { // Crypto info data
                        i += 2;
                        cout <<endl<< "  Transaction Type =>";
                        for (unsigned int j = 0; j < format[i]; ++j) 
                          cout << PRINT(this_input.value[e++]);
                      }
                      }
                  }
              cout << endl;
              }
            }
};

