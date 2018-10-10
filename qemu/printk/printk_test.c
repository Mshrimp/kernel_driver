#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

struct timer_list timer;

void timer_handler(unsigned long arg)
{
	static int index = 0;
	mod_timer(&timer, jiffies + HZ);
	//printk("index: %d\n", index);
	index++;
	pr_debug("...... index: %d\n", index);
}

static int hello_init(void)
{
	printk(KERN_DEBUG"A %s Hello world!\n", KERN_DEBUG);

	init_timer(&timer);
	timer.function = timer_handler;

	add_timer(&timer);

	return 0;
}

static void hello_exit(void)
{
	printk(KERN_DEBUG"A %s Bye world!\n", KERN_DEBUG);

	del_timer_sync(&timer);

	return;
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


