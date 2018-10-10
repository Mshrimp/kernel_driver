#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>


static int hello_init(void)
{
	printk(KERN_DEBUG"A %s Hello world!\n", KERN_DEBUG);

	return 0;
}

static void hello_exit(void)
{
	printk(KERN_DEBUG"A %s Bye world!\n", KERN_DEBUG);

	return;
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


