#include <linux/module.h>
#include <linux/fs.h>
#include<linux/cdev.h>
#include <linux/device.h>
#include <linux/device/class.h>

//#include <sys/types.h>
//#include <unistd.h>

/*Function prototypes*/
loff_t my_lseek (struct file *, loff_t, int);
ssize_t my_read (struct file *, char __user *, size_t, loff_t *);
ssize_t my_write (struct file *, const char __user *, size_t, loff_t *);
int my_open (struct inode *, struct file *);
int my_release (struct inode *, struct file *);
ssize_t my_show_attr(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t my_show_serial(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t my_store_attr(struct device *dev, struct device_attribute *attr,const char *buf, size_t count);
static int __init mod_start(void);
static void __exit mod_stop(void);

//device memory MAX size
#define DEV_MEM_SIZE    500
char device_buff [DEV_MEM_SIZE];

loff_t my_lseek (struct file * fd , loff_t offset, int whence)
{
    switch(whence)
    {
        case SEEK_SET:
            if((offset > DEV_MEM_SIZE)||(offset < 0))
            {
                return -EINVAL;
            }
            fd->f_pos = offset;
            break;

        case SEEK_CUR:
            if(fd->f_pos + offset > DEV_MEM_SIZE)
            {
                return -ENOMEM;
            }
            if(offset < 0)
            {
                return -EINVAL;
            }
            fd->f_pos += offset;
            break;

        case SEEK_END:
            if(offset + DEV_MEM_SIZE > DEV_MEM_SIZE)
            {
                /* SEEK END NOT SUPPORTED */
                return -ENOMEM;
            }
            if(offset < 0)
            {
                return -EINVAL;
            }
            break;
        default:
            return -EINVAL;
    }
    pr_info("File pointer updated with: %lld value\n", fd->f_pos );
    return fd->f_pos;
}

ssize_t my_read (struct file *fd, char __user * buff, size_t count, loff_t * pos_pt)
{
    pr_info("Read from the device requested for: %ld bytes\n", count);

    if(*pos_pt + count > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *pos_pt;   
    }


    if(copy_to_user((void *) buff, &device_buff[*pos_pt], count))
    {  
        return -EFAULT;
    }

    *pos_pt += count;

    pr_info("Data readed from the device: %ld\n", count);

    return count;

}

ssize_t my_write (struct file *fd, const char __user * buff, size_t count, loff_t * pos_pt)
{   
    if(*pos_pt + count > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *pos_pt;   
    }

    if(!count)
    {
        pr_info("Not enought memory\n");
        return -ENOMEM;
    }

    if(copy_from_user(&device_buff[*pos_pt],(const void *) buff, count))
    {  
        return -EFAULT;
    }

    *pos_pt += count;

    pr_info("Data written to the device: %ld\n", count);

    return count;
    
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
        return sprintf(buf, "HELLO FROM THE ATTRIBUTE\n");
    }

    ssize_t my_show_serial(struct device *dev, struct device_attribute *attr, char *buf)
    {
        /*just print string*/
        return sprintf(buf, "AXF123\n");
    }


	ssize_t my_store_attr(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
    {
        /*just get the string and convert it to long*/
        long result;
        int ret;
        ret = kstrtol(buf, 10, &result);
        
        if(ret)
        {
            return ret;
        }

        printk("Got the information from the user: %ld\n", result);

        return count;
    }

    
    /*first method attribute creating*/
    DEVICE_ATTR(serial_number, S_IRUGO, my_show_serial, NULL);
    

    /*second method attribute creating*/
    struct device_attribute my_dev_attr = 
    {
        .attr = {
            .mode = (S_IRUGO | S_IWUSR),
            .name = "my_aux_attribute"
        },
        .show = my_show_attr,
        .store = my_store_attr
    };

    int test;

    struct attribute * my_attr_list[] = 
    {
        &my_dev_attr.attr,
        &dev_attr_serial_number.attr,
        NULL
    };

    struct attribute_group my_attr_grp = 
    {
        .name = "my_attr_group",
        .attrs = my_attr_list
    };

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
    
    /*test = sysfs_create_file((struct kobject*)&my_device->kobj ,&my_dev_attr.attr);*/

    test = sysfs_create_group((struct kobject*)&my_device->kobj,&my_attr_grp);
    
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