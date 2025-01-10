#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

#define DEV_1_MEM_SIZE 1024
#define DEV_2_MEM_SIZE 512
#define DEV_3_MEM_SIZE 1024
#define DEV_4_MEM_SIZE 512

#define NO_OF_DEVICES 4

/* DEVICE memory */

char device_buffer_1[DEV_1_MEM_SIZE];
char device_buffer_2[DEV_2_MEM_SIZE];
char device_buffer_3[DEV_3_MEM_SIZE];
char device_buffer_4[DEV_4_MEM_SIZE];

/* DEVICE private data */

struct pcdev_private_data
{
    char *buffer;
    unsigned size;
    const char *serial_number;
    int permissions;
    struct cdev cdev;
};

/* private DRIVER structure */
struct pcdrv_private_data
{
    int total_devices;
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcdev_private_data pcdev_data [NO_OF_DEVICES];

};

struct pcdrv_private_data pcdrv_data =
{
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
        [0] = {
            .buffer = device_buffer_1,
            .size = DEV_1_MEM_SIZE,
            .serial_number = "PCDEV23D32",
            .permissions = 0x1 /*RDONLY*/
        },
        [1] = {
            .buffer = device_buffer_2,
            .size = DEV_2_MEM_SIZE,
            .serial_number = "PCDEV23DF2",
            .permissions = 0x1 /*RDONLY*/
        },
        [2] = {
            .buffer = device_buffer_3,
            .size = DEV_3_MEM_SIZE,
            .serial_number = "PCDEV23D33",
            .permissions = 0x1 /*RDONLY*/
        },
        [3] = {
            .buffer = device_buffer_4,
            .size = DEV_4_MEM_SIZE,
            .serial_number = "PCDEV23D3F",
            .permissions = 0x1 /*RDONLY*/
        }
        
    }
};

loff_t pcd_lseek (struct file * filp, loff_t offset, int whence)
{
    #if 0
	loff_t temp;

	pr_info("lseek requested \n");
	pr_info("Current value of the file position = %lld\n",filp->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
				return -EINVAL;
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	
	pr_info("New value of the file position = %lld\n",filp->f_pos);

	return filp->f_pos;
    #endif
   return -ENOMEM;
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    #if 0
    pr_info("Module read called for %zu bytes\n", count);
    pr_info("Current file pos: %lld\n", *f_pos);

    /* adjust count */
    if((*f_pos + count) > DEV_MEM_SIZE)
        count = DEV_MEM_SIZE - *f_pos;

    /* copy to user */
    if(copy_to_user(buff, &device_buffer[*f_pos], count))
    {
        return -EFAULT;
    }

    /* update curr file position */
    *f_pos += count;

    pr_info("Number of bytes succesfully read: %zu\n", count );
    pr_info("Updated f_pos: %lld\n", *f_pos );

    return count;
    #endif
    return 0;
}

ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    #if 0
    pr_info("Module write called for %zu bytes\n", count);
    pr_info("Current file pos: %lld\n", *f_pos);

    /* adjust count */
    if((*f_pos + count) > DEV_MEM_SIZE)
        count = DEV_MEM_SIZE - *f_pos;

    if(!count)
    {
        pr_alert("Not enought memory size..\n");
        return -ENOMEM;
    }
    
    /* copy from user */
    if(copy_from_user(&device_buffer[*f_pos], buff, count))
    {
        return -EFAULT;
    }

    /* update curr file position */
    *f_pos += count;

    pr_info("Number of bytes succesfully written: %zu\n", count );
    pr_info("Updated f_pos: %lld\n", *f_pos );

    return count;
    #endif
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


static int __init pcd_driver_init(void)
{
    #if 0
    int ret; 
    /* 1. dynamically allocate a device number */
    ret = alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
    
    if(ret < 0)
        goto out;

    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    /* 2. Initialize cdev struct with fops */
    cdev_init(&pcd_cdev, &pcd_fops);

    /* 3. register a device (cdev structure) with VFS */
    pcd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&pcd_cdev, device_number, 1);

    if(ret < 0)
        goto unreg_chardev;
    /* 4. create device class under /sys/class/ */
    class_pcd = class_create(THIS_MODULE, "pcd_class");

    if(IS_ERR(class_pcd))
    {
        pr_err("Class creation failed\n");
        ret = PTR_ERR(class_pcd);
        goto cdev_del;
    }

    /* populate the sysds with device info */
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
    if(IS_ERR(device_pcd))
    {
        pr_err("Device creation failed\n");
        ret = PTR_ERR(class_pcd);
        goto class_del;
    }
    pr_info("Module init successfull.\n");

	return 0;

class_del:
    class_destroy(class_pcd);
cdev_del:
    cdev_del(&pcd_cdev);
unreg_chardev:
    unregister_chrdev_region(device_number, 1);
out:
    pr_info("Module insert fail\n");
    return ret;
    #endif
    return 0;
}


static void __exit pcd_driver_exit(void)
{
    #if 0
    device_destroy(class_pcd, device_number);

    class_destroy(class_pcd);

    cdev_del(&pcd_cdev);

    unregister_chrdev_region(device_number, 1);

    pr_info("Module unloaded\n");
    #endif
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAKTOPANIE");
MODULE_DESCRIPTION("FIRST DRIVER MODULE");
MODULE_INFO(board, "RPi");

