
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>	/* for copy_to_user */
#include <linux/cdev.h>
#include <asm-generic/errno.h>
#include <linux/init.h>
#include <linux/tty.h>      /* For fg_console */
#include <linux/kd.h>       /* For KDSETLED */
#include <linux/vt_kern.h>
#include <linux/version.h> /* For LINUX_VERSION_CODE */
#include <linux/uidgid.h> /*Para recibir el uid y ver si es root*/

MODULE_LICENSE("GPL");

/*
 *  Declaraciones de las funciones
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);


#define SUCCESS 0
#define DEVICE_NAME "chardev_leds"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

dev_t start; /* par de entrada para el driver */

struct cdev* chardev = NULL; /*estructura cdev asociada al driver*/
static int Device_Open = 0;	/* Is device open?
                 * Used to prevent multiple access to device */


static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/*puntero para lectura*/
static char* lectura = NULL;


/*puntero al handler del driver del teclado*/
struct tty_driver* kbd_driver = NULL;


/* Get keyboard driver handler */
struct tty_driver* get_kbd_driver_handler(void) {
    printk(KERN_INFO "chardev_leds: loading\n");
    printk(KERN_INFO "chardev_leds: su terminal es %x\n", fg_console);
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return vc_cons[fg_console].d->port.tty->driver;
#else
    return vc_cons[fg_console].d->vc_tty->driver;
#endif
}

/* Set led state to that specified by mask */
static inline int set_leds(struct tty_driver* handler, unsigned int mask)
{
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return (handler->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, mask);
#else
    return (handler->ops->ioctl) (vc_cons[fg_console].d->vc_tty, NULL, KDSETLED, mask);
#endif
}

int init_module(void) {
    printk(KERN_INFO "Cargamos el modulo chardev_leds\n");
    int major;		/* Major number assigned to our device driver */
    int minor;		/* Minor number assigned to the associated character device */
    int ret;

    /* Get available (major,minor) range */
    if ((ret = alloc_chrdev_region(&start, 0, 1, DEVICE_NAME))) {
        printk(KERN_INFO "Can't allocate chrdev_region()");
        return ret;
    }

    /* Create associated cdev */
    if ((chardev = cdev_alloc()) == NULL) {
        printk(KERN_INFO "cdev_alloc() failed ");
        unregister_chrdev_region(start, 1);
        return -ENOMEM;
    }

    cdev_init(chardev, &fops); /*asociar operaciones al dispositivo*/


    /* Asociamos interfaz de operaciones del driver a la estructura cdev */
    if ((ret = cdev_add(chardev, start, 1))) {
        printk(KERN_INFO "cdev_add() failed ");
        kobject_put(&chardev->kobj);
        unregister_chrdev_region(start, 1);
        return ret;
    }

    kbd_driver = get_kbd_driver_handler();

    major = MAJOR(start);
    minor = MINOR(start);

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'sudo mknod -m 666 /dev/%s c %d %d'.\n", DEVICE_NAME, major, minor);
    printk(KERN_INFO "Try to cat and echo to the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}
void cleanup_module(void) {
    printk(KERN_INFO "Des-cargamos el modulo\n");

    /* Destroy chardev */
    if (chardev)
        cdev_del(chardev);

    /*
     * Unregister the device
     */
    unregister_chrdev_region(start, 1);
}




static int device_open(struct inode* inode, struct file* file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++; /* Estamos ocupando el driver */

    printk(KERN_INFO "Dispositivo abierto\n");

    /* Increase the module's reference counter */
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode* inode, struct file* file)
{
    Device_Open--;		/* We're now ready for our next caller */

    module_put(THIS_MODULE);
    printk(KERN_INFO "Dispositivo cerrado\n");

    return 0;
}

static ssize_t device_read(struct file* filp,	/* see include/linux/fs.h   */
    char* buffer,	/* buffer to fill with data */
    size_t length,	/* length of the buffer     */
    loff_t* offset)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EPERM;
}

static ssize_t
device_write(struct file* filp, const char* buff, size_t len, loff_t* off)
{

    if (get_current_user()->uid.val != 0) {
        return -EPERM;
    }
    printk(KERN_INFO "Escribiendo en dispositivo");

    lectura = (char*)vmalloc(len);

    if (lectura == NULL) {
        return -EMSGSIZE;
    }

    if (copy_from_user(lectura, buff, len)) {
        return -EFAULT;
    }

    int numLock = 0;
    int capsLock = 0;
    //mi ordenador no dispone de scroll lock
    int i = 0;
    int er = 0;
    for (i; i < len; ++i) {
        switch (*lectura) {
        case '1': numLock = 2; break;
        case '2': capsLock = 4; break;
        case '3':  break; // no tengo este led 
        default: er = er + 1; break;
        }
        lectura++;
    }

    if (er == 1) {
        unsigned int mascara = numLock + capsLock;
        set_leds(kbd_driver, mascara);
    }

    else {
        return -EINVAL;
    }

    return i;
}
