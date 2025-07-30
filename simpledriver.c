#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define LED_GPIO 49

static dev_t numba;
static struct cdev ext_dev;

ssize_t led_write(struct file *filp, const char __user *buf, size_t size, loff_t *ind) {
    char kbuf[8];
    int state;

    if (size > sizeof(kbuf) - 1)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, size))
        return -EFAULT;

    kbuf[size] = '\0';

    if (kstrtoint(kbuf, 10, &state) < 0)
        return -EINVAL;

    gpio_set_value(LED_GPIO, state > 0);
    pr_info("GPIO49 LED set to: %d\n", state > 0);

    return size;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_write,
};

static int __init led_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&numba, 0, 1, "numba");
    if (ret) {
        pr_err("Failed to allocate char dev region\n");
        return ret;
    }
    pr_info("Registered device: major %d, minor %d\n", MAJOR(numba), MINOR(numba));

    cdev_init(&ext_dev, &fops);
    ext_dev.owner = THIS_MODULE;

    ret = cdev_add(&ext_dev, numba, 1);
    if (ret) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(numba, 1);
        return ret;
    }

    pr_info("GPIO49 LED: module loaded\n");

    ret = gpio_request(LED_GPIO, "LED_GPIO49");
    if (ret) {
        pr_err("Failed to request GPIO49\n");
        cdev_del(&ext_dev);
        unregister_chrdev_region(numba, 1);
        return ret;
    }

    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        pr_err("Failed to set GPIO direction\n");
        gpio_free(LED_GPIO);
        cdev_del(&ext_dev);
        unregister_chrdev_region(numba, 1);
        return ret;
    }

    gpio_set_value(LED_GPIO, 1);  // Turn on initially
    pr_info("GPIO49 LED is ON\n");

    return 0;
}

static void __exit led_exit(void)
{
    gpio_set_value(LED_GPIO, 0); // Turn LED OFF
    gpio_free(LED_GPIO);

    cdev_del(&ext_dev);
    unregister_chrdev_region(numba, 1);

    pr_info("LED module unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AFE");
MODULE_DESCRIPTION("GPIO49 LED toggle driver for BeagleBone Black");
