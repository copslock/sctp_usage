#ifndef UE_H
#define UE_H

#include <stdint.h>

#include "common.h"

struct attach_req {
    uint8_t imsi[10];
    long enb_ue_s1ap_id;
    uint8_t plmn_id;
    uint8_t tai;
    uint8_t net_cap;
};
struct auth_req {
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t auth_challenge;
};
struct auth_res {
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t auth_challenge_answer;
};
struct sec_mode_command {
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t sec_algo;
};
struct sec_mode_complete {
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t tai;
    uint8_t plmn_id;
};
struct attach_accept {
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t ambr;
    uint8_t sec_cap;
};
union message_union {
    struct attach_req attach_req;
    struct auth_req auth_req;
    struct auth_res auth_res;
    struct sec_mode_command sec_mode_command;
    struct sec_mode_complete sec_mode_complete;
    struct attach_accept attach_accept;
};

struct message {
    enum s1ap_message_type type;
    union message_union message_union;
};
struct ue_info {
    uint8_t ue_id[10]; //IMSI or GUTI
    long enb_ue_s1ap_id;
    long mme_ue_s1ap_id;
    uint8_t plmn_id;
    uint8_t tai;
    struct message message;
    int datalen;
    enum ue_state ue_state;
};

extern struct ue_info *ue_info_arr;
#endif