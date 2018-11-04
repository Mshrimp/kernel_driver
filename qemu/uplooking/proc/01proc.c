#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <linux/proc_fs.h>

struct proc_dir_entry *my_dir;
struct proc_dir_entry *my_file;

int read_my_file(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	printk("reading!\n");

	return 0;
}

static __init int test_init(void)
{
	int ret = 0;
	// 创建目录 /proc/upolooking
	my_dir = proc_mkdir("uplooking", NULL);    // NULL表示父目录为/proc，也可以是别的目录
	if (NULL == my_dir) {
		return -ENOMEM;    // 内存不足
	}

	// 创建一个只读的文件
	my_file = create_proc_read_entry("123.txt", 0, my_dir, read_my_file, (void *)0x1234);
	if (!my_file) {
		ret = -ENOMEM;
		goto create_file_failed;
	}

	return 0;

create_file_failed:
	remove_proc_entry("uplooking", my_dir);
	return ret;
}

// 在init函数中，越靠后的操作，在exit函数中，反操作越靠前
static __exit void test_exit(void)
{
	    
	remove_proc_entry("uplooking", my_dir);
	remove_proc_entry("uplooking", NULL);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
