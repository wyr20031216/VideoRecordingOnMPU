#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

// LED驱动的ioctl命令
#define LED_IOC_MAGIC 'L'
#define SET_LED_ON    _IO(LED_IOC_MAGIC, 2) // 开灯命令
#define SET_LED_OFF   _IO(LED_IOC_MAGIC, 1) // 关灯命令

int led_fd; // LED设备文件描述符
int button_fd; // 按钮设备文件描述符
volatile int running = 1; // 程序运行标志

// SIGIO信号处理函数
void handle_sigio(int sig, siginfo_t *info, void *ucontext) {
    int button_index; // 按钮索引
    ssize_t ret = read(button_fd, &button_index, sizeof(button_index));

    if(ret == sizeof(int)){
        if(button_index == 0) { 
            printf("第一个按钮被按下，开始\n");
            ioctl(led_fd, SET_LED_ON);
        } else if(button_index == 1) {
            printf("第二个按钮被按下，结束\n");
            ioctl(led_fd, SET_LED_OFF);
        } else if(button_index == 2) {
            printf("第三个按钮被按下，程序退出\n");
            running = 0; // 退出程序
        }
    }
}

int main() {
    struct sigaction sa; // 信号处理结构
    
    // 打开按钮设备和LED设备
    button_fd = open("/dev/button", O_RDWR);
    led_fd = open("/dev/led", O_RDWR);
    if (button_fd < 0) {
        printf("无法打开按钮设备\n");
        return -1;
    }
    if(led_fd < 0) {
        printf("无法打开LED设备\n");
        close(button_fd);
        return -1;
    }

    // 设置异步通知
    fcntl(button_fd, F_SETOWN, getpid()); // 设置文件所有者为当前进程
    int flags = fcntl(button_fd, F_GETFL); // 获取文件标志
    fcntl(button_fd, F_SETFL, flags | FASYNC); // 启用异步通知

    // 注册SIGIO信号处理函数
    sa.sa_sigaction = handle_sigio; // 设置信号处理函数
    sigemptyset(&sa.sa_mask); // 清空信号掩码
    sa.sa_flags = SA_SIGINFO; // 使用siginfo_t结构传递信息
    sigaction(SIGIO, &sa, NULL); // 注册SIGIO信号处理函数

    // 循环等待信号
    while(running) {
        pause(); // 等待信号
    }

    close(button_fd); // 关闭按钮设备文件
    close(led_fd); // 关闭LED设备文件
    return 0;
}