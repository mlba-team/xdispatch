/*
 * IDENTIFICATION:
 * stub generated Sat Jun 26 16:53:17 2010
 * with a MiG generated Wed Nov 25 05:11:15 PST 2009 by root@hokies.apple.com
 * OPTIONS:
 */

#include <config/config.h>

#if HAVE_MACH

#define	__MIG_check__Reply__libdispatch_internal_protocol_subsystem__ 1
#define	__NDR_convert__Reply__libdispatch_internal_protocol_subsystem__ 1
#define	__NDR_convert__mig_reply_error_subsystem__ 1

#include "protocol.h"


#ifndef	mig_internal
#define	mig_internal	static __inline__
#endif	/* mig_internal */

#ifndef	mig_external
#define mig_external
#endif	/* mig_external */

#if	!defined(__MigTypeCheck) && defined(TypeCheck)
#define	__MigTypeCheck		TypeCheck	/* Legacy setting */
#endif	/* !defined(__MigTypeCheck) */

#if	!defined(__MigKernelSpecificCode) && defined(_MIG_KERNEL_SPECIFIC_CODE_)
#define	__MigKernelSpecificCode	_MIG_KERNEL_SPECIFIC_CODE_	/* Legacy setting */
#endif	/* !defined(__MigKernelSpecificCode) */

#ifndef	LimitCheck
#define	LimitCheck 0
#endif	/* LimitCheck */

#ifndef	min
#define	min(a,b)  ( ((a) < (b))? (a): (b) )
#endif	/* min */

#if !defined(_WALIGN_)
#define _WALIGN_(x) (((x) + 3) & ~3)
#endif /* !defined(_WALIGN_) */

#if !defined(_WALIGNSZ_)
#define _WALIGNSZ_(x) _WALIGN_(sizeof(x))
#endif /* !defined(_WALIGNSZ_) */

#ifndef	UseStaticTemplates
#define	UseStaticTemplates	0
#endif	/* UseStaticTemplates */

#ifndef	__MachMsgErrorWithTimeout
#define	__MachMsgErrorWithTimeout(_R_) { \
    switch (_R_) { \
    case MACH_SEND_INVALID_DATA: \
    case MACH_SEND_INVALID_DEST: \
    case MACH_SEND_INVALID_HEADER: \
        mig_put_reply_port(InP->Head.msgh_reply_port); \
        break; \
    case MACH_SEND_TIMED_OUT: \
    case MACH_RCV_TIMED_OUT: \
    default: \
        mig_dealloc_reply_port(InP->Head.msgh_reply_port); \
    } \
}
#endif	/* __MachMsgErrorWithTimeout */

#ifndef	__MachMsgErrorWithoutTimeout
#define	__MachMsgErrorWithoutTimeout(_R_) { \
    switch (_R_) { \
    case MACH_SEND_INVALID_DATA: \
    case MACH_SEND_INVALID_DEST: \
    case MACH_SEND_INVALID_HEADER: \
        mig_put_reply_port(InP->Head.msgh_reply_port); \
        break; \
    default: \
        mig_dealloc_reply_port(InP->Head.msgh_reply_port); \
    } \
}
#endif	/* __MachMsgErrorWithoutTimeout */

#ifndef	__DeclareSendRpc
#define	__DeclareSendRpc(_NUM_, _NAME_)
#endif	/* __DeclareSendRpc */

#ifndef	__BeforeSendRpc
#define	__BeforeSendRpc(_NUM_, _NAME_)
#endif	/* __BeforeSendRpc */

#ifndef	__AfterSendRpc
#define	__AfterSendRpc(_NUM_, _NAME_)
#endif	/* __AfterSendRpc */

#ifndef	__DeclareSendSimple
#define	__DeclareSendSimple(_NUM_, _NAME_)
#endif	/* __DeclareSendSimple */

#ifndef	__BeforeSendSimple
#define	__BeforeSendSimple(_NUM_, _NAME_)
#endif	/* __BeforeSendSimple */

#ifndef	__AfterSendSimple
#define	__AfterSendSimple(_NUM_, _NAME_)
#endif	/* __AfterSendSimple */

#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port



/* SimpleRoutine mach_notify_port_deleted */
mig_external kern_return_t _dispatch_send_mach_notify_port_deleted
(
    mach_port_t _notify,
    mach_port_name_t _name
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
        NDR_record_t NDR;
        mach_port_name_t _name;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__mach_notify_port_deleted_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__mach_notify_port_deleted_t__defined */

    __DeclareSendSimple(65, "mach_notify_port_deleted")

    InP->NDR = NDR_record;

    InP->_name = _name;

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _notify;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 65;

    __BeforeSendSimple(65, "mach_notify_port_deleted")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(65, "mach_notify_port_deleted")
    return msg_result;
}

/* SimpleRoutine mach_notify_port_destroyed */
mig_external kern_return_t _dispatch_send_mach_notify_port_destroyed
(
    mach_port_t _notify,
    mach_port_t _rights
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
        /* start of the kernel processed data */
        mach_msg_body_t msgh_body;
        mach_msg_port_descriptor_t _rights;
        /* end of the kernel processed data */
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__mach_notify_port_destroyed_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__mach_notify_port_destroyed_t__defined */

    __DeclareSendSimple(69, "mach_notify_port_destroyed")

#if	UseStaticTemplates
    const static mach_msg_port_descriptor_t _rightsTemplate = {
        /* name = */		MACH_PORT_NULL,
        /* pad1 = */		0,
        /* pad2 = */		0,
        /* disp = */		16,
        /* type = */		MACH_MSG_PORT_DESCRIPTOR,
    };
#endif	/* UseStaticTemplates */

    InP->msgh_body.msgh_descriptor_count = 1;
#if	UseStaticTemplates
    InP->_rights = _rightsTemplate;
    InP->_rights.name = _rights;
#else	/* UseStaticTemplates */
    InP->_rights.name = _rights;
    InP->_rights.disposition = 16;
    InP->_rights.type = MACH_MSG_PORT_DESCRIPTOR;
#endif	/* UseStaticTemplates */

    InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _notify;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 69;

    __BeforeSendSimple(69, "mach_notify_port_destroyed")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(69, "mach_notify_port_destroyed")
    return msg_result;
}

/* SimpleRoutine mach_notify_no_senders */
mig_external kern_return_t _dispatch_send_mach_notify_no_senders
(
    mach_port_t _notify,
    mach_port_mscount_t _mscnt
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
        NDR_record_t NDR;
        mach_port_mscount_t _mscnt;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__mach_notify_no_senders_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__mach_notify_no_senders_t__defined */

    __DeclareSendSimple(70, "mach_notify_no_senders")

    InP->NDR = NDR_record;

    InP->_mscnt = _mscnt;

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _notify;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 70;

    __BeforeSendSimple(70, "mach_notify_no_senders")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(70, "mach_notify_no_senders")
    return msg_result;
}

/* SimpleRoutine mach_notify_send_once */
mig_external kern_return_t _dispatch_send_mach_notify_send_once
(
    mach_port_t _notify
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__mach_notify_send_once_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__mach_notify_send_once_t__defined */

    __DeclareSendSimple(71, "mach_notify_send_once")

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _notify;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 71;

    __BeforeSendSimple(71, "mach_notify_send_once")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(71, "mach_notify_send_once")
    return msg_result;
}

/* SimpleRoutine mach_notify_dead_name */
mig_external kern_return_t _dispatch_send_mach_notify_dead_name
(
    mach_port_t _notify,
    mach_port_name_t _name
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
        NDR_record_t NDR;
        mach_port_name_t _name;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__mach_notify_dead_name_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__mach_notify_dead_name_t__defined */

    __DeclareSendSimple(72, "mach_notify_dead_name")

    InP->NDR = NDR_record;

    InP->_name = _name;

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _notify;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 72;

    __BeforeSendSimple(72, "mach_notify_dead_name")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(72, "mach_notify_dead_name")
    return msg_result;
}

/* SimpleRoutine wakeup_main_thread */
mig_external kern_return_t _dispatch_send_wakeup_main_thread
(
    mach_port_t _port,
    natural_t _waitTimeout
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__wakeup_main_thread_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__wakeup_main_thread_t__defined */

    __DeclareSendSimple(78, "wakeup_main_thread")

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(19, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _port;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 78;

    __BeforeSendSimple(78, "wakeup_main_thread")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_SEND_TIMEOUT|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, _waitTimeout, MACH_PORT_NULL);
    __AfterSendSimple(78, "wakeup_main_thread")

    if (msg_result == MACH_SEND_TIMED_OUT) {
    }

    return msg_result;
}

/* SimpleRoutine consume_send_once_right */
mig_external kern_return_t _dispatch_send_consume_send_once_right
(
    mach_port_t _port
)
{

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
    typedef struct {
        mach_msg_header_t Head;
    } Request;
#ifdef  __MigPackStructs
#pragma pack()
#endif
    /*
     * typedef struct {
     * 	mach_msg_header_t Head;
     * 	NDR_record_t NDR;
     * 	kern_return_t RetCode;
     * } mig_reply_error_t;
     */

    union {
        Request In;
    } Mess;

    Request *InP = &Mess.In;

    mach_msg_return_t msg_result;

#ifdef	__MIG_check__Reply__consume_send_once_right_t__defined
    kern_return_t check_result;
#endif	/* __MIG_check__Reply__consume_send_once_right_t__defined */

    __DeclareSendSimple(79, "consume_send_once_right")

    InP->Head.msgh_bits =
        MACH_MSGH_BITS(18, 0);
    /* msgh_size passed as argument */
    InP->Head.msgh_request_port = _port;
    InP->Head.msgh_reply_port = MACH_PORT_NULL;
    InP->Head.msgh_id = 79;

    __BeforeSendSimple(79, "consume_send_once_right")
    msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    __AfterSendSimple(79, "consume_send_once_right")
    return msg_result;
}

#endif /* HAVE_MACH */
