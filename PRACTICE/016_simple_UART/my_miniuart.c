#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "rpi_mini_uart"
#define MINI_UART_BASE 0x3F215040
#define MINI_UART_SIZE 0x20

static void __iomem *uart_base;
static int major;

//Timers
static struct timer_list TIMER_1;

char LED_ON_CMD[] = "#1q\n";
char LED_OFF_CMD[] = "#0q\n";

// Mini UART register offsets
#define AUX_MU_IO_REG     0x00
#define AUX_MU_LSR_REG    0x14
#define AUX_MU_LSR_DATA   (1 << 0)
#define AUX_MU_LSR_SPACE  (1 << 5)

#define LED_ON _IO(0x10, 2)
#define LED_OFF _IO(0x10, 3)

//#define DEBUG_PRINT

void UART_hw_write_char(char data_to_send)
{
    while (!(readl(uart_base + AUX_MU_LSR_REG) & AUX_MU_LSR_SPACE))
        cpu_relax();
    writel(data_to_send, uart_base + AUX_MU_IO_REG);
}

static int rpi_uart_open(struct inode *inode, struct file *file)
{
    pr_info("rpi_mini_uart: device opened\n");
    return 0;
}

static int rpi_uart_release(struct inode *inode, struct file *file)
{
    pr_info("rpi_mini_uart: device closed\n");
    return 0;
}

static ssize_t rpi_uart_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    size_t i;
    char c;
    uint32_t data_size = 0;

    for (i = 0; i < count; i++) {
        uint8_t tmp_timer = 0;
        while ((!(readl(uart_base + AUX_MU_LSR_REG) & AUX_MU_LSR_DATA)) && tmp_timer < 50 )
            {
                tmp_timer++;
                cpu_relax();
            }

        if(tmp_timer == 50)
        {
            break;
        }

        c = readl(uart_base + AUX_MU_IO_REG) & 0xFF;
        data_size++;

        if (copy_to_user(buf + i, &c, 1))
            return -EFAULT;

    }
    return data_size;
}

static ssize_t rpi_uart_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    size_t i;
    char c;

    for (i = 0; i < count; i++) {
        if (copy_from_user(&c, buf + i, 1))
            return -EFAULT;

        UART_hw_write_char(c);
    }
    return count;
}

long my_dev_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
#ifdef DEBUG_PRINT
    pr_info("IOCTL CALLED\n");
#endif

    int i = 0;
    switch(cmd) 
    {
        
        case LED_ON:

            for(i = 0; i < strlen(LED_ON_CMD); i++)
            {
                UART_hw_write_char(LED_ON_CMD[i]);
            }

#ifdef DEBUG_PRINT
            pr_info("LED_ON");
#endif
            break;

        case LED_OFF:

            for(i = 0; i < strlen(LED_OFF_CMD); i++)
            {
                UART_hw_write_char(LED_OFF_CMD[i]);
            }
#ifdef DEBUG_PRINT
            pr_info("LED_OFF");
#endif
            break;

        default:
#ifdef DEBUG_PRINT
                pr_info("Default\n");
#endif
                break;
    }

    return 0;

}

/*
    THIS timer is called each 1s to inform that module is cooperating with STM
*/
uint8_t LED_STATE = 0;

void my_timer_callback(struct timer_list * t)
{
#ifdef DEBUG_PRINT
    pr_info("rpi_mini_uart: TIMER_1 expired\n");
#endif
    //LED toggle
    if(LED_STATE)
    {
        for(int i = 0; i < strlen(LED_OFF_CMD); i++)
        {
            UART_hw_write_char(LED_OFF_CMD[i]);
        }
        LED_STATE = 0;
    }else
    {
        for(int i = 0; i < strlen(LED_ON_CMD); i++)
        {
            UART_hw_write_char(LED_ON_CMD[i]);
        }
        LED_STATE = 1;
    }

    //set timer to 1s again
    mod_timer(&TIMER_1, jiffies + msecs_to_jiffies(1000));
}

static struct file_operations rpi_uart_fops = {
    .owner = THIS_MODULE,
    .open = rpi_uart_open,
    .release = rpi_uart_release,
    .read = rpi_uart_read,
    .write = rpi_uart_write,
    .unlocked_ioctl = my_dev_ioctl,
};

static int __init rpi_uart_init(void)
{
    pr_info("rpi_mini_uart: initializing\n");

    //map UART registry to __iomem *uart_base;
    uart_base = ioremap(MINI_UART_BASE, MINI_UART_SIZE);
    if (!uart_base) {
        pr_err("rpi_mini_uart: unable to map UART registers\n");
        return -ENOMEM;
    }

    //make char device
    major = register_chrdev(0, DRIVER_NAME, &rpi_uart_fops);
    if (major < 0) {
        iounmap(uart_base);
        pr_err("rpi_mini_uart: failed to register chrdev\n");
        return major;
    }

    //setup timer TIMER_1 for UART led toggling on STM
    timer_setup(&TIMER_1, my_timer_callback, 0);

    //jiffies(actual time) + 1000 ms = 1s [callback timer]
    mod_timer(&TIMER_1, jiffies + msecs_to_jiffies(1000));

    pr_info("rpi_mini_uart: registered with major %d\n", major);
    return 0;
}

static void __exit rpi_uart_exit(void)
{
    unregister_chrdev(major, DRIVER_NAME);
    iounmap(uart_base);
    pr_info("rpi_mini_uart: exited\n");

    //delete timer
    del_timer_sync(&TIMER_1);
}

module_init(rpi_uart_init);
module_exit(rpi_uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Simple Mini UART driver for RPi3");
