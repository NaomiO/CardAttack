

#include <iostream>
#include "class/emv.hpp"


nfc_device* current_pnd = NULL;

int	main() {
  
  nfc_context *current_reader;
  
  //lunch init
  nfc_init(&current_reader);

  //erro init libnfc
  if (current_reader == NULL) exit(EXIT_FAILURE);
  
  current_pnd = nfc_open(current_reader, NULL);

  // Error open device
  if (current_pnd == NULL) {
    nfc_exit(current_reader);
    exit(EXIT_FAILURE);
  }

  // Error init
  if (nfc_initiator_init(current_pnd) < 0) {
    nfc_perror(current_pnd, "nfc_initiator_init");
    nfc_close(current_pnd);
    nfc_exit(current_reader);
    exit(EXIT_FAILURE);
  }

  cout << "Succes Connect Reader Card"<< endl;


  nfc_target nfc_target_current;
 
  if (nfc_initiator_poll_target(current_pnd, _Command.modulation, 2, 1, 2, &nfc_target_current) < 0) {
    nfc_perror(current_pnd, "nfc_initiator_poll_target");
    nfc_close(current_pnd);
    nfc_exit(current_reader);
    exit(EXIT_FAILURE);
  }


  list<EMV> ppse_all = EmvClass::getPPSE();

  if (ppse_all.size() == 0) return 1;
  
  EmvClass infos[ppse_all.size()];
  int index = 0;
  for (EMV app : ppse_all) {
    
    DataUnit response = EmvClass::IsPriority( app.priority,ppse_all);
    
    if (response.len == 0) continue;

    infos[index].extractAppResponse(app, response);
    infos[index].RecordExtract();
    infos[index].LogExtract();
    index++;
  }

  for (int index = 0; index < ppse_all.size(); ++index) 
    infos[index].GetDetails();

  return 0;
}
