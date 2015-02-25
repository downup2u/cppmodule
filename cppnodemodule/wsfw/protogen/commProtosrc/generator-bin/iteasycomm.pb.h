/* Automatically generated nanopb header */
/* Generated by nanopb-0.2.6 at Sat Mar 15 09:59:35 2014. */

#ifndef _PB_ITEASYCOMM_PB_H_
#define _PB_ITEASYCOMM_PB_H_
#include <pb.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
/* Struct definitions */
typedef struct _iteasycomm_PkgLogReply_StruLogReply {
    int32_t id;
    bool issuccess;
} iteasycomm_PkgLogReply_StruLogReply;

typedef struct _iteasycomm_PkgLogReq_StruLogReq {
    int32_t id;
    pb_callback_t logtime;
    int32_t logtype;
    int32_t loglevel;
    pb_callback_t logcontent;
} iteasycomm_PkgLogReq_StruLogReq;

typedef struct _iteasycomm_PkgLoginReply {
    bool issuccess;
    pb_callback_t servertime;
    bool has_reqpinginterval;
    int32_t reqpinginterval;
    pb_callback_t err;
} iteasycomm_PkgLoginReply;

typedef struct _iteasycomm_PkgLoginReq {
    pb_callback_t username;
    pb_callback_t password;
    pb_callback_t policylastid;
    pb_callback_t policyinlinelastid;
} iteasycomm_PkgLoginReq;

typedef struct _iteasycomm_PkgPing {
    int32_t id;
} iteasycomm_PkgPing;

typedef struct _iteasycomm_PkgPolicyEcho {
    pb_callback_t policylastid;
    bool issuccess;
} iteasycomm_PkgPolicyEcho;

typedef struct _iteasycomm_PkgPolicyPush {
    pb_callback_t policylastid;
    pb_callback_t policyprocessname;
    pb_callback_t policyextfilename;
    pb_callback_t policydisableprocessname;
    int32_t policyofflinetime;
} iteasycomm_PkgPolicyPush;

typedef struct _iteasycomm_PkgPolicyinlineEcho {
    pb_callback_t policylastid;
    bool issuccess;
} iteasycomm_PkgPolicyinlineEcho;

typedef struct _iteasycomm_PkgPolicyinlinePush {
    pb_callback_t policyinlinelastid;
    pb_callback_t policyinlinefile;
} iteasycomm_PkgPolicyinlinePush;

typedef struct _iteasycomm_PkgLogReply {
    pb_callback_t logreplylist;
} iteasycomm_PkgLogReply;

typedef struct _iteasycomm_PkgLogReq {
    pb_callback_t logreqlist;
} iteasycomm_PkgLogReq;

/* Default values for struct fields */

/* Field tags (for use in manual encoding/decoding) */
#define iteasycomm_PkgLogReply_StruLogReply_id_tag 1
#define iteasycomm_PkgLogReply_StruLogReply_issuccess_tag 2
#define iteasycomm_PkgLogReq_StruLogReq_id_tag   1
#define iteasycomm_PkgLogReq_StruLogReq_logtime_tag 2
#define iteasycomm_PkgLogReq_StruLogReq_logtype_tag 3
#define iteasycomm_PkgLogReq_StruLogReq_loglevel_tag 4
#define iteasycomm_PkgLogReq_StruLogReq_logcontent_tag 5
#define iteasycomm_PkgLoginReply_issuccess_tag   1
#define iteasycomm_PkgLoginReply_servertime_tag  2
#define iteasycomm_PkgLoginReply_reqpinginterval_tag 3
#define iteasycomm_PkgLoginReply_err_tag         4
#define iteasycomm_PkgLoginReq_username_tag      1
#define iteasycomm_PkgLoginReq_password_tag      2
#define iteasycomm_PkgLoginReq_policylastid_tag  3
#define iteasycomm_PkgLoginReq_policyinlinelastid_tag 4
#define iteasycomm_PkgPing_id_tag                1
#define iteasycomm_PkgPolicyEcho_policylastid_tag 1
#define iteasycomm_PkgPolicyEcho_issuccess_tag   2
#define iteasycomm_PkgPolicyPush_policylastid_tag 1
#define iteasycomm_PkgPolicyPush_policyprocessname_tag 2
#define iteasycomm_PkgPolicyPush_policyextfilename_tag 3
#define iteasycomm_PkgPolicyPush_policydisableprocessname_tag 4
#define iteasycomm_PkgPolicyPush_policyofflinetime_tag 5
#define iteasycomm_PkgPolicyinlineEcho_policylastid_tag 1
#define iteasycomm_PkgPolicyinlineEcho_issuccess_tag 2
#define iteasycomm_PkgPolicyinlinePush_policyinlinelastid_tag 1
#define iteasycomm_PkgPolicyinlinePush_policyinlinefile_tag 2
#define iteasycomm_PkgLogReply_logreplylist_tag  1
#define iteasycomm_PkgLogReq_logreqlist_tag      1

/* Struct field encoding specification for nanopb */
extern const pb_field_t iteasycomm_PkgPing_fields[2];
extern const pb_field_t iteasycomm_PkgLoginReq_fields[5];
extern const pb_field_t iteasycomm_PkgLoginReply_fields[5];
extern const pb_field_t iteasycomm_PkgLogReq_fields[2];
extern const pb_field_t iteasycomm_PkgLogReq_StruLogReq_fields[6];
extern const pb_field_t iteasycomm_PkgLogReply_fields[2];
extern const pb_field_t iteasycomm_PkgLogReply_StruLogReply_fields[3];
extern const pb_field_t iteasycomm_PkgPolicyPush_fields[6];
extern const pb_field_t iteasycomm_PkgPolicyEcho_fields[3];
extern const pb_field_t iteasycomm_PkgPolicyinlinePush_fields[3];
extern const pb_field_t iteasycomm_PkgPolicyinlineEcho_fields[3];

/* Maximum encoded size of messages (where known) */
#define iteasycomm_PkgPing_size                  11
#define iteasycomm_PkgLogReply_StruLogReply_size 13

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif