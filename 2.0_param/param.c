#include <linux/module.h>
#include <linux/moduleparam.h>

static int value = 0;
static int arr[2] = {0, 0};
int pram_cb_name = 0;
char *name = "default";

module_param(value, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(value, "An integer parameter");

module_param(name, charp, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(name, "A string parameter");

module_param_array(arr, int, NULL, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(arr, "An array of integers");

static int set_param(const char *val, const struct kernel_param *kp)
{
    int ret = param_set_int(val, kp);
    if (ret == 0) {
        pr_info("Callback: New pram_cb_name value = %d\n", pram_cb_name);
        return 0;
    }
    return -EINVAL;
}

static struct kernel_param_ops pops = {
    .set = &set_param,
    .get = &param_get_int,
};

module_param_cb(pram_cb_name, &pops, &pram_cb_name, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(pram_cb_name, "An integer parameter with custom callback");

/**
 * Module initialization function
*/
static int __init param_init(void)
{
    pr_info("value = %d\n", value);
    pr_info("pram_cb_name = %d\n", pram_cb_name);
    pr_info("arr: %d %d\n", arr[0], arr[1]);
    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;
}

/**
 * Module exit function
*/
static void __exit param_exit(void)
{
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(param_init);
module_exit(param_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A parameter device driver");
MODULE_VERSION("1.0");
