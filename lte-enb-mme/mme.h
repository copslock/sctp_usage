#ifndef MME_H
#define MME_H

#include <stdbool.h>
#include <stdint.h>

//bool handle_local = true; // TODO: Set it based upon a command line parameter

void process_message(struct message *msg, long id);
void build_auth_request(long id);
void build_sec_mode_command(long id);
void build_attach_accept(long id);
int wait_for_msg();
void cleanup();
void handle_received_message(uint8_t *buffer);
void create_connection();
void send_sctp_message(long id);
#endif