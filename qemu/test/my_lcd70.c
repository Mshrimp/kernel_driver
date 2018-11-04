#include <linux/init.h>
#include <linux/module.h>

static __init int test_init(void)
{
	int i = 0;
	printk("This is a driver for lcd70\n");

	for (i = 0; i < CONFIG_MY_NUM; i++)
	{
		printk("MY_TEST_LCD70, %s, count: %d\n", CONFIG_MY_STR, i);
	}

	return 0;
}

static __init void test_exit(void)
{

	return;
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

