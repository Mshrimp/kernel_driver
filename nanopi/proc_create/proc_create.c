#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


int test_proc_show(struct seq_file *seq, void *data)
{
	seq_printf(seq, "This is a proc create driver!\n");

	return 0;
}

int test_proc_open (struct inode *inode, struct file *file)
{
	printk("test_proc_open\n");

	return single_open(file, test_proc_show, inode->i_private);
}

static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = test_proc_open,
	.read = seq_read,
	.release = seq_release
};

static int __init test_init(void)
{
	struct proc_dir_entry *proc_entry = NULL;
	printk(KERN_DEBUG "test_init\n");

	proc_entry = proc_create("test_proc", 0, NULL, &proc_fops);
	if (NULL == proc_entry)
	{
		printk("proc_create failed!\n");
		return -1;
	}

	printk("proc_create OK!\n");

	return 0;
}

static void test_exit(void)
{
	remove_proc_entry("test_proc", NULL);
	printk("remove proc_create\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

