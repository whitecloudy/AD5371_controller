#include "IPC_controller.h"
#include <iostream>
#include <cstring>
#include <algorithm>

//#define __IPC_DEBUG__


IPC_controller::IPC_controller(){
  receiver_key = ftok(PATH, 's');
  sender_key = ftok(PATH, 'c');

  if((receiver_key < 0) || (sender_key < 0)){
    std::cerr << "IPC key initialize failed!";
    exit(1);
  }

  receiver_id = msgget(receiver_key, 0666 | IPC_CREAT);
  sender_id = msgget(sender_key, 0666 | IPC_CREAT);

#ifdef __IPC_DEBUG__
  std::cout << "r_id : "<<receiver_id<<std::endl;
  std::cout << "s_id : "<<sender_id<<std::endl;
#endif

  std::cout<<"IPC initialize success"<<std::endl;
}



IPC_controller::~IPC_controller(){
  msgctl(receiver_id, IPC_RMID, NULL);
  msgctl(sender_id, IPC_RMID, NULL);
}




int IPC_controller::data_send(void * buf, int buf_len, char flag){
  send_buf.msg_type = 1;
  memset(send_buf.msg_data, 0, IO_BUF_SIZE);

  memcpy(send_buf.msg_data, buf, std::min(IO_BUF_SIZE, buf_len));
  send_buf.msg_flag = flag;

  int rt = msgsnd(sender_id, &send_buf, sizeof(send_buf) - sizeof(long), 0);

#ifdef __IPC_DEBUG__
  std::cout<<"ipc data send result : "<<rt<<std::endl;
  std::cout<<"size of : "<<sizeof(send_buf)<<std::endl;
#endif

  return rt;
}




int IPC_controller::data_recv(void * buf, int buf_len){
  int n = msgrcv(receiver_id, &recv_buf, sizeof(recv_buf) - sizeof(long), 0, 0);

#ifdef __IPC_DEBUG__
  std::cout << "ipc data receive"<<std::endl;
  std::cout << "flag : "<<(int)(recv_buf.msg_flag)<< ", n : "<<n<<std::endl;
  std::cout << "return value would be " << (recv_buf.msg_flag & IPC_SYNC__) <<std::endl;
#endif


  if((recv_buf.msg_flag & IPC_ACK__)) //ACK
    return -IPC_ACK__;
  if((recv_buf.msg_flag & IPC_FIN__)) //FIN
    return -IPC_FIN__;
  if((recv_buf.msg_flag & IPC_SYNC__))  //SYNC
    return -IPC_SYNC__;

  memcpy(buf, recv_buf.msg_data, std::min(IO_BUF_SIZE, buf_len));

  return n;
}


int IPC_controller::send_sync(){
  char dummy_buf[IO_BUF_SIZE] = {};
  int rt;

  while(1){
    if(data_send(dummy_buf, IO_BUF_SIZE, IPC_SYNC__)){
      std::cerr <<"SYNC send error";
      return 1;
    }
    rt = data_recv(dummy_buf);
    if(rt == -1)  //Receive Error
      return 1;
    else if(rt == -IPC_ACK__){ //Receive proper ACK
      std::cout<< "SYNC complete"<<std::endl; 
      return 0;
    }else if(rt == -IPC_FIN__) //Wrong FIN
      std::cout << "Wrong FIN came"<<std::endl;
    return 1;
  }
}



int IPC_controller::wait_sync(){
  char dummy_buf[IO_BUF_SIZE] = {};
  int rt;

  while(1){
    rt = data_recv(dummy_buf);
#ifdef __IPC_DEBUG__
    std::cout<<"data_recv return : "<<rt<<std::endl;
    std::cout<<"It is supposed to be : "<<-IPC_SYNC__<<std::endl;
#endif

    if(rt == -1)  //Receive Error
      return 1;
    else if(rt == -IPC_SYNC__){ //SYNC
      data_send(dummy_buf, IO_BUF_SIZE, IPC_ACK__);
      std::cout<< "SYNC complete"<<std::endl; 
      return 0;
    }else if(rt == -IPC_FIN__) //Wrong FIN
      std::cout << "Wrong FIN came"<<std::endl;
    return 1;
  }
}

