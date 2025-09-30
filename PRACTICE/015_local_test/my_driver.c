#include <linux/module.h>
#include <linux/device.h>
#include<linux/device/driver.h>
#include <linux/platform_device.h>

int testo_probe (struct platform_device *dev)
{
	pr_err("PROBE!!!\n");
	return 0;
}

static const struct device_driver testo_driver = {
	   .owner		= THIS_MODULE,
       .name		= "testo_driver"
};

static struct platform_driver testo_platform_driver = {
	.probe			= &testo_probe,
	.driver 		= testo_driver
};

static int __init helloworld_init(void)
{
	int ret;

	pr_err("Hello world from the module\n");
	
	ret = platform_driver_register(&testo_platform_driver);
	
	if(ret == -EBUSY || ret == -EINVAL)
	{
		pr_err("ERROR DURING REGISTERING DRIVER\n");
		return -EINVAL;
	}
	
	return 0;
}

static void __exit helloworld_exit(void)
{
	platform_driver_unregister(&testo_platform_driver);
	pr_err("Bye cruel world from the module\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAKTOPANIE");
MODULE_DESCRIPTION("SIMPLE HELLO WORLD MODULE");
MODULE_INFO(board, "RPi");
