#ifndef COMPNET_LAB4_SRC_SWITCH_H
#define COMPNET_LAB4_SRC_SWITCH_H

#include "types.h"

struct mac_entry{
    int time=0;
    mac_addr_t MAC_addr;
    int outPort=0;
    mac_entry* next=NULL;
};

class SwitchBase {
 public:
  SwitchBase() = default;
  ~SwitchBase() = default;

  virtual void InitSwitch(int numPorts) = 0;
  virtual int ProcessFrame(int inPort, char* framePtr) = 0;
};

extern SwitchBase* CreateSwitchObject();

// TODO : Implement your switch class.
class EthernetSwitch:public SwitchBase{
public:
    
    int entry_num=1;
    int port_num=0;
    mac_entry* head;
    void InitSwitch(int numPorts) override{
        port_num=numPorts;
        head=new mac_entry;
        head->outPort=1;
        head->time=10;
    }
    int ProcessFrame(int inPort, char *framePtr) override{
        ether_header_t header;
        int outPort=0;
        memcpy(&header,framePtr, sizeof(ether_header_t));
        if(header.ether_type==ETHER_DATA_TYPE){
            if(inPort<=1||inPort>port_num)
                return -1;
            mac_entry* p=NULL;
            mac_entry* q=NULL;
            p=head;
            while(1){
                p=p->next;
                if(p==NULL)
                    break;
                if(memcmp(&p->MAC_addr,&header.ether_dest, sizeof(mac_addr_t))==0&&p->time>0){
                    break;
                }
            }
            if(p==NULL){
                outPort=0;
            }
            if(p!=NULL){
                if(p->outPort==inPort)
                    outPort=-1;
                else if(p->outPort>1&&p->outPort<=port_num)
                    outPort=p->outPort;
            }
            p=head->next;
            q=head;
            while(1){
                if(p==NULL){
                    p=new mac_entry;
                    p->outPort=inPort;
                    p->next=NULL;
                    memcpy(&p->MAC_addr,&header.ether_src, sizeof(mac_addr_t));
                    p->time=ETHER_MAC_AGING_THRESHOLD;
                    q->next=p;
                    break;
                }
                if(memcmp(&p->MAC_addr,&header.ether_src, sizeof(mac_addr_t))==0){
                    p->time=ETHER_MAC_AGING_THRESHOLD;
                    break;
                }
                p=p->next;
                q=q->next;
            }
            return outPort;
        }
        if(header.ether_type==ETHER_CONTROL_TYPE){
            mac_entry* p;
            mac_entry* q;
            p=head->next;
            q=head;
            while(p!=NULL){
                if(p->time!=0){
                    p->time--;
                }
                if(p->time==0){
                    mac_entry* temp=p->next;
                    q->next=temp;
                    delete p;
                    p=temp;
                } else{
                    p=p->next;
                    q=q->next;
                }
            }
            return -1;
        }
    }
};

#endif  // ! COMPNET_LAB4_SRC_SWITCH_H
