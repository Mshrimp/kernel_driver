#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

int test_read_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;

	sprintf(page, "This is a create proc entry driver!\n");

	len = strlen(page);

	return len + 1;
}

static int __init test_init(void)
{
	struct proc_dir_entry *proc_entry = NULL;
	printk(KERN_DEBUG "test_init\n");

	proc_entry = create_proc_read_entry("test_proc", 0, NULL, test_read_proc, NULL);
	if (NULL == proc_entry)
	{
		printk("create_proc_read_entry failed!\n");
		return -1;
	}

	printk("create_proc_read_entry OK!\n");

	return 0;
}

static void test_exit(void)
{
	remove_proc_entry("test_proc", NULL);
	printk("remove_proc_entry\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

