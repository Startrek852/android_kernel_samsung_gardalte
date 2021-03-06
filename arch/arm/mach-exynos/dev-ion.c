/* linux/arch/arm/mach-exynos/dev-ion.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/ion.h>
#include <linux/exynos_ion.h>
#include <linux/slab.h>

static struct platform_device exynos_device_ion = {
	.name		= "ion-exynos",
	.id		= -1,
};

static void __init exynos_ion_set_platdata(void)
{
	struct ion_platform_data *pdata;
	pdata = kzalloc(sizeof(struct ion_platform_data)
			+ 5 * sizeof(struct ion_platform_heap), GFP_KERNEL);
	if (pdata) {
		pdata->nr = 4;
		pdata->heaps[0].type = ION_HEAP_TYPE_SYSTEM;
		pdata->heaps[0].name = "ion_noncontig_heap";
		pdata->heaps[0].id = EXYNOS_ION_HEAP_SYSTEM_ID;
		pdata->heaps[1].type = ION_HEAP_TYPE_SYSTEM_CONTIG;
		pdata->heaps[1].name = "ion_contig_heap";
		pdata->heaps[1].id = EXYNOS_ION_HEAP_SYSTEM_CONTIG_ID;
		pdata->heaps[2].type = ION_HEAP_TYPE_EXYNOS;
		pdata->heaps[2].name = "exynos_noncontig_heap";
		pdata->heaps[2].id = EXYNOS_ION_HEAP_EXYNOS_ID;
		pdata->heaps[3].type = ION_HEAP_TYPE_EXYNOS_CONTIG;
		pdata->heaps[3].name = "exynos_contig_heap";
		pdata->heaps[3].id = EXYNOS_ION_HEAP_EXYNOS_CONTIG_ID;
		exynos_device_ion.dev.platform_data = pdata;
	}
}

static int __register_ion(void)
{
	exynos_ion_set_platdata();
	return platform_device_register(&exynos_device_ion);
}
subsys_initcall(__register_ion);
