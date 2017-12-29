/*
 * char device driver
 */
#include <linux/cdev.h>        /* struct cdev */
#include <linux/types.h>       /* dev_t */
#include <linux/fs.h>          /* struct file_operations */
#include <linux/printk.h>      /* pr_err */
#include <linux/module.h>      /* MODULE_AUTHOR */
#include <linux/moduleparam.h>
#include <linux/init.h>        /* module_init */
#include <linux/err.h>         /* IS_ERR */
#include <linux/device.h>      /* device_create */
#include <linux/platform_device.h>


MODULE_AUTHOR("Sun Yang");
MODULE_LICENSE("GPL");

#define MAX_CHR_NUM  3
#define CHR_DEMO_NAME "chr_demo"
static dev_t chr_dev;
static unsigned chr_dev_num = MAX_CHR_NUM;
static struct cdev chr_cdev[MAX_CHR_NUM];
static struct device *chr_device[MAX_CHR_NUM];
static struct class *chr_demo_class;

static int demo_chr_open(struct inode *inode, struct file *filp)
{
    return 0;
}
static ssize_t demo_chr_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    return 0;
}
static ssize_t demo_chr_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    return 0;
}
static long demo_chr_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}
static int demo_chr_release(struct inode *inode, struct file *filp)
{
    return 0;
}

struct file_operations chr_ops = {
    .owner  = THIS_MODULE,
    .open   = demo_chr_open,
    .read   = demo_chr_read,
    .write  = demo_chr_write,
    .unlocked_ioctl  = demo_chr_ioctl,
    .release = demo_chr_release,
};

int demo_chr_init(void)
{
    int i, ret = 0;
    /* allocate minor from 0 to chr_dev_num
    */
    ret = alloc_chrdev_region(&chr_dev, 0, chr_dev_num, CHR_DEMO_NAME);
    if (ret) {
        pr_err("%s : alloc of chrdev returned %d\n",
               CHR_DEMO_NAME, ret);
        return 0;
    }
    
    printk("%s : major = %d\n", CHR_DEMO_NAME, MAJOR(chr_dev));
#if 1   
    /* 分别注册各个minor device，这样minor device的操作函数可以是不同的 */
    for(i = 0; i < MAX_CHR_NUM; i++) {
        cdev_init(&chr_cdev[i], &chr_ops);
        chr_cdev[i].owner = THIS_MODULE;
        chr_cdev[i].ops = &chr_ops;
        cdev_add(&chr_cdev[i], chr_dev_num, 1);
    }
#else
    /* 一次性注册所有minor device         */
    struct cdev chr_dev;
    cdev_init(&chr_dev, &chr_ops);
    cdev_add(&chr_dev, chr_dev_num, MAX_CHR_NUM);
#endif
    /* Create a sysfs class */
    /* udev会在/dev/下创建对应的设别节点，前提是用户空间有udev守护进程 */
    chr_demo_class = class_create(THIS_MODULE, "char_demo");
    if (IS_ERR(chr_demo_class)) {
        ret = PTR_ERR(chr_demo_class);
        pr_err("%s: error %d creating the char_demo class", CHR_DEMO_NAME, ret);
        goto err_del_cdev;
    }
    for (i = 0; i < MAX_CHR_NUM; i++) {
        chr_device[i] = device_create(chr_demo_class, &platform_bus, 
                                MKDEV(MAJOR(chr_dev), i), NULL, "char_demo%d", i);
        if (IS_ERR(chr_device[i])) {
            pr_err("%s: could not create device\n", CHR_DEMO_NAME);
            goto err_del_cdev;
        }
    }
    printk("%s : register char demo success\n", CHR_DEMO_NAME);
    return 0;

err_del_cdev:
    for (i = 0; i < MAX_CHR_NUM; i++) {
        cdev_del(&chr_cdev[i]);
    }
    printk("%s : register char demo error\n", CHR_DEMO_NAME);
    return 0;
}

int demo_chr_uninit(void)
{
    int i;
    
    for (i = 0; i < MAX_CHR_NUM; i++) {
        device_unregister(chr_device[i]);
        cdev_del(&chr_cdev[i]);
    }
    class_destroy(chr_demo_class);
    unregister_chrdev_region(MKDEV(chr_dev, 0), MAX_CHR_NUM);
    
    printk("%s : unregister char demo success\n", CHR_DEMO_NAME);
    return 0;
}
module_init(demo_chr_init);
module_exit(demo_chr_uninit);
