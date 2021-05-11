#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xad1a7def, "module_layout" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0xffb0dcd9, "gpiod_set_raw_value" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x72c365ef, "gpiod_to_irq" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x16e02b99, "gpiod_get_raw_value" },
	{ 0x350a004, "gpio_to_desc" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb43f9365, "ktime_get" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x2196324, "__aeabi_idiv" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2368F4522A1F1C55C6FB5C4");
