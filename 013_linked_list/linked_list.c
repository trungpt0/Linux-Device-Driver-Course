#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;

/* node info */
struct my_node
{
    int data;
    struct list_head list;
};

/* initialize head of linked list */
LIST_HEAD(head_node); 

/* add node function */
static void add_node(int value)
{
    struct my_node *new_node;

    /* alloc a new node */
    new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
    if (!new_node) {
        pr_err("cannot alloc a new node\n");
        return;
    }

    new_node->data = value;
    INIT_LIST_HEAD(&new_node->list);
    list_add_tail(&new_node->list, &head_node);

    pr_info("new node added with value: %d\n", value);
}

/* delete all node function */
static void delete_all_nodes(void)
{
    struct my_node *pos_node, *tmp;
    list_for_each_entry_safe(pos_node, tmp, &head_node, list);
    list_del(&pos_node->list);
    kfree(pos_node);
    pr_info("node deleted with value: %d\n", pos_node->data);
}

/* Open rootv device file */
static int rootv_open(struct inode *inode, struct file *file)
{
    pr_info("device was opened\n");
    return 0;
}

/* Close rootv device file */
static int rootv_release(struct inode *inode, struct file *file)
{
    pr_info("device was closed\n");
    return 0;
}

/* Read rootv device file */
static ssize_t rootv_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    struct my_node *node;
    char kbuffer[256];
    int pos = 0;

    list_for_each_entry(node, &head_node, list) {
        pos += snprintf(kbuffer + pos, sizeof(kbuffer) - pos, "%d\n", node->data);
    }

    if (*off >= pos) return 0;
    if (len > pos - *off) len = pos - *off;

    if (copy_to_user(buf, kbuffer, len)) return -EFAULT;
    *off += len;

    pr_info("device was read");
    return len;
}

/* Write rootv device file */
static ssize_t rootv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    char kbuffer[16];
    int value;

    if (len > sizeof(kbuffer) - 1) len = sizeof(kbuffer) - 1;
    if (copy_from_user(kbuffer, buf, len)) return -EFAULT;

    kbuffer[len] = '\0';
    if (kstrtoint(kbuffer, 10, &value) < 0) return -EINVAL;

    add_node(value);

    pr_info("device was wrote");
    return len;
}

/* File operations device file */
static const struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = rootv_open,
    .release = rootv_release,
    .read = rootv_read,
    .write = rootv_write,
};

/* Add the device driver */
static int __init linked_list_driver_init(void)
{
    /* Resgister device number */
    if (alloc_chrdev_region(&dev_num, 0, 1, "rootv_device") < 0) {
        pr_err("cannot alloc device number\n");
        return -1;
    }
    pr_info("rootv_device - major %d minor %d\n", MAJOR(dev_num), MINOR(dev_num));

    /* Create class device */
    if (IS_ERR(dev_class = class_create(THIS_MODULE, "rootv_class"))) {
        pr_err("cannot create device class\n");
        goto err_class;
    }

    /* Create device */
    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "rootv_dev"))) {
        pr_err("cannot create device\n");
        goto err_dev;
    }

    /* Add device to the system */
    cdev_init(&rootv_cdev, &fops);
    if (cdev_add(&rootv_cdev, dev_num, 1) < 0) {
        pr_err("cannot add device\n");
        goto err_cdev;
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_cdev:
    device_destroy(dev_class, dev_num);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/* Remove driver */
static void __exit linked_list_driver_exit(void)
{
    delete_all_nodes();
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(linked_list_driver_init);
module_exit(linked_list_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Linked List Linux Driver");
MODULE_VERSION("1.0");
