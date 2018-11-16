#include <stdio.h>

#include "common.h"
#include "ue.h"
#include "mme.h"

void handle_received_message(uint8_t *buffer)
{
    long id = 0;
    struct message *msg = (struct message *)buffer;
    if (msg->type == ATTACH_REQ)
    {
        ue_info_arr[id].mme_ue_s1ap_id = id;
        ++id;
    }
    process_message(msg, id);
}

void process_message(struct message *msg, long id)
{
    switch (msg->type)
    {
    case ATTACH_REQ:
    {
        debug_print("Attach Req received\n");
        struct attach_req *attach_req = (struct attach_req *)&msg->message_union;
        ue_info_arr[id].ue_id[0] = attach_req->imsi[0];
        ue_info_arr[id].tai = attach_req->tai;
        ue_info_arr[id].enb_ue_s1ap_id = attach_req->enb_ue_s1ap_id;
        ue_info_arr[id].ue_state = IDLE;
        ue_info_arr[id].plmn_id = attach_req->plmn_id;
        build_auth_request(id);
        send_sctp_message(id);
    }
    break;

    case AUTH_RES:
    {
        debug_print("Authentication response received\n");
        struct sec_mode_command *sec_mode_command = (struct sec_mode_command *)&msg->message_union;
        int id = sec_mode_command->mme_ue_s1ap_id;
        build_sec_mode_command(id);
        send_sctp_message(id);
    }
    break;

    case SEC_MODE_COMPLETE:
    {
        debug_print("Security Mode Complete received\n");
        struct sec_mode_complete *sec_mode_complete = (struct sec_mode_complete *)&msg->message_union;
        int id = sec_mode_complete->mme_ue_s1ap_id;
        ue_info_arr[id].ue_state = CONNECTED;
        build_attach_accept(id);
        send_sctp_message(id);
    }
    break;

    default:
        printf("Unknown message received!\n");
        break;
    }
}

void build_auth_request(long id)
{
    ue_info_arr[id].message.type = AUTH_REQ;
    ue_info_arr[id].message.message_union.auth_req.mme_ue_s1ap_id = id;
    ue_info_arr[id].message.message_union.auth_req.enb_ue_s1ap_id = ue_info_arr[id].enb_ue_s1ap_id;
    ue_info_arr[id].message.message_union.auth_req.auth_challenge = 0xaa;
    ue_info_arr[id].datalen = sizeof(struct message);
}

void build_sec_mode_command(long id)
{
    ue_info_arr[id].message.type = SEC_MODE_COMMAND;
    ue_info_arr[id].message.message_union.sec_mode_command.mme_ue_s1ap_id = id;
    ue_info_arr[id].message.message_union.sec_mode_command.enb_ue_s1ap_id = ue_info_arr[id].enb_ue_s1ap_id;
    ue_info_arr[id].message.message_union.sec_mode_command.sec_algo = 0xaa;
    ue_info_arr[id].datalen = sizeof(struct message);
}

void build_attach_accept(long id)
{
    ue_info_arr[id].message.type = ATTACH_ACCEPT;
    ue_info_arr[id].message.message_union.attach_accept.mme_ue_s1ap_id = ue_info_arr[id].mme_ue_s1ap_id;
    ue_info_arr[id].message.message_union.attach_accept.ambr = 100;
    ue_info_arr[id].message.message_union.attach_accept.sec_cap = 100;
    ue_info_arr[id].datalen = sizeof(struct message);
}
