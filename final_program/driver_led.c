#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/kernfs.h>


#define GPIO_NUM (43)  /* 待操作的gpio管脚 */
/* ioctl命令行 */
#define LED_IOC_MAGIC 'L'
#define SET_LED_OFF    _IO(LED_IOC_MAGIC, 1) //开灯指令
#define SET_LED_ON     _IO(LED_IOC_MAGIC, 2) //关灯指令

enum led_stat { 
	LED_OFF = 0x0,
	LED_ON,
};

/*文件打开函数*/
int led_open(struct inode *inode, struct file *filp)
{    
    return 0; //这个是个空函数，默认都能成功打开
}

// 读函数，空
static ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
    return -EINVAL; // 返回无效操作错误
}

/*文件释放函数*/
int led_release(struct inode *inode, struct file *filp)
{
    return 0; //这个是个空函数，默认都能成功打开
}


static long led_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{	
	long ret = 0;

	/* check IOCTL command magic */
	if (_IOC_TYPE(cmd) != LED_IOC_MAGIC) {
		printk("[%s] 无效参数!\n", __func__);
        return -ESRCH;
	}
	
	switch (cmd) { 
        case SET_LED_ON:
			gpio_direction_output(GPIO_NUM, LED_ON); //点亮该led
			break;
        case SET_LED_OFF:
			gpio_direction_output(GPIO_NUM, LED_OFF); //熄灭该led
			break;
        default:
            ret = -ESRCH;
	}
	
	return ret;
}

/*文件操作结构体*/
static const struct file_operations led_fops =
{
	.owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
	.unlocked_ioctl = led_ioctl,
    .release = led_release,
};

struct cdev cdev; //静态定义字符设备cdev结构
dev_t devno;  //静态定义字符设备号

/*设备驱动模块加载函数*/
static int __init led_init(void)
{   
	int ret = -1;
	unsigned int *addr;
	
    cdev_init(&cdev, &led_fops);/* 初始化cdev结构， 并将文件IO与字符设备的cdev建立链接 */
    ret = alloc_chrdev_region(&devno, 0, 1, "led"); /* 动态申请设备号， */
	if (ret) {
		printk("申请设备号失败:%d\n", ret);
		cdev_del(&cdev);   /*注销设备*/
		return ret;
	}
    cdev_add(&cdev, devno, 1); /* 向linux系统添加一个字符设备 */

	/* 初始化gpio管脚 */
	/* 修改gpio42\gpio43\gpio52\gpio53管脚复用关系为gpio模式 */
	addr = ioremap(0x04020068, 4);
	writel(0x01001000, addr);
	writel(0x01001000, addr+1);
	writel(0x01001000, addr+2);
	writel(0x01001000, addr+3);
	iounmap(addr);
	ret = gpio_request(GPIO_NUM, "led_gpio"); //申请led一个gpio
	if(ret < 0) {
		printk("gpio_request[%d] error:%d\n", GPIO_NUM, ret);
		goto ERR;
	}
    printk("led驱动初始化完成\n");
	
	return 0;

ERR:
	cdev_del(&cdev);   /*注销设备*/
    unregister_chrdev_region(devno, 1); /*释放设备号*/
	
	return ret;
}

/*模块卸载函数*/
static void __exit led_exit(void)
{
    cdev_del(&cdev);   /*注销设备*/
    unregister_chrdev_region(devno, 1); /*释放设备号*/
	gpio_direction_output(GPIO_NUM, LED_OFF); //默认熄灭该led
	gpio_free(GPIO_NUM);
	printk ("led驱动卸载完成\n");
}
module_init(led_init); //内核模块加载函数
module_exit(led_exit); //内核模块卸载函数
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR ("MorningStar");
MODULE_DESCRIPTION ("Ioctl-based LED Driver");