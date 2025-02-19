#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/irqdomain.h>

#define MY_IRQ_NUMBER 1  // 假设我们想查询的中断号为 5

static int __init my_module_init(void)
{
	struct irq_desc *desc;
	struct irq_data *irq_data;
	irq_hw_number_t hwirq;
	unsigned int virq = MY_IRQ_NUMBER;

	// 获取中断描述符
	desc = irq_to_desc(virq);
	if (!desc) {
		pr_err("Failed to get IRQ descriptor for virq: %u\n", virq);
		return -EINVAL;
	}

	// 获取 irq_data
	irq_data = &desc->irq_data;

	// 获取硬件中断号 (hwirq)
	hwirq = irq_data->hwirq;

	// 打印中断信息
	pr_info("virq: %u, hwirq: %lu\n", virq, hwirq);

	// 获取中断域并打印域信息
	if (irq_data->domain) {
		pr_info("IRQ domain name: %s\n", irq_data->domain->name);
	} else {
		pr_warn("No IRQ domain found for virq: %u\n", virq);
	}

	return 0;
}

static void __exit my_module_exit(void)
{
	pr_info("Module exited\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module to get IRQ information");
