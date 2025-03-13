/*
 * Raspberry PI - SoC GPIO IRQ handling
 *
 * IRQ is called when GPIO_4 - PHYS_PIN_7 [KERNEL GPIO PIN 539]
 *
 */

#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/of_gpio.h>
#include <linux/idr.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/sysrq.h>
#include <linux/syscalls.h>
#include <linux/kthread.h>
#include <linux/gpio.h>

#define RASPBERRYPI_GPIO_IRQ_PIN  539
#define RASPBERRYPI_GPIO_BOOTED 42

static int g_irq = -1;
static struct task_struct *task;
static int need_Print = 0;
static atomic_t IRQ_needed = ATOMIC_INIT(0);
static DECLARE_WAIT_QUEUE_HEAD(probe_waitqueue);
static void raspberrypi_IRQ_print(void);

static int raspberrypi_IRQ_thread(void *arg) {
    while(true)
    {
        /* block the thread and wait for a wakeup.
        wakeup is done in two cases: module unload and reset gpio state change */
        wait_event_interruptible(probe_waitqueue, atomic_read(&IRQ_needed) > 0);   
        
        atomic_set(&IRQ_needed, 0);
        
        printk(KERN_INFO "IRQ THREAD\n");

        /* call the printing function if needed */
        if (need_Print) {
            raspberrypi_IRQ_print();
        }
    }
  return 0;
}

static void raspberrypi_IRQ_print(void)
{
    printk(KERN_ALERT "IRQ CALLED\n");
}

static irqreturn_t raspberrypi_irq_handler(int irq, void *dev_id) {
  /* handled when reset gpio state changed */
  if (irq == g_irq) {

    /* Set the values and wake up IRQ queue */
    need_Print = 1;
    atomic_set(&IRQ_needed, 1);
    wake_up(&probe_waitqueue);

    /* clear the IRQ flags */
    irq_clear_status_flags(irq, IRQ_LEVEL);
  }
  return IRQ_HANDLED;
}

static int __init raspberrypigpio_init(void) {
  unsigned int gpio;
  int ret; 

  printk(KERN_DEBUG "Raspberry PI SoC IRQ: initializing\n");

  /* create reboot thread */
  task = kthread_create(raspberrypi_IRQ_thread, NULL, "raspberry-pi-soc-irq");
  if (IS_ERR(task)) {
    printk(KERN_ALERT "kthread_create error\n");
  }
  wake_up_process(task);

  //TODO: change OLD method to NEW  <<<<<

  /* setup an irq that watch the reset gpio state */
  gpio = RASPBERRYPI_GPIO_IRQ_PIN;
  gpio_request(gpio, "softIRQ");
  gpio_direction_input(gpio);
  g_irq = gpio_to_irq(gpio);

    if(g_irq < 0)
    {
        printk(KERN_INFO "ERROR\n");
        return g_irq;
    }

  irq_clear_status_flags(g_irq, IRQ_LEVEL);

  ret = request_any_context_irq(g_irq, raspberrypi_irq_handler, IRQF_TRIGGER_FALLING, "raspberry-pi", NULL);

  printk(KERN_INFO "Raspberry PI SoC IRQ: initialized\n");

  return 0;
}

static void __exit raspberrypigpio_cleanup(void) {
  printk(KERN_DEBUG "Raspberry PI SoC IRQ: cleaning...\n");

  /* terminate the thread */
  need_Print = 0;
  wake_up(&probe_waitqueue);

  /* ensure free irq */
  if (g_irq != -1) {
    free_irq(g_irq, NULL);
  }

  /* free gpio */
  gpio_free(RASPBERRYPI_GPIO_IRQ_PIN);

  printk(KERN_DEBUG "Raspberry SoC IRQ cleaned\n");
  return;
}

module_init(raspberrypigpio_init);
module_exit(raspberrypigpio_cleanup);

MODULE_AUTHOR("Taktopanie");
MODULE_DESCRIPTION("Raspberry - Raspberry PI SoC simple IRQ");
MODULE_LICENSE("GPL");