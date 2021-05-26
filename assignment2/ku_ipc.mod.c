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
	{ 0xfe990052, "gpio_free" },
	{ 0x999e8297, "vfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x1d11aa3a, "cdev_del" },
	{ 0x16e02b99, "gpiod_get_raw_value" },
	{ 0x366ed94b, "gpiod_direction_input" },
	{ 0xffb0dcd9, "gpiod_set_raw_value" },
	{ 0xe31620ea, "gpiod_direction_output_raw" },
	{ 0x350a004, "gpio_to_desc" },
	{ 0x8899dc97, "kmem_cache_alloc_trace" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0x61a76852, "kmalloc_caches" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x5bbe49f4, "__init_waitqueue_head" },
	{ 0x88647e86, "cdev_add" },
	{ 0x7a3acc89, "cdev_init" },
	{ 0xb4673642, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0xdb9ca3c5, "_raw_spin_lock" },
	{ 0x7c32d0f0, "printk" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "41A82652437C0CF357DA742");
