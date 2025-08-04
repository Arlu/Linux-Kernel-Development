#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#define NETLINK_LAB03 31

struct lab03_msg
{
    int type;
    int data;
    char text[64];
};

static struct sock *nl_sock = NULL;

static void netlink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
    struct lab03_msg *msg = (struct lab03_msg *)nlmsg_data(nlh);
    int pid = nlh->nlmsg_pid;
    
    pr_info("Received from PID %d: type=%d, data=%d, text='%s'\n",
            pid, msg->type, msg->data, msg->text);
    
    // Echo back with modified data:
    msg->data += 100;
    strcpy(msg->text, "Response from kernel");
    
    struct sk_buff *skb_out = nlmsg_new(sizeof(struct lab03_msg), GFP_KERNEL);
    if (!skb_out) return;
    
    struct nlmsghdr *nlh_out = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, sizeof(struct lab03_msg), 0);
    memcpy(nlmsg_data(nlh_out), msg, sizeof(struct lab03_msg));
    
    netlink_unicast(nl_sock, skb_out, pid, MSG_DONTWAIT);
}

static int __init netlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv_msg,
    };
    
    nl_sock = netlink_kernel_create(&init_net, NETLINK_LAB03, &cfg);
    if (!nl_sock)
    {
        pr_err("Failed to create netlink socket\n");
        return -ENOMEM;
    }
    
    pr_info("Netlink module loaded (protocol %d)\n", NETLINK_LAB03);
    return 0;
}

static void __exit netlink_exit(void)
{
    if (nl_sock)
        netlink_kernel_release(nl_sock);
    pr_info("Netlink module unloaded\n");
}

module_init(netlink_init);
module_exit(netlink_exit);

MODULE_LICENSE("GPL");