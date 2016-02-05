#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xef025c67, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x72efa1d5, __VMLINUX_SYMBOL_STR(netlink_kernel_release) },
	{ 0x851751e1, __VMLINUX_SYMBOL_STR(__netlink_kernel_create) },
	{ 0x72ee5686, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x5c7fa529, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
	{ 0xa0dfc78a, __VMLINUX_SYMBOL_STR(netlink_unicast) },
	{ 0xc52b7068, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5AC5B724BEAC9ADEF46D1BB");
