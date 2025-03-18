#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/of.h>
#include<linux/of_device.h>

#include <linux/gpio/consumer.h>
#include<linux/mod_devicetable.h>

#include <linux/interrupt.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

/* driver private data structure */
struct gpiodrv_private_data
{
	int total_devices;
	struct class *class_gpio;
	struct device **dev;
};

struct gpiodrv_private_data gpio_drv_data;

static irqreturn_t my_irq(int irq, void *dev_id)
{
	printk(KERN_ALERT "IRQ CALLED\n");
	return IRQ_HANDLED;
}

int gpio_sysfs_probe(struct platform_device *pdev)
{
	int ret;
	struct device * dev = &pdev->dev;
	struct device_node * parent = pdev->dev.of_node;

	//count the child nodes
	gpio_drv_data.total_devices = of_get_child_count(parent);

	ret = devm_request_irq(dev, platform_get_irq(pdev, 0),
			       my_irq, IRQF_NO_SUSPEND, dev_name(dev),
			       pdev);
	if (ret) {
		dev_err(dev, "Failed to register a GPIO IRQ handler: %d\n",
			ret);
		return -ENODEV;
	}

	dev_info(&pdev->dev,"Module inserted succesfully\n");
	return 0;
}

int gpio_sysfs_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev,"Module remove called\n");
	return 0;

}

struct of_device_id gpio_device_match [] = 
{
	{.compatible = "org,taktopanie_irq"},
	{}
};

struct platform_driver gpiosysfs_platform_driver =
{
	.probe = gpio_sysfs_probe,
	.remove = gpio_sysfs_remove,
	.driver = {
		.name = "takto-gpio-sysfs",
		.of_match_table = of_match_ptr(gpio_device_match)
	}
};

int __init gpio_sysfs_init(void)
{
	gpio_drv_data.class_gpio = class_create("taktopanie_irq_gpios");
	if(IS_ERR(gpio_drv_data.class_gpio))
	{
		pr_err("Error in creating class\n");
		return PTR_ERR(gpio_drv_data.class_gpio);
	}

	platform_driver_register(&gpiosysfs_platform_driver);
	return 0;

}

void __exit gpio_sysfs_exit(void)
{
	platform_driver_unregister(&gpiosysfs_platform_driver);
	class_destroy(gpio_drv_data.class_gpio);
}

module_init(gpio_sysfs_init);
module_exit(gpio_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("taktopanie");
MODULE_DESCRIPTION("Simple GPIO IRQ devicetree handling driver");

