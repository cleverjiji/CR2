
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <unistd.h>

#include "netlink.h"
#include "utility.h"

#define NETLINK_USER 31


struct sockaddr_nl NetLink::src_addr = {0};
struct sockaddr_nl NetLink::dest_addr = {0};
struct nlmsghdr *NetLink::nlh = NULL;
struct iovec NetLink::iov = {0};
int NetLink::sock_fd = -1;
struct msghdr NetLink::msg = {0};

void NetLink::connect_with_lkm()
{
	// 1.open socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
	ASSERT(sock_fd>=0);
	// 2.init src_addr and dest_addr
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */
	// 3.bind sock_fd
	bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	// 4.init hlh
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(MESG_BAG)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(MESG_BAG)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(MESG_BAG));
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
	// 5.init msg
	iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    // 6.connect with lkm
    MESG_BAG mesg = {CONNECT, 0, 0, 0, 0, "Shuffle process is ready!"};
    NetLink::send_mesg(mesg);
}

void NetLink::send_mesg(MESG_BAG mesg)
{
    memcpy(NLMSG_DATA(nlh), &mesg, sizeof(MESG_BAG));
    nlh->nlmsg_pid = getpid();
    BLUE("Sending message to kernel: %s\n", mesg.mesg);
    sendmsg(sock_fd, &msg, 0);
}

void NetLink::send_cv_ready_mesg(BOOL is_cv1, long new_pc)
{
    int cvn_ready = is_cv1 ? CV1_IS_READY : CV2_IS_READY;
    MESG_BAG msg_content = {cvn_ready, 0, new_pc, 0, 0, "Code variant is ready!"};
    send_mesg(msg_content);
}

MESG_BAG NetLink::recv_mesg()
{
	BLUE("Waiting for message from kernel: ");
    recvmsg(sock_fd, &msg, 0);
    BLUE("%s\n", (*(MESG_BAG*)NLMSG_DATA(nlh)).mesg);
    return *(MESG_BAG*)NLMSG_DATA(nlh);
}

void NetLink::recv_init_mesg(PID &protected_id, P_ADDRX &curr_pc, SIZE &cc_offset, SIZE &ss_offset)
{
	BLUE("Waiting for init message from kernel: ");
    recvmsg(sock_fd, &msg, 0);
    MESG_BAG mesg = *(MESG_BAG*)NLMSG_DATA(nlh);
    ASSERT(mesg.connect==P_PROCESS_IS_IN);
    protected_id = (PID)mesg.proctected_procid;
    curr_pc = (P_ADDRX)mesg.new_ip;
    cc_offset = mesg.cc_offset;
    ss_offset = mesg.ss_offset;
    BLUE("%s\n", mesg.mesg);
    return ;
}

BOOL NetLink::recv_cv_request_mesg(P_ADDRX &curr_pc, BOOL &need_cv1)
{
    BLUE("Waiting for cv request message from kernel: ");
    recvmsg(sock_fd, &msg, 0);
    MESG_BAG mesg = *(MESG_BAG*)NLMSG_DATA(nlh);
    BLUE("%s\n", mesg.mesg);
    switch(mesg.connect){
        case P_PROCESS_IS_OUT:
            return false;
        case CURR_IS_CV1_NEED_CV2:
            curr_pc = (P_ADDRX)mesg.new_ip;
            need_cv1 = false;
            return true;
        case CURR_IS_CV2_NEED_CV1:
            curr_pc = (P_ADDRX)mesg.new_ip;
            need_cv1 = true;
            return true;
        default:
            ASSERT(0);
            return false;
    }
}

void NetLink::disconnect_with_lkm()
{
    MESG_BAG out_mesg = {DISCONNECT, 0, 0, 0, 0, "Shuffle process is exit!"};
    send_mesg(out_mesg);
	free(nlh);
    close(sock_fd);
}

