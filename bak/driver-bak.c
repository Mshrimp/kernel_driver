#include "../log/log.h"
#include "../product/product.h"

#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/kfifo.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/miscdevice.h>

#if 0
#define DRV_DBG(fmt, args...)	printk("==> DRV_DBG:[%s:%d]:" fmt "\n",  __func__, __LINE__, ##args)
#define DRV_ERR(fmt, args...)	printk("==> DRV_ERR:[%s:%d]:" fmt "\n",  __func__, __LINE__, ##args)
#else
#define DRV_DBG nv_debug
#define DRV_ERR nv_error
#endif

#define EV_FIFO_SIZE	(10<<10)	/* 10K */

extern int nv_log_init(void);
extern void nv_log_exit(void);

typedef struct {
	struct semaphore sem;

	spinlock_t fifo_lock;
	struct kfifo ev_fifo;
	wait_queue_head_t ev_wait_queue;

	const nv_product_ops_t *nv_product;
} nv_driver_t;

static nv_driver_t nv_driver = {
	.sem = __SEMAPHORE_INITIALIZER(nv_driver.sem, 1),
	.fifo_lock = __SPIN_LOCK_UNLOCKED(nv_driver.fifo_lock),
	.ev_wait_queue = __WAIT_QUEUE_HEAD_INITIALIZER(nv_driver.ev_wait_queue),
};

static void nv_drv_ev_report(nv_dev_ev_e ev, void *param) {
	unsigned long flags;
	nv_driver_t *driver = (nv_driver_t *)param;
	
	spin_lock_irqsave(&driver->fifo_lock, flags);

	if (!kfifo_is_full(&driver->ev_fifo)) {
		kfifo_in(&driver->ev_fifo, &ev, sizeof(ev));
	} else {
		DRV_ERR("Err ev fifo full !");
	}
	
	spin_unlock_irqrestore(&driver->fifo_lock, flags);
	
	wake_up_interruptible(&driver->ev_wait_queue);
	DRV_DBG("Ev:%s EV Cnt:%d", EV_STRING(ev), kfifo_len(&driver->ev_fifo) / sizeof(ev));
}

static int nv_dev_open(struct inode *inode, struct file *file) {
	int ret = 0;

	if (down_trylock(&nv_driver.sem)) {
		DRV_ERR("Err device busy");
		return -EBUSY;
	}

	do {
		ret = kfifo_alloc(&nv_driver.ev_fifo, EV_FIFO_SIZE, GFP_KERNEL);
		if (ret) {
			DRV_ERR("Err alloc fifo : %d", ret);
			break;
		}

		nv_driver.nv_product = creat_product();
		ret = nv_driver.nv_product->init(nv_drv_ev_report, &nv_driver);
		if (ret) {
			kfifo_free(&nv_driver.ev_fifo);
			DRV_ERR("Err init product : %d", ret);
			break;
		}

		file->private_data = &nv_driver;
		DRV_DBG("NV dev open success !");
	} while (0);

	if (ret) {
		up(&nv_driver.sem);
	}

	return ret;
}

static long nv_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	nv_driver_t *driver = (nv_driver_t *)file->private_data;
	return driver->nv_product->ioctl(cmd, arg);
}

static ssize_t  nv_dev_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
	nv_dev_ev_e ev;
	nv_driver_t *driver = (nv_driver_t *)file->private_data;

	if (file->f_flags & O_NONBLOCK) {
		if (kfifo_is_empty(&driver->ev_fifo)) {
			DRV_ERR("No event happened");
			return -EAGAIN;
		}
	} else {
		wait_event_interruptible(driver->ev_wait_queue, !kfifo_is_empty(&driver->ev_fifo));
	}

	preempt_disable();
	kfifo_out(&driver->ev_fifo, &ev, sizeof(ev));
	preempt_enable();
	
	if(copy_to_user(buf, &ev, sizeof(ev))) {
		return -EFAULT;
	}

	return sizeof(ev);
}

static unsigned int nv_dev_poll(struct file *file, poll_table *wait) {
	unsigned int mask = 0;
	nv_driver_t *driver = (nv_driver_t *)file->private_data;

	poll_wait(file, &driver->ev_wait_queue, wait);

	if (!kfifo_is_empty(&driver->ev_fifo)) {
		mask |= POLLIN | POLLRDNORM;
	}
	return mask;
}

static int nv_dev_release(struct inode *inode, struct file *file) {
	int ret;
	nv_driver_t *driver = (nv_driver_t *)file->private_data;
	ret = driver->nv_product->uninit();

	if (!ret) {
		up(&driver->sem);
		kfifo_free(&driver->ev_fifo);
	}

	DRV_DBG("Nv dev release done(%d)", ret);
	return ret;
}

static struct file_operations nv_dev_fops = {
	.owner = THIS_MODULE,
	.open = nv_dev_open,
	.read = nv_dev_read,
	.poll = nv_dev_poll,
	.unlocked_ioctl = nv_dev_ioctl,
	.release = nv_dev_release,
};

static struct miscdevice nv_misc = {
	.fops  = &nv_dev_fops,
	.name  = NV_DEVICE_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

static int nv_drv_init(void) {
	int ret;
	if (0 != (ret = nv_log_init())) {
		return ret;
	}
	
	if (0 != (ret = misc_register(&nv_misc))) {
		DRV_ERR("Err registe misc device:%d\n", ret);
		nv_log_exit();
		return ret;
	}

	DRV_DBG("nv_drv_init success");
	return 0;
}

static void nv_drv_exit(void) {
	DRV_DBG("nv_drv_exit");
	
	nv_log_exit();
	misc_deregister(&nv_misc);
}

module_init(nv_drv_init);
module_exit(nv_drv_exit);
MODULE_LICENSE("GPL");
