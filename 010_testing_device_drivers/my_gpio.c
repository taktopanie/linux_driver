#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/gpio.h>

#include <linux/of.h>
#include <linux/gpio/consumer.h>

#define NO_OF_DEV   2
#define MAX_BUFF    200


char dev_buffer[NO_OF_DEV][MAX_BUFF];

struct gpio_priv_data
{
    dev_t dev_num; 
    struct gpio_desc* my_gpio;
    struct device * my_device;
    struct class * my_class;
    struct cdev dev_cdev;
};

struct gpio_priv_data GPIO_data;

struct dev_data {
    const char * serial_number;
    char * buffer;
    struct cdev dev_cdev;
};

struct driver_data {  
    dev_t dev_num;  
    int total_dev;
    struct class * my_class;
    struct device * my_device;
    struct dev_data dev_data[NO_OF_DEV];
};

struct driver_data my_data =
{
    .total_dev = NO_OF_DEV,
    .dev_data = 
    {
        {
            .serial_number = "DEV001",
            .buffer = dev_buffer[0]
        },
        {
            .serial_number = "DEV002",  
            .buffer = dev_buffer[1]
        }
    }
};

loff_t my_dev_lseek (struct file *, loff_t, int);
ssize_t my_dev_read (struct file *, char __user *, size_t, loff_t *);
ssize_t my_dev_write (struct file *, const char __user *, size_t, loff_t *);
int my_dev_open (struct inode *, struct file *);
int my_dev_release (struct inode *, struct file *);

loff_t my_dev_lseek (struct file *filp, loff_t l_pos, int whence)
{
    int temp;

    switch(whence)
    {
        case SEEK_SET:
            if(l_pos > MAX_BUFF || l_pos < 0)
                return -EINVAL;
            filp->f_pos = l_pos;
            break;

        case SEEK_CUR:
            temp = (filp->f_pos + l_pos);
            if(temp > MAX_BUFF || temp < 0)
                return -EINVAL;
            filp->f_pos += l_pos;
            break;

        case SEEK_END:
            /* not supported */
            return -EINVAL;
            break;
        
        default:
            return -EINVAL;
    }

    return filp->f_pos;

}

ssize_t my_dev_read (struct file *filp, char __user *buff, size_t count, loff_t *l_pos)
{
    struct dev_data* my_dat = filp->private_data;

    if(*l_pos + count > MAX_BUFF)
    {
        count = MAX_BUFF - *l_pos;
    }
    

    if(copy_to_user(buff, my_dat->buffer, count))
    {
        return -EFAULT;
    }

    *l_pos += count;

    return count;
}

ssize_t my_dev_write (struct file *filp, const char __user *buff, size_t count, loff_t *l_pos)
{
    struct dev_data* my_dat = filp->private_data;

    if(*l_pos + count > MAX_BUFF)
    {
        count = MAX_BUFF - *l_pos;
    }

    if(!count)
    {
        pr_alert("Not enought memory...");
        return -ENOMEM;
    }

    if(copy_from_user(my_dat->buffer + *l_pos, buff, count))
    {
        return -EFAULT;
    }

    *l_pos += count;

    return count;
}

int my_dev_open (struct inode *inode , struct file *filp)
{
    struct dev_data* my_dt;

    my_dt = container_of(inode->i_cdev, struct dev_data, dev_cdev);
    
    filp->private_data = my_dt;

    pr_info("Device %s opened...\n", my_dt->serial_number);
    
    /* GPIO ON <<<< OLD METHOD GPIO HANDLING */  
    if(gpio_is_valid(539) == false)
    {
        pr_info("PIN 27 invalid\n");
    }else
    {
        if(gpio_request(539, "gpio_LED") < 0)
        {
            /*pr_info("PIN 27 cannot be used\n");*/
        }else
        {
            pr_info("Setting gpio 27 <ON> (RPI 539)\n");
            gpio_direction_output(539, 0);
            gpio_set_value(539, 1);
            gpio_free(539); 
        }
    }

    return 0;
}

int my_dev_release (struct inode *inode, struct file *filp)
{
    /* GPIO OFF <<<< OLD METHOD GPIO HANDLING */   
    if(gpio_is_valid(539) == false)
    {
        pr_info("PIN 27 invalid\n");
    }else
    {
        if(gpio_request(539, "gpio_LED") < 0)
        {
            /*pr_info("PIN 27 cannot be used\n");*/
            
        }else
        {
            pr_info("Setting gpio 27 <OFF> (RPI 539)\n");
            gpio_direction_output(539, 0);
            gpio_set_value(539, 1);
            gpio_free(539);
        }
    }

    pr_info("Device released\n");
 
    return 0;
}

struct file_operations f_ops = 
{
    .llseek = my_dev_lseek,
    .read = my_dev_read,
    .write = my_dev_write,
    .open = my_dev_open,
    .release = my_dev_release
};

struct file_operations gpio_ops = 
{
    /* TODO: ioctl will be done */
};

static int __init my_driver_init(void)
{
    int ret;
    int i;
    const char * name;

    struct device_node* my_dev_par;
    struct device_node* my_dev_child;
    const char * node_name = "my_gpios";

    /* allocate device numbers */
    ret = alloc_chrdev_region(&my_data.dev_num, 0, NO_OF_DEV, "my_devices");

    if(ret)
    {
        pr_info("Cannot allocate device number\n");
        goto out;
    }

    /* create class under /sys/class */
    my_data.my_class = class_create("my_test_devices");

    if(IS_ERR(my_data.my_class))
    {
        pr_info("Class creation failed\n");
        ret = PTR_ERR(my_data.my_class);
        goto class_fail;
    }
   
    for (i = 0; i < NO_OF_DEV; i++)
    {
        pr_info("Device <major>:<minor>: %d:%d\n", MAJOR(my_data.dev_num+i), MINOR(my_data.dev_num+i));
         
        /* Create chardev */
        cdev_init(&my_data.dev_data[i].dev_cdev, &f_ops);
        my_data.dev_data[i].dev_cdev.owner = THIS_MODULE;

        ret = cdev_add(&my_data.dev_data[i].dev_cdev, my_data.dev_num + i, 1);
        if(ret)
        {
            pr_info("cdev add failed\n");
            goto cdev_fail;
        }

        /* Create devices under /dev/ */
        my_data.my_device = device_create(my_data.my_class, NULL, my_data.dev_num + i, NULL, "my_dev-%d", i);
        if(IS_ERR(my_data.my_device))
        {   
            pr_info("device_create failed\n");
            ret = PTR_ERR(my_data.my_device);
            goto dev_err;
        }
    }

    my_dev_par = of_find_node_by_name(NULL, node_name);
    
    /*if device node exists*/
    if(my_dev_par)
    {

        of_node_put(my_dev_par);

        if(of_property_read_string(my_dev_par, "compatible", &name))
        {
            /* error */
            pr_err("wrong value\n");
            return -EINVAL;
        }


        my_dev_child = of_get_next_child(my_dev_par, NULL);


        printk("\"%s\" node has %d child, Fetched \"compatible\" data: %s\n",node_name ,of_get_available_child_count(my_dev_par),name);
        of_property_read_string(my_dev_child, "label", &name);
        printk("Child fetched \"label\" data: %s\n", name);

        /*create GPIO desc for my_device -> will be edited*/
        GPIO_data.my_gpio = devm_fwnode_gpiod_get(my_data.my_device, &my_dev_child->fwnode, "RPi",GPIOD_ASIS, "my_gpio");

        if(IS_ERR(GPIO_data.my_gpio))
        {
            pr_err("Could not create GPIO0\n");
            return (PTR_ERR(GPIO_data.my_gpio ));
        }

        /* allocate device number */
        ret = alloc_chrdev_region(&GPIO_data.dev_num, 2, 1, "my_devices");
        pr_info("Device <major>:<minor>: %d:%d\n", MAJOR(GPIO_data.dev_num), MINOR(GPIO_data.dev_num));
        if(ret)
        {
            pr_info("Cannot allocate device number\n");
            goto gpio_out;
        }

        cdev_init(&GPIO_data.dev_cdev, &gpio_ops);
        GPIO_data.dev_cdev.owner = THIS_MODULE;

        ret = cdev_add(&GPIO_data.dev_cdev, GPIO_data.dev_num, 1);

        if(ret)
        {
            pr_info("GPIO cdev add failed\n");
            goto GPIO_cdev_fail;
        }

        GPIO_data.my_class = my_data.my_class;

        /* Create devices for GPIO but not under /dev/ */
        GPIO_data.my_device = device_create(my_data.my_class, NULL, GPIO_data.dev_num, NULL, "my_gpio-0");
        if(IS_ERR(GPIO_data.my_device))
        {   
            pr_info("device_create failed\n");
            ret = PTR_ERR(GPIO_data.my_device);
            goto gpio_dev_err;
        }

        gpiod_direction_output(GPIO_data.my_gpio , 0);
        gpiod_set_value(GPIO_data.my_gpio , 1);
        pr_info("LED turned ON\n");
        
        goto gpio_out;

        gpio_dev_err:
        GPIO_cdev_fail:
            device_destroy(GPIO_data.my_class, GPIO_data.dev_num);
            unregister_chrdev_region(GPIO_data.dev_num, 1);
        gpio_out:
            /*nothing to be done*/
    }

    printk("Module inserted OK\n");
    return 0;
    
    dev_err:
    cdev_fail:
        for(i = 0; i < NO_OF_DEV; i++)
        {
            device_destroy(my_data.my_class, my_data.dev_num+i);
            cdev_del(&my_data.dev_data[i].dev_cdev);
        }

        class_destroy(my_data.my_class);
    class_fail:
        unregister_chrdev_region(my_data.dev_num, NO_OF_DEV);
    out:
        return ret;
};

static void __exit my_driver_exit(void)
{
    int i;
    for(i = 0; i < NO_OF_DEV; i++)
    {
        device_destroy(my_data.my_class, my_data.dev_num+i);
        cdev_del(&my_data.dev_data[i].dev_cdev);
    }

    gpiod_set_value(GPIO_data.my_gpio , 0);

    /* destroy GPIO */
    device_destroy(GPIO_data.my_class, GPIO_data.dev_num);
    cdev_del(&GPIO_data.dev_cdev);
    unregister_chrdev_region(GPIO_data.dev_num, 1);

    /* destroy class and CHAR devs */
    class_destroy(my_data.my_class);
    unregister_chrdev_region(my_data.dev_num, NO_OF_DEV);

    
    pr_info("LED turned OFF\n");
    printk("Module removed OK\n");
};

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Taktopanie");