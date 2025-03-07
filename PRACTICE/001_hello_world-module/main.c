#include <linux/module.h>

static int __init helloworld_init(void)
{
	pr_err("Hello world from the module\n");
	return 0;
}

static void __exit helloworld_exit(void)
{
	pr_err("Bye cruel world from the module\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAKTOPANIE");
MODULE_DESCRIPTION("SIMPLE HELLO WORLD MODULE");
MODULE_INFO(board, "RPi");
