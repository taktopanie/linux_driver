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

#define RDONLY  0x01
#define WRONLY  0x10
#define RDWR    0x11

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
            .permissions = RDONLY
        },
        [1] = {
            .buffer = device_buffer_2,
            .size = DEV_2_MEM_SIZE,
            .serial_number = "PCDEV23DF2",
            .permissions = WRONLY 
        },
        [2] = {
            .buffer = device_buffer_3,
            .size = DEV_3_MEM_SIZE,
            .serial_number = "PCDEV23D33",
            .permissions = RDWR
        },
        [3] = {
            .buffer = device_buffer_4,
            .size = DEV_4_MEM_SIZE,
            .serial_number = "PCDEV23D3F",
            .permissions = RDWR
        }
        
    }
};

loff_t pcd_lseek (struct file * filp, loff_t offset, int whence)
{

    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *) filp->private_data;

    int max_data_size = pcdev_data->size;

	loff_t temp;

	pr_info("lseek requested \n");
	pr_info("Current value of the file position = %lld\n",filp->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > max_data_size) || (offset < 0))
				return -EINVAL;
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > max_data_size) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = max_data_size + offset;
			if((temp > max_data_size) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	
	pr_info("New value of the file position = %lld\n",filp->f_pos);

	return filp->f_pos;

   return -ENOMEM;
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{

    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *) filp->private_data;

    int max_data_size = pcdev_data->size;

    pr_info("Module read called for %zu bytes\n", count);
    pr_info("Current file pos: %lld\n", *f_pos);

    /* adjust count */
    if((*f_pos + count) > max_data_size)
        count = max_data_size - *f_pos;

    /* copy to user */
    if(copy_to_user(buff, pcdev_data->buffer+(*f_pos), count))
    {
        return -EFAULT;
    }

    /* update curr file position */
    *f_pos += count;

    pr_info("Number of bytes succesfully read: %zu\n", count );
    pr_info("Updated f_pos: %lld\n", *f_pos );

    return count;
}

ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *) filp->private_data;

    int max_data_size = pcdev_data->size;

    pr_info("Momadule write called for %zu bytes\n", count);
    pr_info("Current file pos: %lld\n", *f_pos);

    /* adjust count */
    if((*f_pos + count) > max_data_size)
        count = max_data_size - *f_pos;

    if(!count)
    {
        pr_alert("Not enought memory size..\n");
        return -ENOMEM;
    }
    
    /* copy from user */
    if(copy_from_user(pcdev_data->buffer+(*f_pos), buff, count))
    {
        return -EFAULT;
    }

    /* update curr file position */
    *f_pos += count;

    pr_info("Number of bytes succesfully written: %zu\n", count );
    pr_info("Updated f_pos: %lld\n", *f_pos );

    return count;
}



int check_permission(int dev_perm, int acc_mode)
{

    if(dev_perm == RDWR)
        return 0;

    if((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE))) 
        return 0;

    if((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ))) 
        return 0;

    return -EPERM;

    return 0;
}

int pcd_open (struct inode *inode, struct file *filp)
{
    /* find out which device file is open */

    int minor_n;
    int ret;

    struct pcdev_private_data *pcdev_data;

    minor_n = MINOR(inode->i_rdev);
    pr_info("minor_access = %d\n", minor_n);

    /* get device private data structure */
    pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, cdev);

    /* store pcdev_data for other methods */
    filp -> private_data = pcdev_data;

    /* checks permission */
    ret = check_permission(pcdev_data->permissions, filp->f_mode);

    (!ret)?pr_info("open was successful\n"):pr_info("open was unccessful\n");

    return ret;
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
 
    int ret; 
    int i;
    /* 1. dynamically allocate a device number */
    ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd_devices");
    
    if(ret < 0)
        goto out;

    /* 2. create device class under /sys/class/ */
    pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");

    if(IS_ERR(pcdrv_data.class_pcd))
    {
        pr_err("Class creation failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        goto unreg_chardev;
    }

    for(i = 0 ; i < NO_OF_DEVICES ; i++)
    {
        pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pcdrv_data.device_number + i), MINOR(pcdrv_data.device_number + i));

        /* 3. Initialize cdev struct with fops */
        cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

        /* 4. register a device (cdev structure) with VFS */
        pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;
        ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number + i, 1);

        if(ret < 0)
            goto cdev_del;

        /* populate the sysds with device info */
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number+i, NULL, "pcdev-%d",i);
        if(IS_ERR(pcdrv_data.device_pcd))
        {
            pr_err("Device creation failed\n");
            ret = PTR_ERR(pcdrv_data.class_pcd);
            goto class_del;
        }
        //pcdrv_data.device_pcd->type->name = "taktopanie";
        //pr_info("dev: %s created.\n", pcdrv_data.device_pcd->type->name);

    }

    pr_info("Module init successfull.\n");

	return 0;


cdev_del:
class_del:
    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }
    class_destroy(pcdrv_data.class_pcd);
unreg_chardev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
out:
    pr_info("Module insert fail\n");
    return ret;
}


static void __exit pcd_driver_exit(void)
{
    int i;

    for(i = 0; i < NO_OF_DEVICES; i++)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }

    class_destroy(pcdrv_data.class_pcd);

    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
    pr_info("Module unloaded\n");
}


module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAKTOPANIE");
MODULE_DESCRIPTION("FIRST DRIVER MODULE");
MODULE_INFO(board, "RPi");

