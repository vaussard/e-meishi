#include <zephyr/kernel.h>

int main(void)
{
	printk("Zephyr e-meishi\n");

	while (true) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
