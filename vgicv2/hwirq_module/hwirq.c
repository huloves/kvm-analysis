#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/irqdomain.h>

static struct kprobe kp_translate, kp_find_mapping;
static unsigned long hwirq_value;
static int virq_value;

// 捕获 irq_domain_translate 的 hwirq
static int handler_irq_domain_translate_pre(struct kprobe *p, struct pt_regs *regs)
{
	// ARM64 参数约定: x0=domain, x1=fwspec, x2=hwirq指针
	struct irq_domain *domain = (struct irq_domain *)regs->regs[0];
	struct irq_fwspec *fwspec = (struct irq_fwspec *)regs->regs[1];
	irq_hw_number_t *hwirq = (irq_hw_number_t *)regs->regs[2];

	if (domain && hwirq) {
		pr_info("irq_domain_translate: domain=%px, hwirq=%lu\n", domain, *hwirq);
		hwirq_value = *hwirq;  // 记录 hwirq
	}
	return 0;
}

// 捕获 irq_find_mapping 的返回值 virq
static void handler_irq_find_mapping_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	// ARM64 返回值约定: x0=virq
	virq_value = (int)regs->regs[0];
	pr_info("irq_find_mapping: hwirq=%lu -> virq=%d\n", hwirq_value, virq_value);
}

static int __init kprobe_init(void)
{
	// 设置 irq_domain_translate 的 kprobe
	kp_translate.symbol_name = "irq_domain_translate";
	kp_translate.pre_handler = handler_irq_domain_translate_pre;
	if (register_kprobe(&kp_translate)) {
		pr_err("Failed to register kprobe for irq_domain_translate\n");
		return -1;
	}

	// 设置 irq_find_mapping 的 kretprobe
	kp_find_mapping.symbol_name = "irq_find_mapping";
	kp_find_mapping.post_handler = handler_irq_find_mapping_post;
	if (register_kprobe(&kp_find_mapping)) {
		pr_err("Failed to register kprobe for irq_find_mapping\n");
		unregister_kprobe(&kp_translate);
		return -1;
	}

	pr_info("Kprobes registered\n");
	return 0;
}

static void __exit kprobe_exit(void)
{
	unregister_kprobe(&kp_translate);
	unregister_kprobe(&kp_find_mapping);
	pr_info("Kprobes unregistered\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");
