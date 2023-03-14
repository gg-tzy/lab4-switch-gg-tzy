#include "switch.h"

SwitchBase* CreateSwitchObject() {
  // TODO : Your code.
  return new EthernetSwitch();
}