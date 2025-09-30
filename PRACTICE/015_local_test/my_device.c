
#include<linux/module.h>
#include<linux/device/driver.h>
#include <linux/platform_device.h>

void pcdev_release(struct device *dev)
{
	pr_info("Device released \n");
}

struct platform_device platform_pcdev_1 = {
	.name = "testo_driver",
	.id = 0,
	.dev = {
		.release = pcdev_release
	}
};


static int __init pcdev_platform_init(void)
{
	/* register n platform devices */
	platform_device_register(&platform_pcdev_1);

	pr_info("Device setup module loaded \n");

	return 0;
}


static void __exit pcdev_platform_exit(void)
{

	platform_device_unregister(&platform_pcdev_1);
	pr_info("Device setup module unloaded \n");


}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which registers n platform devices ");
MODULE_AUTHOR("Taktopanie");