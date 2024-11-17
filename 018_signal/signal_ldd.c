#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>       // MAJOR and MINOR
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define IRQ_GPIO 30
#define SIG_NUM 44
#define REG_CURRENT_TASK _IOW('a', 'a', int32_t *)

dev_t dev_num;
static struct class *rootv_class;
static struct cdev rootv_cdev;
static struct task_struct *task = NULL;

/* handle irq function: return irq_handler_t */
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    struct kernel_siginfo k_siginfo;
    pr_info("interrupted!\n");

    memset(&k_siginfo, 0, sizeof(struct kernel_siginfo));
    k_siginfo.si_signo = SIG_NUM;
    k_siginfo.si_code = SI_QUEUE;
    k_siginfo.si_int = 1;

    if (task != NULL) {
        if (send_sig_info(SIG_NUM, &k_siginfo, task) < 0) {
            pr_err("cannot send signal\n");
        }
    }

    return IRQ_HANDLED;
}

static int rootv_open(struct inode *inode, struct file *file)
{
    pr_info("device was opened!\n");
    return 0;
}

static int rootv_release(struct inode *inode, struct file *file)
{
    struct task_struct *ref_task = get_current();
    if (ref_task == task) {
        task = NULL;
    }
    
    pr_info("device was closed!\n");
    return 0;
}

static ssize_t rootv_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("device was read!\n");
    return 0;
}

static ssize_t rootv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("device was wrote!\n");
    return 0;
}

static long rootv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    if (cmd == REG_CURRENT_TASK) {
        task = get_current();
    }

    pr_info("device in ioctl!\n");
    return 0;
}

/* file operations */
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = rootv_open,
    .release = rootv_release,
    .read = rootv_read,
    .write = rootv_write,
    .unlocked_ioctl = rootv_ioctl,
};

/* init module */
static int __init signal_driver_init(void)
{
    /* register major and minor number */
    if (alloc_chrdev_region(&dev_num, 0, 1, "rootv_devnum") < 0) {
        pr_err("cannot alloc device number\n");
        return -1;
    }
    pr_info("rootv_device: major %d minor %d\n", MAJOR(dev_num), MINOR(dev_num));

    /* create class device */
    if (IS_ERR(rootv_class = class_create(THIS_MODULE, "rootv_class"))) {
        pr_err("cannot create device class\n");
        goto err_class;
    }

    /* create device file */
    if (IS_ERR(device_create(rootv_class, NULL, dev_num, NULL, "rootv_device"))) {
        pr_err("cannot create device\n");
        goto err_dev;
    }

    /* add device to the system */
    cdev_init(&rootv_cdev, &fops);
    if (cdev_add(&rootv_cdev, dev_num, 1) < 0) {
        pr_err("cannot add device\n");
        goto err_cdev;
    }

    if (gpio_request(IRQ_GPIO, "irq-gpio") < 0) {
        pr_err("cannot request irq gpio\n");
        goto err_gpio;
    }

    if (gpio_direction_input(IRQ_GPIO) < 0) {
        pr_err("cannot set input for gpio\n");
        goto err_gpio;
    }

    if (request_irq(gpio_to_irq(IRQ_GPIO), irq_handler, IRQF_TRIGGER_RISING,
	    "irq-gpio", NULL) < 0) {
        pr_err("cannot request irq\n");
        goto err_irq;       
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_irq:
    free_irq(gpio_to_irq(IRQ_GPIO), NULL);
err_gpio:
    gpio_free(IRQ_GPIO);
err_cdev:
    cdev_del(&rootv_cdev);
    device_destroy(rootv_class, dev_num);
err_dev:
    class_destroy(rootv_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/* remove module */
static void __exit signal_driver_exit(void)
{
    free_irq(gpio_to_irq(IRQ_GPIO), NULL);
    gpio_free(IRQ_GPIO);
    cdev_del(&rootv_cdev);
    device_destroy(rootv_class, dev_num);
    class_destroy(rootv_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/* register init and remove module */
module_init(signal_driver_init);
module_exit(signal_driver_exit);

/**
 * module infomation
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Signal Linux Driver");
MODULE_VERSION("GPL");
