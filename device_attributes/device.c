#include <linux/module.h>
#include <linux/fs.h>
#include<linux/cdev.h>
#include <linux/device.h>
#include <linux/device/class.h>

loff_t my_lseek (struct file *, loff_t, int)
{
    return 0;
}

ssize_t my_read (struct file *, char __user *, size_t, loff_t *)
{
    return 0;
}

ssize_t my_write (struct file *, const char __user *, size_t, loff_t *)
{
    return -EPERM;
}

int my_open (struct inode *, struct file *)
{
    pr_alert("open working...\n");
    return 0;
}

int my_release (struct inode *, struct file *)
{
    pr_alert("release working...\n");
    return 0;
}

struct cdev my_char_dev;
dev_t my_dev_num;
struct class * my_class;
struct device * my_device;

const struct file_operations my_def_f_ops = {
    .llseek = my_lseek,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release
};


	ssize_t my_show_attr(struct device *dev, struct device_attribute *attr, char *buf)
    {
        /*just print string*/
        sprintf(buf, "HELLO FROM THE ATTRIBUTE\n");
        return strlen("HELLO FROM THE ATTRIBUTE\n");
    }

    /*
    // will be done
    DEVICE_ATTR(my_aux_attribute, S_IRUGO, my_show_attr, NULL);
    */


    struct device_attribute my_dev_attr = 
    {
        .attr = {
            .mode = S_IRUGO,
            .name = "my_aux_attribute"
        },
        .show = my_show_attr,
        .store = NULL
    };

    int test;

static int __init mod_start(void)
{
    int ret;
    ret = alloc_chrdev_region(&my_dev_num,0, 1, "my_char_dev");

    if(ret < 0)
    {
        goto end;
    }
    
    cdev_init(&my_char_dev, &my_def_f_ops);

    my_char_dev.owner=THIS_MODULE;

    ret = cdev_add(&my_char_dev, my_dev_num, 1);
   
    if(ret < 0)
    {
        goto unregister_char_dev;
    }

    my_class = class_create(THIS_MODULE, "my_char_dev");

    if(IS_ERR(my_class))
    {
        pr_err("Class creation failed \n");
        ret = PTR_ERR(my_class);
        goto cdev_destroy;
    }

    my_device = device_create(my_class, NULL, my_dev_num, NULL, "pcd");
    if(IS_ERR(my_device))
    {
        pr_err("Device registration failed\n");
        ret = PTR_ERR(my_device);
        goto class_destroy;
    }
    test = sysfs_create_file((struct kobject*)&my_device->kobj ,&my_dev_attr.attr);
    
    if(test)
    {   
        /*error with creating ATTR*/
        device_destroy(my_class, my_dev_num);
    }

    pr_err("Hello world from the module\n");
    return 0;

    class_destroy:
        class_destroy(my_class);
    cdev_destroy:
        cdev_del(&my_char_dev);
    unregister_char_dev:
        unregister_chrdev_region(my_dev_num, 1);
    end:  
        pr_err("Init module failed\n"); 
        return ret;

}

static void __exit mod_stop(void)
{
    device_destroy(my_class, my_dev_num);
    class_destroy(my_class);
    cdev_del(&my_char_dev);
    unregister_chrdev_region(my_dev_num, 1);
    pr_err("BYE from the module\n");
}

module_init(mod_start);
module_exit(mod_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("taktopanie");
MODULE_DESCRIPTION("SIMPLE HELLO WORLD MODULE");
MODULE_INFO(board, "RPi");