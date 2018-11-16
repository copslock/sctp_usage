#ifndef ENB_H
#define ENB_H

#include "common.h"
#include "ue.h"

int socket_fd;

void *wait_for_msg(void *vargp);
void send_sctp_message(long id);
void cleanup();
void create_connection();
void state_machine(enum s1ap_message_type type, long id);
void build_attach(long id);
void build_auth_response(long id);
void build_sec_mode_complete(long id);
void handle_received_message(uint8_t *buffer);
void process_message(struct message *msg, long id);
#endif