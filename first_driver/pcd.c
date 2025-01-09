#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

#define DEV_MEM_SIZE 512

dev_t device_number;
struct cdev pcd_cdev;

loff_t pcd_lseek (struct file * filp, loff_t offset, int whence)
{
    pr_info("Module lseek called\n");
    return 0;
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Module read called for %zu bytes\n", count);
    return 0;
}

ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Module write called for %zu bytes\n", count);
    return 0;
}

int pcd_open (struct inode *inode, struct file *filp)
{
    pr_info("Module open called successful\n");
    return 0;
}

int pcd_release (struct inode *inode, struct file *filp)
{
    pr_info("Module release called successful\n");
    return 0;
}

struct file_operations pcd_fops = 
{
    .open = pcd_open,
    .release = pcd_release,
    .write = pcd_write,
    .read = pcd_read,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE
};

struct class *class_pcd;

struct device *device_pcd;


static int __init pcd_driver_init(void)
{
    //dynamically allocate a device number
    alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
    
    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    //Initialize cdev struct with fops
    cdev_init(&pcd_cdev, &pcd_fops);

    //register a device (cdev structure) with VFS
    pcd_cdev.owner = THIS_MODULE;
    cdev_add(&pcd_cdev, device_number, 1);

    class_pcd = class_create(THIS_MODULE, "pcd_class");

    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

    pr_info("Module init successfull.\n");

	return 0;
}


static void __exit pcd_driver_exit(void)
{
    device_destroy(class_pcd, device_number);

    class_destroy(class_pcd);

    cdev_del(&pcd_cdev);

    unregister_chrdev_region(device_number, 1);

    pr_info("Module unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAKTOPANIE");
MODULE_DESCRIPTION("FIRST DRIVER MODULE");
MODULE_INFO(board, "RPi");

