// Example test driver:
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/leds.h>
#include <linux/kfifo.h>
#include <linux/uaccess.h>
//#error Are we building this file?

#define MY_DEVICE_FILE  "morse-code"
#define DOT_TIME_MS 200
#define DASH_TIME_MS 3 * DOT_TIME_MS
#define ASCII2LETTER -65
// Morse Code Encodings (from http://en.wikipedia.org/wiki/Morse_code)
//   Encoding created by Brian Fraser. Released under GPL.
//
// Encoding description:
// - msb to be output first, followed by 2nd msb... (left to right)
// - each bit gets one "dot" time.
// - "dashes" are encoded here as being 3 times as long as "dots". Therefore
//   a single dash will be the bits: 111.
// - ignore trailing 0's (once last 1 output, rest of 0's ignored).
// - Space between dashes and dots is one dot time, so is therefore encoded
//   as a 0 bit between two 1 bits.
//
// Example:
//   R = dot   dash   dot       -- Morse code
//     =  1  0 111  0  1        -- 1=LED on, 0=LED off
//     =  1011 101              -- Written together in groups of 4 bits.
//     =  1011 1010 0000 0000   -- Pad with 0's on right to make 16 bits long.
//     =  B    A    0    0      -- Convert to hex digits
//     = 0xBA00                 -- Full hex value (see value in table below)
//
// Between characters, must have 3-dot times (total) of off (0's) (not encoded here)
// Between words, must have 7-dot times (total) of off (0's) (not encoded here).
//
static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1
		0xEA80,	// B 1110 1010 1
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0	// Z 1110 1110 101
};

static int end_sleep_time_modifier = 1;

/******************************************************
 * FIFO
 ******************************************************/

// KFIFO usage based off of these two examples
//https://github.com/atomic-penguin/linux/blob/master/samples/kfifo/bytestream-example.c
//https://docs.huihoo.com/doxygen/linux/kernel/3.7/inttype-example_8c_source.html
#define FIFO_SIZE	512
static DECLARE_KFIFO(kfifo_read, char, FIFO_SIZE);


/******************************************************
 * LED
 ******************************************************/
// from demo_ledtrig.c
DEFINE_LED_TRIGGER(ledtrig);

static void my_led_dot(void) {
	led_trigger_event(ledtrig, LED_FULL);
	msleep(DOT_TIME_MS);
	led_trigger_event(ledtrig, LED_OFF);
	msleep(DOT_TIME_MS * end_sleep_time_modifier); // '0' after ones
}

static void my_led_dash(void) {
    led_trigger_event(ledtrig, LED_FULL);
	msleep(DASH_TIME_MS);
	led_trigger_event(ledtrig, LED_OFF);
	msleep(DOT_TIME_MS * end_sleep_time_modifier); // '0' after ones
}

static void my_led_blink(short letter)
{
	int i = 0;
    int ones = 0;
    short value;
    // MSB at highest address, start at 15.
    for (i = 15; i >= 0; i--) {
        value = (morsecode_codes[letter] & (1 << i)) >> i;
        if (value == 0) {
            if (ones == 3) { // dash
                kfifo_put(&kfifo_read, '-');
                my_led_dash();
                ones = 0;
            }
            else if (ones == 1) { //dot
                kfifo_put(&kfifo_read, '.');
                my_led_dot();
                ones = 0;
            }
        }
        else {
            ones++;
        }
    }

    
}

static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("morse-code", &ledtrig);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig);
}



// From demo_misc_template.c
/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_read(struct file *file,
		char *buff,	size_t count, loff_t *ppos)
{
    int bytesRead = 0;
    if (kfifo_to_user(&kfifo_read, buff, count, &bytesRead)) {
		return -EFAULT;
	}

    return bytesRead;
}

static ssize_t my_write(struct file *file,
		const char *buff, size_t count, loff_t *ppos)
{
    char to = 0;
    int i = 0;
    short morseLetter = 0;
    int messageStart = 0;
    int prevIsSpace = 0;
    char end = 0;
    int preprocessIdx = 0;
    end_sleep_time_modifier = 1;

    for (preprocessIdx = 0; preprocessIdx < count; preprocessIdx++) {
        if (copy_from_user(&end, &buff[i], sizeof(char))) {
            printk(KERN_ERR "Unable to read from buffer.");
            return -EFAULT;    
        }

        if (end == 0) { // null terminated string
            printk(KERN_INFO "In my_write() null terminator found\n");
            break;
        }
    }

    if (preprocessIdx > 0) {
        preprocessIdx -= 1;
    }

    for (i = 0; i < count; i++) {
        //unsigned long __copy_from_user(void * to, const void __user * from, unsigned long n); (from man page)
        //to Destination address, in kernel space.
        //from Source address, in user space.
        //n Number of bytes to copy
        if (copy_from_user(&to, &buff[i], sizeof(char))) {
            printk(KERN_ERR "Unable to read from buffer.");
            return -EFAULT;    
        }
        printk(KERN_INFO "In my_write() doing writing\n");


        if (i == preprocessIdx) {
            end_sleep_time_modifier = 0; //last element of the string, make it so there is no hanging sleep at the end
        }

        if ((to == ' ' && messageStart == 1) || (to >= 65 && to <= 90) || (to >= 97 && to <= 122)){
            // based off of https://www.tutorialspoint.com/c-program-for-lowercase-to-uppercase-and-vice-versa
            if (to >= 'a' && to <= 'z') {
                to -= 32;
            }
            
            if (to == ' ' && prevIsSpace == 1) {
                continue;
            }

            morseLetter = to + ASCII2LETTER;

            if (to == ' ') {
                // to is a proper space, wait for 7 dottimes and make sure LED is off
                // there is one dottime after every character, so 6 will ensure that it adds up to 7
                kfifo_put(&kfifo_read, ' ');
                kfifo_put(&kfifo_read, ' ');
                kfifo_put(&kfifo_read, ' ');
                led_trigger_event(ledtrig, LED_OFF);
                prevIsSpace = 1;
                msleep(6 * DOT_TIME_MS * end_sleep_time_modifier);
            }
            else {
                // otherwise go for it
                if (messageStart == 1 && prevIsSpace == 0) {    
                    kfifo_put(&kfifo_read, ' ');
                }
                my_led_blink(morseLetter);
                prevIsSpace = 0;
                msleep(2 * DOT_TIME_MS * end_sleep_time_modifier);
            }


            if (messageStart == 0) {
                messageStart = 1;
            }

        }
        
        if (to != ' ') {
            messageStart = 1;
        }



    }

    *ppos += count;
	return count;  // # bytes actually read.
}

/****************************************************/


/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in <kernel>/include/linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
    .read     =  my_read,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init my_init(void) {
    int ret;
	printk(KERN_INFO "----> morsecode driver my_init(): file /dev/%s.\n", MY_DEVICE_FILE);
    INIT_KFIFO(kfifo_read);
    ret = misc_register(&my_miscdevice);
    led_register();
    return ret; 
}

static void __exit my_exit(void) {
    printk(KERN_INFO "----> morsecode driver my_exit(): file /dev/%s.\n", MY_DEVICE_FILE);
    misc_deregister(&my_miscdevice);
    led_unregister();
}


/******************************************************/

// Link our init/exit functions into the kernel's code.

module_init(my_init);
module_exit(my_exit);


// Information about this module:
MODULE_AUTHOR("Orion Hsu");
MODULE_DESCRIPTION("Morse Code LED Driver - CMPT 433 Assignment 4.");
MODULE_LICENSE("GPL");// Important to leave as GPL