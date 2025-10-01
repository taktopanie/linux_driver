#include <linux/module.h>
#include <linux/device.h>
#include<linux/device/driver.h>
#include <linux/platform_device.h>
#include "common.h"
#include <linux/fs.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

dev_t cdev;

int testo_probe (struct platform_device *dev)
{

	struct my_device_data* plt_data;
	
	int res;

	plt_data = (struct my_device_data *)dev_get_platdata(&dev->dev);

	res = register_chrdev_region(cdev,1, "maciej_c_dev");


	if(!plt_data)
	{
		pr_alert("no platform data provided...\n");
		return -EINVAL;
	}

	dev_set_drvdata(&dev->dev, plt_data);

	pr_info("Device serial number = %d\n",plt_data->serial_number);
	pr_info("Device text = %s\n", plt_data->text);

	pr_err("PROBE DONE!!!\n");
	return 0;
}

int testo_remove(struct platform_device * dev)
{
	struct my_device_data* plt_data = (struct my_device_data*)dev->dev.driver_data;
	pr_info("Device %d, will be removed...\n",plt_data->serial_number);
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
