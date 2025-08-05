#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define MCG_LAB03 31

struct lab03_msg
{
    int type;
    int data;
    char text[64];
};

void read_event(int sock)
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct lab03_msg *msg;
    struct msghdr msg_hdr;
    struct iovec iov;
    char buffer[65536];
    
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct lab03_msg)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(struct lab03_msg)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(struct lab03_msg));
    
    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg_hdr, 0, sizeof(msg_hdr));
    msg_hdr.msg_name = (void *)&dest_addr;
    msg_hdr.msg_namelen = sizeof(dest_addr);
    msg_hdr.msg_iov = &iov;
    msg_hdr.msg_iovlen = 1;
  
    printf("Listen for message...\n");
    if (recvmsg(sock, &msg_hdr, 0) >= 0)
    {
        msg = (struct lab03_msg *)NLMSG_DATA(nlh);
        printf("Received: type=%d, data=%d, text='%s'\n", 
               msg->type, msg->data, msg->text);
    }
    
    free(nlh);
}

int main()
{
    int sock_fd, group = MCG_LAB03, rc;
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct lab03_msg *msg;
    struct iovec iov;
    struct msghdr msg_hdr;
    
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
    if (sock_fd < 0)
    {
        perror("socket");
        return -1;
    }
    
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = MCG_LAB03;
    
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct lab03_msg)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(struct lab03_msg)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(struct lab03_msg));
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    
    msg = (struct lab03_msg *)NLMSG_DATA(nlh);
    msg->type = 1;
    msg->data = 42;
    strcpy(msg->text, "Hello from userspace");
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;
    
    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg_hdr, 0, sizeof(msg_hdr));
    msg_hdr.msg_name = (void *)&dest_addr;
    msg_hdr.msg_namelen = sizeof(dest_addr);
    msg_hdr.msg_iov = &iov;
    msg_hdr.msg_iovlen = 1;

    if (setsockopt(sock_fd, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0)
    {
        perror("setsockopt");
        close(sock_fd);
        return -1;
    }
    
    printf("Sending message to kernel...\n");
    rc = sendmsg(sock_fd, &msg_hdr, 0);
    if (rc < 0)
    {
        perror("sendmsg");
        close(sock_fd);
        return -1;
    }

    read_event(sock_fd);
    
    close(sock_fd);
    free(nlh);
    return 0;
}