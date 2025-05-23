#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/sched.h>

extern void fh_irq_enable(unsigned int gpio);
extern void fh_irq_disable(unsigned int gpio);

#define GPIO_BUTTON_1 (24)
#define GPIO_BUTTON_2 (25)
#define GPIO_BUTTON_3 (56)
#define BUTTON_COUNT (3) // 按键数量

static int gpio_buttons[] = {GPIO_BUTTON_1, GPIO_BUTTON_2, GPIO_BUTTON_3}; // 定义按键对应的GPIO数组
static int button_irqs[BUTTON_COUNT]; // 定义按键中断号数组
static struct fasync_struct *button_async; // 定义异步通知结构体指针

struct cdev cdev; // 静态定义字符设备cdev结构
dev_t devno; // 静态定义字符设备号

// 被按下的按钮
static int pressed_button = -1;

// 按键中断处理函数
static irqreturn_t button_isr(int irq, void *dev_id){
    int index = (uintptr_t)dev_id; // 获取中断号对应的按键索引
    printk("button[%d] isr\n", index);
    
    fh_irq_disable(gpio_buttons[index]); // 临时禁止中断放抖动

    pressed_button = index; // 记录被按下的按钮索引
    if(button_async) {
        kill_fasync(&button_async, SIGIO, POLL_IN); // 异步通知应用程序
    }
    fh_irq_enable(gpio_buttons[index]); // 重新启用中断

    return IRQ_HANDLED; // 中断返回

}

// 异步通知接口
static int button_fasync(int fd, struct file *filp, int on){
    return fasync_helper(fd, filp, on, &button_async); // 异步通知初始化
}

// 打开函数，空
static int button_open(struct inode *inode, struct file *filp){
    return 0; // 成功打开
}

// 释放函数，空
static int button_release(struct inode *inode, struct file *filp){
    button_fasync(-1, filp, 0); // 关闭异步通知
    return 0;
}

// 读函数，空
static ssize_t button_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
    int val;
    val = pressed_button; // 获取被按下的按钮索引
    pressed_button = -1; // 重置

    if(copy_to_user(buf, &val, sizeof(val))){ // 将索引复制到用户空间
        return -EFAULT; // 复制失败
    }
    return sizeof(val); // 返回读取的字节数
}

// 文件操作结构体
static const struct file_operations button_fops = {
    .owner = THIS_MODULE, // 模块所有者
    .open = button_open, // 打开函数
    .release = button_release, // 释放函数
    .read = button_read, // 读函数
    .fasync = button_fasync, // 异步通知函数
};

// 模块加载函数
static int __init button_init(void){
    int ret;
    int i;
    unsigned int *addr;

    // 分配设备号
    ret = alloc_chrdev_region(&devno, 0, 1, "button");
    if(ret < 0){
        printk("分配设备号错误:%d\n", ret);
        return ret;
    }

    // 初始化字符设备
    cdev_init(&cdev, &button_fops);
    ret = cdev_add(&cdev, devno, 1);
    if(ret < 0){
        printk("添加字符设备错误:%d\n", ret);
        unregister_chrdev_region(devno, 1); // 释放设备号
        return ret;
    }

    // 修改GPIO管脚复用关系为GPIO模式
    addr = ioremap(0x04020134, 4); // gpio24
    writel(0x1001000, addr);
    iounmap(addr);

    addr = ioremap(0x04020138, 4); // gpio25
    writel(0x1001000, addr);
    iounmap(addr);

    addr = ioremap(0x040200b4, 4); // gpio56
    writel(0x1001000, addr);
    iounmap(addr);

    // 申请GPIO并注册中断(仅上升沿触发)
    for(i = 0; i < BUTTON_COUNT; i++){
        ret = gpio_request(gpio_buttons[i], "button"); // 申请GPIO
        if(ret < 0){
            printk("gpio_request[%d] error:%d\n", gpio_buttons[i], ret);
            goto err_gpio;
        }

        gpio_direction_input(gpio_buttons[i]); // 设置GPIO为输入模式

        button_irqs[i] = gpio_to_irq(gpio_buttons[i]); // 获取GPIO对应的中断号
        ret = request_irq(button_irqs[i], button_isr, IRQF_TRIGGER_RISING, "button", (void *)i); // 注册中断处理函数
        if(ret < 0){
            printk("request_irq[%d] error:%d\n", button_irqs[i], ret);
            goto err_irq;
        }
    }
    printk("button驱动初始化完成\n");
    return 0;
err_irq:
    free_irq(button_irqs[i], NULL);
    gpio_free(gpio_buttons[i]);
    for(--i; i >= 0; i--){ // 释放GPIO和中断
        free_irq(button_irqs[i], NULL);
        gpio_free(gpio_buttons[i]);
    }
    cdev_del(&cdev); // 注销字符设备
    unregister_chrdev_region(devno, 1); // 释放设备号
    return ret;
err_gpio:
    gpio_free(gpio_buttons[i]); // 释放GPIO
    for(--i; i >= 0; i--){
        free_irq(button_irqs[i], NULL); // 释放中断
        gpio_free(gpio_buttons[i]); // 释放GPIO
    }
    cdev_del(&cdev); // 注销字符设备
    unregister_chrdev_region(devno, 1); // 释放设备号
    return ret;
}

// 模块卸载函数
static void __exit button_exit(void){
    int i;

    for(i = 0; i < BUTTON_COUNT; i++){ // 释放GPIO和中断
        free_irq(button_irqs[i], NULL);
        gpio_free(gpio_buttons[i]);
    }
    cdev_del(&cdev); // 注销字符设备
    unregister_chrdev_region(devno, 1); // 释放设备号
    printk("button驱动卸载完成\n");
}

module_init(button_init); // 注册模块加载函数
module_exit(button_exit); // 注册模块卸载函数
MODULE_LICENSE("GPL"); // 模块许可证声明
MODULE_AUTHOR("MorningStar");
MODULE_DESCRIPTION("Signal-based Button Driver");
